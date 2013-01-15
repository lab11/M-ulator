#!/usr/bin/env python

################################################################################

import sys
import errno
import socket
import struct

try:
    import threading
    import Queue
except ImportError:
    print "Your python installation does not support threads."
    print
    print "Please install a version of python that supports threading."
    raise

try:
    import serial
except ImportError:
    print "You do not have the pyserial library installed."
    print
    print "For installation instructions, see:"
    print "\thttp://pyserial.sourceforge.net/pyserial.html#installation"
    raise

################################################################################

class ICE(object):
    VERSIONS = ((0,1),)

    # XXX: Version-specific constants?
    POWER_0P6 = 0
    POWER_1P2 = 1
    POWER_VBATT = 2

    POWER_0P6_DEFAULT = 0.675
    POWER_1P2_DEFAULT = 1.2
    POWER_VBATT_DEFAULT = 3.8

    GPIO_INPUT = 0
    GPIO_OUTPUT = 1
    GPIO_TRISTATE = 2

    class ICE_Error(Exception):
        '''
        A base class for all exceptions raised by this module
        '''
        pass

    class FormatError(ICE_Error):
        '''
        Something in the ICE protocol communicating with the board went wrong.

        This error should never be encountered in normal use.
        '''
        pass

    class ParameterError(ICE_Error):
        '''
        An illegal parameter was passed.

        This may be raised by the ICE library if it can determine in advance
        that the reqeust is illegal (e.g. out of range), or by the ICE board if
        the board rejects the desired setting (e.g. not configurable)
        '''

    class NAK_Error(ICE_Error):
        '''
        Raised when an unexpected NAK is returned
        '''
        pass

    def __init__(self):
        '''
        An ICE object.

        Most methods are not usuable until connect() has been called.
        '''

        self.event_id = 0
        self.last_event_id = -1
        self.sync_queue = Queue.Queue(1)

        self.msg_handler = {}
        self.msg_handler['d'] = self.d_defragger
        self.d_frag = ''
        self.d_lock = threading.Lock()

    def connect(self, serial_device, baudrate=115200):
        '''
        Opens a connection to the ICE board.

        The ICE object configuration (e.g. message handlers) cannot be safely
        changed after this method is invoked.
        '''
        self.dev = serial.Serial(serial_device, baudrate)
        if self.dev.isOpen():
            print "Connected to serial device at", self.dev.portstr
        else:
            raise self.ICE_Error, "Failed to connect to serial device"

        self.comm_thread = threading.Thread(target=self.communicator)
        self.comm_thread.daemon = True
        self.comm_thread.start()

        self.negotiate_version()

    def spawn_handler(self, msg_type, event_id, length, msg):
        try:
            t = threading.Thread(target=self.msg_handler[msg_type],
                    args=(msg_type, event_id, length, msg))
            t.daemon = True
            t.start()
        except KeyError:
            print "WARNING: No handler registered for message type:", msg_type
            print "Known Types:"
            for t,f in self.msg_handler.iteritems():
                print "%s\t%s" % (t, f)
            print "         Dropping packet:"
            print
            print "    Type: %s" % (msg_type)
            print "Event ID: %d" % (event_id)
            print "  Length: %d" % (length)
            print " Message:", msg.encode('hex')

    def communicator(self):
        while True:
            msg_type, event_id, length = self.dev.read(3)
            msg_type = ord(msg_type)
            event_id = ord(event_id)
            length = ord(length)
            msg = self.dev.read(length)

            if event_id == self.last_event_id:
                print "WARNING: Duplicate event_id! THIS IS A BUG [somewhere]!!"
                print "         Dropping packet:"
                print
                print "    Type: %d" % (ord(msg_type))
                print "Event ID: %d" % (event_id)
                print "  Length: %d" % (length)
                print " Message:", msg.encode('hex')
            else:
                self.last_event_id = event_id

            if msg_type in (0,1):
                # Ack / Nack response from a synchronous message
                try:
                    if msg_type == 0:
                        print "Got an ACK packet. Event:", event_id
                    else:
                        print "Got a NAK packet. Event:", event_id
                    self.sync_queue.put((msg_type, msg))
                except Queue.Full:
                    print "WARNING: Synchronization lost. Unsolicited ACK/NAK."
                    print "         Dropping packet:"
                    print
                    print "    Type: %s" % (["ACK","NAK"][ord(msg_type)])
                    print "Event ID: %d" % (event_id)
                    print "  Length: %d" % (length)
                    print " Message:", msg.encode('hex')
            else:
                msg_type = chr(msg_type)
                print "Got an async message of type:", msg_type
                self.spawn_handler(msg_type, event_id, length, msg)

    def d_defragger(self, msg_type, event_id, length, msg):
        '''
        Helper function to defragment 'd' type I2C messages before forwarding.

        This helper is installed by default for 'd' messages. It will attempt to
        call a helper registered under the name 'd+' when a complete message has
        been received. The message will be assigned the event id of the last
        received fragment.

        It may be safely overridden.
        '''
        with self.d_lock:
            assert msg_type == 'd'
            self.d_frag += msg
            # XXX: Make version dependent
            if length != 255:
                print "Got a complete I2C transaction of length %d bytes. Forwarding..." % (len(self.d_frag))
                self.spawn_handler('d+', event_id, len(self.d_frag), self.d_frag)
                self.d_frag = ''
            else:
                print "Got a fragment message... thus far %d bytes received:" % (len(self.d_frag))

    def send_message(self, msg_type, msg='', length=None):
        if len(msg_type) != 1:
            raise self.FormatError, "msg_type must be exactly 1 character"

        if len(msg) > 255:
            raise self.FormatError, "msg too long. Maximum msg is 255 bytes"

        if length is None:
            length = len(msg)

        buf = struct.pack("BBB", ord(msg_type), self.event_id, length)
        self.event_id = (self.event_id + 1) % 256
        self.dev.write(buf + msg)

        return self.sync_queue.get()

    def send_message_until_acked(self, msg_type, msg='', length=None, tries=5):
        while tries:
            ack, msg = self.send_message(msg_type, msg, length)
            if ack == 0:
                return msg
            tries -= 1

        raise self.NAK_Error

    def negotiate_version(self):
        '''
        Establish communication with an ICE board.

        This function is called automatically by __init__ and should not be
        called directly.
        '''
        print "This library supports versions..."
        for major, minor in ICE.VERSIONS:
            print "%d.%d" % (major, minor)

        resp = self.send_message_until_acked('V')
        if (len(resp) is 0) or (len(resp) % 2):
            raise self.FormatError, "Version response: " + resp

        print "This ICE board supports versions..."
        self.major = None
        self.minor = None
        while len(resp) > 0:
            major, minor = struct.unpack("BB", resp[:2])
            resp = resp[2:]
            if self.major is None and (major, minor) in ICE.VERSIONS:
                self.major = major
                self.minor = minor
                print "%d.%d **Chosen version" % (major, minor)
            else:
                print "%d.%d" % (major, minor)

        if self.major is None:
            print "No versions in common. Version negotiation failed."
            raise self.ICE_Error

        self.send_message_until_acked('v', struct.pack("BB", self.major, self.minor))

    def _fragment_sender(self, msg_type, msg):
        '''
        Internal. (helper for {i2c,goc}_send
        '''
        # XXX: Make version dependent?
        FRAG_SIZE = 255

        sent = 0
        print "Sending %d byte message (in %d byte fragments)" % (len(msg), FRAG_SIZE)
        while len(msg) >= FRAG_SIZE:
            ack,resp = self.send_message(msg_type, msg[0:FRAG_SIZE])
            if ack == 1: # (NAK)
                return sent + ord(resp)
            msg = msg[FRAG_SIZE:]
            sent += FRAG_SIZE
            print "\tSent %d bytes, %d remaining" % (sent, len(msg))
        print "Sending last message, %d bytes long" % (len(msg))
        ack,resp = self.send_message(msg_type, msg)
        if ack == 1:
            return sent + ord(resp)
        sent += len(resp)
        return sent

    def goc_send(self, msg):
        '''
        Blinks a message via GOC.

        Takes a raw byte stream (e.g. "0xaa".decode('hex')).
        Returns the number of bytes actually sent.

        Long messages may be fragmented between the ICE library and the ICE
        FPGA. These fragments will be combined on the ICE board, and given the
        significantly lower bandwidth of the GOC interface, there should be no
        interruption in message transmission.
        '''
        return self._fragment_sender('f', msg)

    def goc_set_frequency(self, freq_in_hz):
        '''
        Sets the GOC frequency.
        '''
        # Send a 3-byte value N, where 2 MHz / N == clock speed
        NOMINAL = int(2e6)
        setting = NOMINAL / freq_in_hz;
        packed = struct.pack("!I", setting)
        if packed[0] != '\x00':
            raise self.ParameterError, "Out of range."
        msg = struct.pack("B", ord('c')) + packed[1:]
        self.send_message_until_acked('o', msg)

    def i2c_send(self, addr, data):
        '''
        Sends an I2C message.

        Addr should be a single byte address.
        Data should be packed binary data, as returned by struct.pack

        The return value is the number of bytes actually sent *including the
        address byte*.

        Long messages may be fragmented between the ICE library and the ICE
        FPGA. On the I2C wire, this will appear as windows of time where the I2C
        clock is stretched for a period of time.  A faster baud rate between the
        PC host and the ICE FPGA will help mitigate this.
        '''

        msg = struct.pack("B", addr) + data
        return self._fragment_sender('d', msg)

    def i2c_get_speed(self):
        '''
        Get the clock speed of the ICE I2C driver in kHz.
        '''
        ack,msg = self.send_message('I', struct.pack("B", ord('c')))
        if ack == 0:
            if len(msg) != 1:
                raise self.FormatError
            return struct.unpack("B", msg) * 2

        ret = ord(msg[0])
        msg = msg[1:]
        if ret == errno.ENODEV:
            # XXX Generalize me w.r.t. version?
            return 100
        else:
            raise self.ICE_Error, "Unknown Error"

    def i2c_set_speed(self, speed):
        '''
        Set the clock speed of the ICE I2C driver in kHz.

        The accepted range of speeds is [2,400] kHz with steps of undefined
        increments. The actual set speed is returned.

        Raises an ICE_Error if the speed was not set.

        Note: This does *NOT* affect the clock speed of any M3 I2C drivers.
              That requires sending DMA messages to each of the M3 I2C
              controllers that you would like to change the speed of.
        '''
        if speed < 2:
            speed = 2
        elif speed > 400:
            speed = 400

        speed /= 2
        ack,msg = self.send_message('i', struct.pack("BB", ord('c'), speed))

        if ack == 0:
            return speed

        ret = ord(msg[0])
        msg = msg[1:]
        if ret == errno.EINVAL:
            raise self.ICE_Error, "ICE reports: Invalid argument."
        elif ret == errno.ENODEV:
            raise self.ICE_Error, "Changing I2C speed not supported."

    def _i2c_set_address(self, ones, zeros):
        self.send_message_until_acked('i', struct.pack("BBB", ord('a'), ones, zeros))

    def i2c_set_address(self, address=None):
        '''
        Set the I2C address(es) of the ICE peripheral.

        The ICE board will ACK messages sent to any address that matches the
        mask set by this function. The special character 'x' is used to signify
        don't-care bits. As example, to pretend to be the DSP layer:

           address = "1001 100x"

        Spaces are permitted and ignored. To disable this feature, set the
        address to None.
        '''
        if address is None:
            return self._i2c_set_address(0xff, 0xff)

        ones = 0
        zeros = 0
        idx = 8
        for c in address:
            idx -= 1
            if c == '1':
                ones |= (1 << idx)
            elif c == '0':
                zeros |= (1 << idx)
            elif c in ('x', 'X', ' '):
                continue
            else:
                raise self.FormatError, "Illegal character: >>>" + c + "<<<"

        if idx != 0:
            raise self.FormatError, "Address must be exactly 8 bits"

        return self._i2c_set_address(ones, zeros)

    def gpio_get_level(self, gpio_idx):
        '''
        Query whether a gpio is high or low. (high=True)
        '''
        resp = self.send_message_until_acked('G',
                struct.pack('BB', ord('l'), gpio_idx))
        if len(resp) != 1:
            raise self.FormatError, "Too long of a response from `Gl#':" + str(resp)

        return bool(struct.unpack("B", resp))

    def gpio_get_direction(self, gpio_idx):
        '''
        Query gpio pin setup.

        Returns one of:
            ICE.GPIO_INPUT
            ICE.GPIO_OUTPUT
            ICE.GPIO_TRISTATE
        '''
        resp = self.send_message_until_acked('G',
                struct.pack('BB', ord('d'), gpio_idx))
        if len(resp) != 1:
            raise self.FormatError, "Too long of a response from `Gl#':" + str(resp)

        direction = struct.unpack("B", resp)
        if direction not in (ICE.GPIO_INPUT, ICE.GPIO_OUTPUT, ICE.GPIO_TRISTATE):
            raise self.FormatError, "Unknown direction: " + str(direction)

        return direction

    def gpio_set_level(self, gpio_idx, level):
        '''
        Set gpio level. (high=True)
        '''
        self.send_message_until_acked('g',
                struct.pack('BBB', ord('l'), gpio_idx, level))

    def gpio_set_direction(self, gpio_idx, direction):
        '''
        Setup a GPIO pin.
        '''
        if direction not in (ICE.GPIO_INPUT, ICE.GPIO_OUTPUT, ICE.GPIO_TRISTATE):
            raise self.ParameterError, "Unknown direction: " + str(direction)

        self.send_message_until_acked('g',
                struct.pack('BBB', ord('d'), gpio_idx, direction))

    def power_get_voltage(self, rail):
        '''
        Query the current voltage setting of a power rail.

        The `rail' argument must be one of:
            ICE.POWER_0P6
            ICE.POWER_1P2
            ICE.POWER_VBATT
        '''
        if rail not in (ICE.POWER_0P6, ICE.POWER_1P2, ICE.POWER_VBATT):
            raise self.ParameterError, "Invalid rail: " + str(rail)

        resp = self.send_message_until_acked('P', struct.pack("BB", ord('v'), rail))
        if len(resp) != 1:
            raise self.FormatError, "Too long of a response from `Pv#':" + str(resp)
        raw = struct.unpack("B", resp)

        # Vout = (0.537 + 0.0185 * v_set) * Vdefault
        default_voltage = (ICE.POWER_0P6_DEFAULT, ICE.POWER_1P2_DEFAULT,
                ICE.POWER_VBATT_DEFAULT)[rail]
        vout = (0.537 + 0.0185 * raw) * default_voltage
        return vout

    def power_get_onoff(self, rail):
        '''
        Query the current on/off setting of a power rail.

        Returns a boolean, on=True.
        '''
        if rail not in (ICE.POWER_0P6, ICE.POWER_1P2, ICE.POWER_VBATT):
            raise self.ParameterError, "Invalid rail: " + str(rail)

        resp = self.send_message_until_acked('P', struct.pack("BB", ord('o'), rail))
        if len(resp) != 1:
            raise self.FormatError, "Too long of a response from `Po#':" + str(resp)
        onoff = struct.unpack("B", resp)
        return bool(onoff)

    def power_set_voltage(self, rail, output_voltage):
        '''
        Set the voltage setting of a power rail. Units are V.
        '''
        if rail not in (ICE.POWER_0P6, ICE.POWER_1P2, ICE.POWER_VBATT):
            raise self.ParameterError, "Invalid rail: " + str(rail)

        # Vout = (0.537 + 0.0185 * v_set) * Vdefault
        output_voltage = float(output_voltage)
        default_voltage = (ICE.POWER_0P6_DEFAULT, ICE.POWER_1P2_DEFAULT,
                ICE.POWER_VBATT_DEFAULT)[rail]
        vset = ((output_voltage / default_voltage) - 0.537) / 0.0185
        vset = int(vset)
        if (vset < 0) or (vset > 255):
            raise self.ParameterError, "Voltage exceeds range. vset: " + str(vset)

        self.send_message_until_acked('p', struct.pack("BBB", ord('v'), rail, vset))

    def power_set_onoff(self, rail, onoff):
        '''
        Turn a power rail on or off (on=True).
        '''
        if rail not in (ICE.POWER_0P6, ICE.POWER_1P2, ICE.POWER_VBATT):
            raise self.ParameterError, "Invalid rail: " + str(rail)

        self.send_message_until_acked('p', struct.pack("BBB", ord('v'), rail, onoff))
