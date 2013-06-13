#!/usr/bin/env python

import time
import threading
import os
import getpass
import sys
import struct
import socket
import argparse
import select

import logging
logging.basicConfig(level=logging.DEBUG)

class Bus(object):
    class BusError(Exception):
        pass

    class ConnectionClosedError(BusError):
        def __init__(self, conn):
            self.conn = conn

    def __init__(self):
        raise NotImplementedError, "Abstract Base Class"

    def join(self):
        '''
        This method is called to run until the bus dies.

        The join() function will join one of the Bus object's main threads, not
        returning unless the Bus object is destroyed.
        '''
        raise NotImplementedError, "Abstract Base Class"

    def create(self, port):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        path = '/tmp/' + getpass.getuser() + '.M-ulator.bus.' + str(port)
        try:
            os.remove(path)
        except OSError:
            pass
        self.s.bind(path)
        self.s.listen(1)

    def recv_all(self, conn, length):
        r = ''
        while len(r) < length:
            n = conn.recv(length-len(r))
            if n == '':
                raise self.ConnectionClosedError(conn)
            r += n
        assert len(r) == length
        return r

class I2CBus(Bus):
    INFO = '''
I2C BUS {i}

    A bus that acts like an I2C bus. Messages are sent as complete transactions
    instead of individual bits.  The I2C bus interface defines two packet types:

        struct message {
            char type = 0
            uint8_t address     # The destination address
            uint32_t length     # The message length (network byte order)
            char bytes[...]     # The message, starting with byte 0...byte N
        }

        struct acknowledge {
            char type = 1
            char acked?         # Bool, True if ACK, False if NACK
        }

    Only one transaction is permitted to be active at any given time. As in I2C,
    clients should be prepared for a 'lost aribtration' case. If a client is
    idle, it may be sent a message at any time. If the bus has received a
    message from a client, it will be responded to by an acknowledge packet if
    the client won arbitration or a message packet if the client lost
    arbitration. If a client loses arbitration, it is responsible for re-sending
    the message and responding appropriately to the message it received instead.

    Unlike real I2C, all clients must explicitly ACK or NAK every message. Once
    all clients have responded a single ACK/NAK message is returned to the
    original caller. Clients have a 1 second timeout to respond to the message,
    after which they are disconnected.

    Options:

        -a, --address  A string of the form '1001xx1x' the bus should ACK
                       automatically. 1's and 0's must match, x's are don't care
'''

    @staticmethod
    def add_parsers(p):
        p.add_argument('-a', '--address', type=str,
                help="address mask of the form '10x0xxx'")
        return I2CBus.INFO


    class I2CBusError(Bus.BusError):
        pass

    def __init__(self, port, address):
        logging.info("Creating an I2C Bus on port " + str(port))
        self.create(port)

        self.connection_lock = threading.Lock()

        self.accept_thread = threading.Thread(target=self.accept)
        self.accept_thread.daemon = True
        self.accept_thread.start()

        self.connections = {}
        self.conn_thread = threading.Thread(target=self.connection)
        self.conn_thread.daemon = True
        self.conn_thread.start()

        self.connections = {}
        self.conn_thread = None

        if address is None:
            # Disable (bit 0 cannot be both 0 and 1)
            self.ones = 1
            self.zeros = 1
        else:
            self.ones = 0
            self.zeros = 0
            idx = 8
            for c in address:
                idx -= 1
                if c == '1':
                    self.ones |= (1 << idx)
                elif c == '0':
                    self.zeros |= (1 << idx)
                elif c in ('x', 'X', ' '):
                    continue
                else:
                    raise self.I2CBusError,\
                        "Illegal character in address mask: >>>" + c + "<<<"

    def join(self, timeout=None):
        self.accept_thread.join(timeout)

    def accept(self):
        while True:
            conn, addr = self.s.accept()
            logging.info("New connection from " + str(addr) + " on " + str(conn))
            threading.Thread(target=self.new_connection, args=((conn,addr))).start()

    def new_connection(self, conn, addr):
        t = self.recv_all(conn, 1)
        if t != 'i':
            logging.warn("Bad handshake, dropping connection from " + str(addr))
            logging.warn("Expected 'i' got '%c'" % (t))
            conn.close()
            return

        conn.send('i')
        logging.debug("Handshake complete")
        with self.connection_lock:
            self.connections[conn] = addr

    def recv_packet(self, conn):
        t = self.recv_all(conn, 1)
        t = struct.unpack("!B", t)[0]
        if (t == 0):
            # Message type
            address = struct.unpack("!B", self.recv_all(conn, 1))[0]
            length = struct.unpack("!I", self.recv_all(conn, 4))[0]
            msg = self.recv_all(conn, length)
            return (t, address, length, msg)
        elif (t == 1):
            # Acknowledge type
            acked = bool(self.recv_all(conn, 1))
            return (t, acked)
        else:
            raise TypeError, "Unknown packet type: " + str(t) + " " + str(ord(t))

    def send_packet(self, conn, p):
        if p[0] == 0:
            msg = struct.pack("!BBI", p[0], p[1], p[2]) + p[3]
        elif p[0] == 1:
            msg = struct.pack("!BB", *p)
        else:
            raise TypeError, "Unknown packet type: " + str(t) + " " + str(ord(t))
        conn.send(msg)

    def handle_message(self, conn):
        p = self.recv_packet(conn)
        if p[0] != 0:
            logging.debug("Got packet: " + str(p))
            raise RuntimeError, "Unexpected ACK-type packet?"
        logging.debug("Got packet: MSG 0x%x %d: %s" % (p[1], p[2],
            " ".join(p[3].encode('hex')[i:i+2] for i in range(0,
                len(p[3].encode('hex')), 2))))

        with self.connection_lock:
            # Broadcast message
            for c in self.connections.keys():
                if c == conn:
                    continue
                self.send_packet(c, p)
            logging.debug("Finished Broadcast")

            if (self.ones & p[1]) and (~self.zeros & ~p[1]):
                logging.debug("Bus will ACK this packet")
                acked = True
            else:
                acked = False

            # Collect ACK/NAKs
            for c in self.connections.keys():
                if c == conn:
                    continue
                while True:
                    p = self.recv_packet(c)
                    if p[0] == 0:
                        break
                    elif p[0] == 1:
                        logging.debug(str(c) + " lost arbitration, msg: " +
                                str(p))
                        continue
                    else:
                        raise TypeError, "Unknown packet type: " + str(t)
                acked |= p[1]

            logging.debug("Got all ACK/NAKs, sending " + str(acked))

            # Broadcast ACK/NAK
            for c in self.connections.keys():
                self.send_packet(c, (1, acked))

    def connection(self):
        while True:
            try:
                r = select.select(self.connections.keys(), (), (), 1.0)
                if len(r[0]) == 0:
                    continue

                # More I2C-like arbitration? Is that really possible / necessary?
                conn = r[0][0]
                self.handle_message(conn)
            except self.ConnectionClosedError as e:
                logging.info("Connection " + str(self.connections[e.conn]) + " dropped")
                with self.connection_lock:
                    del(self.connections[e.conn])
            except:
                logging.exception('Got exception on connection thread')
                time.sleep(.1)


parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter)
parser.description = '''
Create a bus/bus-like interface.

This tool instantiates a software bus process. This process links one to many
software instances onto a common bus and optionally interfaces them with a
hardware bus (requires external hardware supprot).

The following bus types are currently available:
'''
parser.add_argument("BUS", help="Bus type to create")
parser.add_argument("PORT", help="Port to listen on")
parser.description += I2CBus.add_parsers(parser)

args = parser.parse_args()
args.BUS = args.BUS.upper()

if args.BUS == "I2C":
    bus = I2CBus(int(args.PORT), args.address)
else:
    logging.error("Unknown BUS type: " + args.BUS)
    print parser.description
    sys.exit(1)

while True:
    # Some responsiveness on the main thread required to catch SIGINT
    bus.join(1)
