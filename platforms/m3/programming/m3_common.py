#!/usr/bin/env python

from __future__ import print_function

import atexit
import os
import sys
import socket
from Queue import Queue
import argparse
import time
import threading

from m3_logging import get_logger
logger = get_logger(__name__)
logger.debug('Got m3_common.py logger')

from ice import ICE

# Do this after ICE since ICE prints a nice help if pyserial is missing
import serial.tools.list_ports

def printing_sleep(seconds):
    try:
        os.environ['ICE_NOSLEEP']
        return
    except KeyError:
        pass
    if seconds < 1:
        time.sleep(seconds)
        return
    while (seconds > 0):
        sys.stdout.write("\rSleeping %d s" % (int(seconds)) + ' '*20)
        sys.stdout.flush()
        time.sleep(min(1, seconds))
        seconds -= 1
    sys.stdout.write('\r' + ' '*80 + '\r')
    sys.stdout.flush()

class m3_common(object):
    TITLE = "Generic M3 Programmer"

    def default_value(self, prompt, default, extra=None, invert=False):
        if invert and (extra is None):
            raise RuntimeError, "invert & !extra ?"
        if self.args.yes:
            fn = print
        else:
            fn = raw_input
        if extra:
            r = fn(prompt + ' [' + default + extra + ']: ')
        else:
            r = fn(prompt + ' [' + default + ']: ')
        if self.args.yes:
            if invert:
                print("Chose {}".format(extra))
                return extra
            print("Chose {}".format(default))
            return default
        if len(r) == 0:
            if invert:
                return extra
            return default
        else:
            return r

    def do_default(self, prompt, fn, else_fn=None):
        y = self.default_value(prompt, 'Y', '/n')
        if y[0] not in ('n', 'N'):
            fn()
        else:
            if else_fn:
                else_fn()

    def dont_do_default(self, prompt, fn, else_fn=None):
        resp = self.default_value(prompt, 'y/', 'N', invert=True)
        if resp[0] in ('y', 'Y'):
            fn()
        else:
            if else_fn:
                else_fn()

    @staticmethod
    def _build_injection_message(
            # Byte 0: Control
            chip_id_mask=0,             # [0:3] Chip ID Mask
            reset_request=0,            #   [4] Reset Request
            chip_id_coding=0,           #   [5] Chip ID coding
            is_mbus=0,                  #   [6] Indicates transmission is MBus message [addr+data]
            run_after=False,            #   [7] Run code after programming?

            # Byte 1,2: Chip ID
            chip_id = 0,

            # Byte 3,4: Memory Address
            memory_address=0,

            # Data to send
            hexencoded_data=None,

            # GOC Version
            goc_version=0,              # Illegal version by default
            ):
        if goc_version == 0:
            raise NotImplementedError("Bad GOC Version?")

        HEADER = ''

        # Control Byte
        control = chip_id_mask |\
                (reset_request << 4) |\
                (chip_id_coding << 5) |\
                (is_mbus << 6) |\
                (run_after << 7)
        HEADER += "%02X" % (control)

        # Chip ID
        HEADER += "%04X" % (chip_id)

        # Memory Address
        if goc_version == 1:
            HEADER += "%04X" % (memory_address)

        # Program Lengh
        if hexencoded_data is not None:
            length = len(hexencoded_data) >> 3   # hex exapnded -> bytes, /2
            length = socket.htons(length)
        else:
            length = 0
        HEADER += "%04X" % (length)

        # Bit-wise XOR parity of header
        header_parity = 0
        for byte in [HEADER[x:x+2] for x in xrange(0, len(HEADER), 2)]:
            byte = int(byte, 16)
            header_parity ^= byte
        HEADER += "%02X" % (header_parity)

        DATA = ''
        if hexencoded_data is not None:
            if goc_version == 2:
                DATA += "%08X" % (memory_address)

            DATA += hexencoded_data

            # Bit-wise XOR parity of data
            data_parity = 0
            for byte in [DATA[x:x+2] for x in xrange(0, len(DATA), 2)]:
                b = int(byte, 16)
                data_parity ^= b

            if goc_version == 1:
                DATA = '%02X' % (data_parity) + DATA
            else:
                DATA += '%02X' % (data_parity)

        return HEADER + DATA

    @staticmethod
    def build_injection_message(**kwargs):
        return m3_common._build_injection_message(goc_version=1, **kwargs)

    @staticmethod
    def build_injection_message_interrupt(hexencoded, run_after=True):
        return m3_common.build_injection_message(
                hexencoded_data=hexencoded,
                run_after=run_after,
                memory_address=0x1A00,
                )

    @staticmethod
    def build_injection_message_custom(mem_addr_custom, hexencoded, run_after):
        return m3_common.build_injection_message(
                hexencoded_data=hexencoded,
                run_after=run_after,
                memory_address=mem_addr_custom,
                )

    @staticmethod
    def build_injection_message_mbus(mbus_addr, mbus_data, run_after=False):
        chip_id_mask = 0                # [0:3] Chip ID Mask
        reset = 0                       #   [4] Reset Request
        chip_id_coding = 0              #   [5] Chip ID coding
        is_i2c = 1                      #   [6] Indicates transmission is I2C message [addr+data]
        run_after = not not run_after   #   [7] Run code after programming?
        # Byte 0: Control
        control = chip_id_mask | (reset << 4) | (chip_id_coding << 5) | (is_i2c << 6) | (run_after << 7)

        # Byte 1,2: Chip ID
        chip_id = 0

        # Byte 3,4,5,6: MBus Address
        i2c_addr = mbus_addr

        # Byte 7,8,9,10: MBus Data
        i2c_data = mbus_data

        # Byte 11: bit-wise XOR parity of header
        header_parity = 0
        for byte in (
                control,
                (chip_id >> 8) & 0xff,
                chip_id & 0xff,
                (i2c_addr >> 24) & 0xff,
                (i2c_addr >> 16) & 0xff,
                (i2c_addr >> 8) & 0xff,
                i2c_addr & 0xff,
                (i2c_data >> 24) & 0xff,
                (i2c_data >> 16) & 0xff,
                (i2c_data >> 8) & 0xff,
                i2c_data & 0xff,
                ):
            header_parity ^= byte

        # Assemble message:
        message = "%02X%04X%08X%08X%02X" % (
                control,
                chip_id,
                i2c_addr,
                i2c_data,
                header_parity)

        return message

    @staticmethod
    def build_reset_req_message():
        return m3_common.build_injection_message(
                hexencoded_data = "00000000",
                run_after = 0,
                memory_address = 0x000,
                reset_request = True,
                )

    @staticmethod
    def build_injection_message_mbus(mbus_addr, mbus_data, run_after=False):
        is_i2c = 1                      #   [6] Indicates transmission is I2C message [addr+data]
        # Byte 3,4,5,6: MBus Address
        i2c_addr = mbus_addr
        # Byte 7,8,9,10: MBus Data
        i2c_data = mbus_data
        
        # Byte 11: bit-wise XOR parity of header
        header_parity = 0
        for byte in (
                control,
                (chip_id >> 8) & 0xff,
                chip_id & 0xff,
    	    (i2c_addr >> 24) & 0xff,
    	    (i2c_addr >> 16) & 0xff,
    	    (i2c_addr >> 8) & 0xff,
    	    i2c_addr & 0xff,
    	    (i2c_data >> 24) & 0xff,
    	    (i2c_data >> 16) & 0xff,
    	    (i2c_data >> 8) & 0xff,
    	    i2c_data & 0xff,
                ):
            header_parity ^= byte
    
        # Assemble message:
        message = "%02X%04X%08X%08X%02X" % (
                control,
                chip_id,
                i2c_addr,
                i2c_data,
                header_parity)

        return message

    @staticmethod
    def build_injection_message_for_goc_v2(**kwargs):
        return m3_common._build_injection_message(goc_version=2, **kwargs)


    @staticmethod
    def build_injection_message_interrupt_for_goc_v2(hexencoded, run_after=True):
        return m3_common.build_injection_message_for_goc_v2(
                hexencoded_data=hexencoded,
                run_after=run_after,
                memory_address=0x1A00,
                )

    def __init__(self):
        self.wait_event = threading.Event()

        try:
            self.print_banner()
            self.parse_args()
            self.read_binfile()
            self.ice = ICE()
            self.callback_q = Queue()
            self.install_handler()
            self.ice.connect(self.serial_path)
            self.wakeup_goc_circuit()
        except NameError:
            logger.error("Abstract element missing.")
            raise

        atexit.register(self.exit_handler)

    def exit_handler(self):
        try:
            if self.args.wait_for_messages:
                self.hang_for_messages()
        except AttributeError:
            pass

    def wakeup_goc_circuit(self):
        # Fix an ICE issue where the power rails must be poked for
        # the GOC circuitry to wake up
        self.ice.goc_set_onoff(False)
        self.ice.power_set_onoff(self.ice.POWER_GOC, True)

    def install_handler(self):
        self.ice.msg_handler[self.MSG_TYPE] = self.callback_helper

    def callback_helper(self, msg_type, event_id, length, msg):
        logger.debug("Callback: msg len " + str(len(msg)))
        if len(msg) == 0:
            logger.debug("Ignore msg of len 0")
            return
        callback_q.put(msg)

    def print_banner(self):
        logger.info("-" * 80)
        logger.info(" -- " + self.TITLE)
        logger.info("")

    def add_parse_args(self, require_binfile=True):
        if require_binfile:
            self.parser.add_argument("BINFILE", help="Program to flash")
        self.parser.add_argument("SERIAL", help="Path to ICE serial device", nargs='?')

        self.parser.add_argument('-w', '--wait-for-messages',
                action='store_true',
                help="Wait for messages (hang) when done.")

        self.parser.add_argument('-y', '--yes',
                action='store_true',
                help="Use default values for all prompts.")


    def parse_args(self):
        self.parser = argparse.ArgumentParser()
        self.add_parse_args()

        self.args = self.parser.parse_args()
        if self.args.SERIAL is None:
            self.serial_path = self.guess_serial()
        else:
            self.serial_path = self.args.SERIAL

    @staticmethod
    def get_serial_candidates():
        candidates = []
        for s in serial.tools.list_ports.comports():
            s = s[0]
            if 'bluetooth' in s.lower():
                continue
            candidates.append(s)
        # In many cases when debugging, we'll be using the fake_ice at '/tmp/com1'
        if os.path.exists('/tmp/com1'):
            candidates.append('/tmp/com1')
        return candidates

    def guess_serial(self):
        candidates = self.get_serial_candidates()
        if len(candidates) == 0:
            logger.error("Could not find the serial port ICE is attached to.\n")
            self.parser.print_help()
            sys.exit(1)
        elif len(candidates) == 1:
            logger.info("Guessing ICE is at: " + candidates[0])
            return candidates[0]
        else:
            def pick_serial():
                logger.info("Multiple possible serial ports found:")
                for i in xrange(len(candidates)):
                    logger.info("\t[{}] {}".format(i, candidates[i]))
                try:
                    resp = raw_input("Choose a serial port (Ctrl-C to quit): ").strip()
                except KeyboardInterrupt:
                    sys.exit(1)
                try:
                    return candidates[int(resp)]
                except:
                    logger.info("Please choose one of the available serial ports.")
                    return pick_serial()
            return pick_serial()

    @staticmethod
    def read_binfile_static(binfile):
        def guess_type_is_hex(binfile):
            for line in open(binfile):
                for c in line.strip():
                    c = ord(c)
                    if c < 0x20 or c > 0x7a:
                        return False
            return True

        if guess_type_is_hex(binfile):
            binfd = open(binfile, 'r')
            hexencoded = ""
            for line in binfd:
                hexencoded += line[0:2].upper()
        else:
            binfd = open(binfile, 'rb')
            hexencoded = binfd.read().encode("hex").upper()

        if (len(hexencoded) % 4 == 0) and (len(hexencoded) % 8 != 0):
            # Image is halfword-aligned. Some tools generate these, but our system
            # assumes things are word-aligned. We pad an extra nop to the end to fix
            hexencoded += '46C0' # nop; (mov r8, r8)

        if (len(hexencoded) % 8) != 0:
            logger.warn("Binfile is not word-aligned. This is not a valid image")
            return None

        return hexencoded

    def read_binfile(self):
        self.hexencoded = m3_common.read_binfile_static(self.args.BINFILE)
        if self.hexencoded is None:
            sys.exit(3)

    def power_on(self, wait_for_rails_to_settle=True):
        logger.info("Turning all M3 power rails on")
        self.ice.power_set_voltage(0,0.6)
        self.ice.power_set_voltage(1,1.2)
        self.ice.power_set_voltage(2,3.8)
        logger.info("Turning 3.8 on")
        self.ice.power_set_onoff(2,True)
        if wait_for_rails_to_settle:
            printing_sleep(1.0)
        logger.info("Turning 1.2 on")
        self.ice.power_set_onoff(1,True)
        if wait_for_rails_to_settle:
            printing_sleep(1.0)
        logger.info("Turning 0.6 on")
        self.ice.power_set_onoff(0,True)
        if wait_for_rails_to_settle:
            printing_sleep(1.0)
            logger.info("Waiting 8 seconds for power rails to settle")
            printing_sleep(8.0)

    def reset_m3(self):
        logger.info("M3 0.6V => OFF (reset controller)")
        self.ice.power_set_onoff(0,False)
        printing_sleep(2.0)
        logger.info("M3 0.6V => ON")
        self.ice.power_set_onoff(0,True)
        printing_sleep(2.0)

    def validate_bin(self): #, hexencoded, offset=0):
        raise NotImplementedError("Need to update for MBus. Let me know if needed.")
        logger.info("Configuring ICE to ACK adress 1001 100x")
        ice.i2c_set_address("1001100x") # 0x98

        logger.info("Running Validation sequence:")
        logger.info("\t DMA read at address 0x%x, length %d" % (offset, len(hexencoded)/2))
        logger.info("\t<Receive I2C message for DMA data>")
        logger.info("\tCompare received data and validate it was programmed correctly")

        length = len(hexencoded)/8
        offset = offset
        data = 0x80000000 | (length << 16) | offset
        dma_read_req = "%08X" % (socket.htonl(data))
        logger.debug("Sending: " + dma_read_req)
        ice.i2c_send(0xaa, dma_read_req.decode('hex'))

        logger.info("Chip Program Dump Response:")
        chip_bin = validate_q.get(True, ice.ONEYEAR)
        logger.debug("Raw chip bin response len " + str(len(chip_bin)))
        chip_bin = chip_bin.encode('hex')
        logger.debug("Chip bin len %d val: %s" % (len(chip_bin), chip_bin))

        #1,2-addr ...
        chip_bin = chip_bin[2:]

        # Consistent capitalization
        chip_bin = chip_bin.upper()
        hexencoded = hexencoded.upper()

        for b in range(len(hexencoded)):
            try:
                if hexencoded[b] != chip_bin[b]:
                    logger.warn("ERR: Mismatch at half-byte" + str(b))
                    logger.warn("Expected:" + hexencoded[b])
                    logger.warn("Got:" + chip_bin[b])
                    return False
            except IndexError:
                logger.warn("ERR: Length mismatch")
                logger.warn("Expected %d bytes" % (len(hexencoded)/2))
                logger.warn("Got %d bytes" % (len(chip_bin)/2))
                logger.warn("All prior bytes validated correctly")
                return False

        logger.info("Programming validated successfully")
        return True

    def hang_for_messages(self):
        logger.info("Script is waiting to print any MBus messages.")
        logger.info("To quit, press Ctrl-C")
        try:
            while not self.wait_event.wait(1000):
                pass
        except KeyboardInterrupt:
            logger.info("Received keyboard interrupt.")

    def exit(self):
        self.wait_event.set()
        sys.exit()


class goc_programmer(m3_common):
    TITLE = "GOC Programmer"
    #SLOW_FREQ_IN_HZ = 0.625
    SLOW_FREQ_IN_HZ = 70

    def set_slow_frequency(self):
        self.ice.goc_set_frequency(self.SLOW_FREQ_IN_HZ)

    def wake_chip(self):
        passcode_string = "7394"
#        passcode_string = "3935"   # Reset request
        logger.info("Sending passcode to GOC")
        logger.debug("Sending:" + passcode_string)
        self.ice.goc_send(passcode_string.decode('hex'))
        printing_sleep(4.0)

    def set_fast_frequency(self):
        self.ice.goc_set_frequency(8*self.SLOW_FREQ_IN_HZ)

    def send_goc_message(self, message):
        logger.info("Sending GOC message")
        logger.debug("Sending: " + message)
        self.ice.goc_send(message.decode('hex'))
        printing_sleep(1.0)

        logger.info("Sending extra blink to end transaction")
        extra = "80"
        logger.debug("Sending: " + extra)
        self.ice.goc_send(extra.decode('hex'))

    def validate_bin(self):
        raise NotImplementedError("If you need this, let me know")

    def DMA_start_interrupt(self):
        logger.info("Sending 0x88 0x00000000")
        self.send("88".decode('hex'), "00000000".decode('hex'))
