#!/usr/bin/env python

CAPABILITES = "?_dIifOoBbMmeGgPp"
MAX_GPIO = 24
DEFAULT_SERIAL = '/tmp/com2'
DEFAULT_BAUD_DIVIDER = 0x00AE
DEFAULT_I2C_MASK = '1001100x'
DEFAULT_I2C_SPEED_IN_KHZ = 100
DEFAULT_FLOW_CLOCK_IN_HZ = .625
DEFAULT_POWER_0P6 = 0.675
DEFAULT_POWER_1P2 = 1.2
DEFAULT_POWER_VBATT = 3.8
DEFAULT_VSET_0P6 = 19
DEFAULT_VSET_1P2 = 25
DEFAULT_VSET_VBATT = 25
DEFAULT_MBUS_FULL_PREFIX_ONES = 0xfffff0
DEFAULT_MBUS_FULL_PREFIX_ZEROS = 0xfffff0
DEFAULT_MBUS_SHORT_PREFIX_ONES = 0xf0
DEFAULT_MBUS_SHORT_PREFIX_ZEROS = 0xf0
DEFAULT_MBUS_SNOOP_FULL_PREFIX_ONES = 0xfffff0
DEFAULT_MBUS_SNOOP_FULL_PREFIX_ZEROS = 0xfffff0
DEFAULT_MBUS_SNOOP_SHORT_PREFIX_ONES = 0xf0
DEFAULT_MBUS_SNOOP_SHORT_PREFIX_ZEROS = 0xf0
DEFAULT_MBUS_BROADCAST_MASK_ONES = 0x0f
DEFAULT_MBUS_BROADCAST_MASK_ZEROS = 0x0f
DEFAULT_MBUS_SNOOP_BROADCAST_MASK_ONES = 0x0f
DEFAULT_MBUS_SNOOP_BROADCAST_MASK_ZEROS = 0x0f

import sys, serial
from time import sleep

import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('fake_ice')

logger.info("-" * 80)
logger.info("-- M3 ICE Interface Board Simulator")
logger.info("")

from ice import ICE

if len(sys.argv) > 1:
    serial_port = sys.argv[1]
else:
    serial_port = DEFAULT_SERIAL

baud_divider = DEFAULT_BAUD_DIVIDER

if len(sys.argv) > 2:
    i2c_mask = sys.argv[2]
else:
    i2c_mask = DEFAULT_I2C_MASK
i2c_mask_ones = 0
i2c_mask_zeros = 0
for bit in i2c_mask:
    i2c_mask_ones <<= 1
    i2c_mask_zeros <<= 1
    i2c_mask_ones |= (bit == '1')
    i2c_mask_zeros |= (bit == '0')
logger.debug("mask %s ones %02x zeros %02x", i2c_mask, i2c_mask_ones, i2c_mask_zeros)
i2c_speed_in_khz = DEFAULT_I2C_SPEED_IN_KHZ

flow_clock_in_hz = DEFAULT_FLOW_CLOCK_IN_HZ
flow_onoff = False

vset_0p6 = DEFAULT_VSET_0P6
vset_1p2 = DEFAULT_VSET_1P2
vset_vbatt = DEFAULT_VSET_VBATT
power_0p6_on = False
power_1p2_on = False
power_vbatt_on = False

mbus_full_prefix_ones = DEFAULT_MBUS_FULL_PREFIX_ONES
mbus_full_prefix_zeros = DEFAULT_MBUS_FULL_PREFIX_ZEROS
mbus_short_prefix_ones = DEFAULT_MBUS_SHORT_PREFIX_ONES
mbus_short_prefix_zeros = DEFAULT_MBUS_SHORT_PREFIX_ZEROS
mbus_snoop_full_prefix_ones = DEFAULT_MBUS_SNOOP_FULL_PREFIX_ONES
mbus_snoop_full_prefix_zeros = DEFAULT_MBUS_SNOOP_FULL_PREFIX_ZEROS
mbus_snoop_short_prefix_ones = DEFAULT_MBUS_SNOOP_SHORT_PREFIX_ONES
mbus_snoop_short_prefix_zeros = DEFAULT_MBUS_SNOOP_SHORT_PREFIX_ZEROS
mbus_broadcast_mask_ones = DEFAULT_MBUS_BROADCAST_MASK_ONES
mbus_broadcast_mask_zeros = DEFAULT_MBUS_BROADCAST_MASK_ZEROS
mbus_snoop_broadcast_mask_ones = DEFAULT_MBUS_SNOOP_BROADCAST_MASK_ONES
mbus_snoop_broadcast_mask_zeros = DEFAULT_MBUS_SNOOP_BROADCAST_MASK_ZEROS
mbus_ismaster = False
mbus_should_interrupt = 0
mbus_should_prio = 0

def match_mask(val, ones, zeros):
    return ((val & ones) == ones) and ((~val & zeros) == zeros)

try:
    s = serial.Serial(serial_port, 115200)
except:
    logger.error("Opening serial failed")
    logger.error("")
    logger.error("If you need to create a software serial tunnel, use socat:")
    logger.error("  socat -x pty,link=/tmp/com1,raw,echo=0 pty,link=/tmp/com2,raw,echo=0")
    logger.error("")
    raise
if not s.isOpen():
    logger.error('Could not open serial port at: ' + serial_port)
    raise IOError, "Failed to open serial port"

class Gpio(object):
    GPIO_INPUT    = 0
    GPIO_OUTPUT   = 1
    GPIO_TRISTATE = 2

    def __init__(self, direction=GPIO_TRISTATE, level=False):
        self.direction = direction
        self.level = level

    def __str__(self):
        s = ''
        if self.direction == Gpio.GPIO_INPUT:
            s += ' IN'
        elif self.direction == Gpio.GPIO_OUTPUT:
            s += 'OUT'
        elif self.direction == Gpio.GPIO_TRISTATE:
            s += 'TRI'
        else:
            raise RuntimeError, "wtf"

        s += ' - '

        if self.level:
            s += '1'
        else:
            s += '0'

        return s

    def __repr__(self):
        return self.__str__()

    def __setattr__(self, name, value):
        if name is 'direction':
            if value not in (Gpio.GPIO_INPUT, Gpio.GPIO_OUTPUT, Gpio.GPIO_TRISTATE):
                raise ValueError, "Attempt to set illegal direction", value
        if name is 'level':
            if value not in (True, False):
                raise ValueError, "GPIO level must be true or false. Got", value
        object.__setattr__(self, name, value)

event = 0
gpios = [Gpio() for x in xrange(MAX_GPIO)]

def respond(msg, ack=True):
    global s
    global event

    if (ack):
        s.write(chr(0))
    else:
        s.write(chr(1))
    s.write(chr(event))
    event += 1
    event %= 256
    s.write(chr(len(msg)))
    if len(msg):
        s.write(msg)
    logger.debug("Sent a response of length: " + str(len(msg)))

def ack():
    respond('')

def nak():
    respond('', ack=False)

class UnknownCommandException(Exception):
    pass


i2c_msg = ''
i2c_match = False
flow_msg = ''
ein_msg = ''
mbus_msg = ''
while True:
    def min_proto(proto):
        if minor < proto:
            logger.error("Request for protocol 0.2 command, but the")
            logger.error("negotiated protocol was 0.1")
            raise UnknownCommandException

    try:
        msg_type, event_id, length = s.read(3)
        logger.debug("Got a message of type: " + msg_type)
        event_id = ord(event_id)
        length = ord(length)
        msg = s.read(length)

        if msg_type == 'V':
            respond('00020001'.decode('hex'))
        elif msg_type == 'v':
            if msg == '0002'.decode('hex'):
                minor = 2
                ack()
                logger.info("Negotiated to protocol version 0.2")
            elif msg == '0001'.decode('hex'):
                minor = 1
                ack()
                logger.info("Negotiated to protocol version 0.1")
            else:
                logger.error("Request for unknown version: " + msg)
                raise Exception

        elif msg_type == '?':
            min_proto(2)
            if msg[0] == '?':
                logger.info("Responded to query capabilites with " + CAPABILITES)
                respond(CAPABILITES)
            elif msg[0] == 'b':
                logger.info("Responded to query for ICE baudrate (divider: 0x%04X)" % (baud_divider))
                respond(chr((baud_divider >> 8) & 0xff) + chr(baud_divider & 0xff))
            else:
                logger.error("Bad '?' subtype: " + msg[0])
                raise UnknownCommandException
        elif msg_type == '_':
            min_proto(2)
            if msg[0] == 'b':
                high = ord(msg[1])
                low = ord(msg[2])
                new_div = low | (high << 8)
                if new_div not in (0x00AE, 0x0007):
                    logger.error("Bad baudrate divider: 0x%04X" % (new_div))
                    raise Exception
                ack()
                if new_div == 0x00AE:
                    s.baudrate = 115200
                elif new_div == 0x0007:
                    s.baudrate = 3000000
                else:
                    logger.error("Unknown baudrate divider")
                    raise Exception
                baud_divider = new_div
                logger.info("New baud divider set: " + str(baud_divider))
            else:
                logger.error("bad '_' subtype: " + msg[0])
                raise UnknownCommandException
        elif msg_type == 'b':
            min_proto(2)
            mbus_msg += msg
            if len(msg) != 255:
                logger.info("Got a MBus message:")
                logger.info("   message: " + mbus_msg.encode('hex'))
                mbus_msg = ''
                if mbus_should_interrupt:
                    logger.info("Message would have interrupted")
                    if mbus_should_interrupt == 1:
                        mbus_should_interrupt = 0
                if mbus_should_prio:
                    logger.info("Message would have been sent high priority")
                    if mbus_should_prio == 1:
                        mbus_should_prio = 0
            else:
                logger.debug("Got MBus fragment")
            ack()
        elif msg_type == 'd':
            i2c_msg += msg
            if not i2c_match:
                if not match_mask(ord(msg[0]), i2c_mask_ones, i2c_mask_zeros):
                    logger.info("I2C address %02x did not match mask %02x %02x",
                            ord(msg[0]), i2c_mask_ones, i2c_mask_zeros)
                    respond(chr(0), ack=False)
                    continue
                i2c_match = True
            if len(msg) != 255:
                logger.info("Got i2c message:")
                logger.info("  addr: " + i2c_msg[0].encode('hex'))
                logger.info("  data: " + i2c_msg[1:].encode('hex'))
                i2c_msg = ''
                i2c_match = False
            else:
                logger.debug("Got i2c fragment")
            ack()
        elif msg_type == 'e':
            min_proto(2)
            ein_msg += msg
            if len(msg) != 255:
                logger.info("Got a EIN message:")
                logger.info("  message: " + ein_msg.encode('hex'))
                ein_msg = ''
            else:
                logger.debug("Got EIN fragment")
            ack()
        elif msg_type == 'f':
            flow_msg += msg
            if len(msg) != 255:
                logger.info("Got f-type message in %s mode:", ('EIN','GOC')[ein_goc_toggle])
                logger.info("  addr: " + flow_msg[0].encode('hex'))
                logger.info("  data: " + flow_msg[1:].encode('hex'))
                flow_msg = ''
            else:
                logger.debug("Got f-type fragment in %s mode", ('EIN','GOC')[ein_goc_toggle])
            ack()
        elif msg_type == 'G':
            if msg[0] == 'l':
                logger.info("Responded to request for GPIO %d Dir (%s)", ord(msg[1]), gpios[ord(msg[1])])
                respond(chr(gpios[ord(msg[1])].level))
            elif msg[0] == 'd':
                logger.info("Responded to request for GPIO %d Level (%s)", ord(msg[1]), gpios[ord(msg[1])])
                respond(chr(gpios[ord(msg[1])].direction))
            else:
                logger.error("bad 'G' subtype: " + msg[0])
                raise Exception
        elif msg_type == 'g':
            if msg[0] == 'l':
                gpios[ord(msg[1])].level = (ord(msg[2]) == True)
                logger.info("Set GPIO %d Level: %s", ord(msg[1]), gpios[ord(msg[1])])
                ack()
            elif msg[0] == 'd':
                gpios[ord(msg[1])].direction = ord(msg[2])
                logger.info("Set GPIO %d Dir: %s", ord(msg[1]), gpios[ord(msg[1])])
                ack()
            else:
                logger.error("bad 'g' subtype: " + msg[0])
                raise Exception
        elif msg_type == 'I':
            if msg[0] == 'c':
                logger.info("Responded to query for I2C bus speed (%d kHz)", i2c_speed_in_khz)
                respond(chr(i2c_speed_in_khz / 2))
            elif msg[0] == 'a':
                logger.info("Responded to query for ICE I2C mask (%02x ones %02x zeros)",
                        i2c_mask_ones, i2c_mask_zeros)
                respond(chr(i2c_mask_ones) + chr(i2c_mask_zeros))
            else:
                logger.error("bad 'I' subtype: " + msg[0])
                raise Exception
        elif msg_type == 'i':
            if msg[0] == 'c':
                i2c_speed_in_khz = ord(msg[1]) * 2
                logger.info("I2C Bus Speed set to %d kHz", i2c_speed_in_khz)
                ack()
            elif msg[0] == 'a':
                i2c_mask_ones = ord(msg[1])
                i2c_mask_zeros = ord(msg[2])
                logger.info("ICE I2C mask set to 0x%02x ones, 0x%02x zeros",
                        i2c_mask_ones, i2c_mask_zeros)
                ack()
            else:
                logger.error("bad 'i' subtype: " + msg[0])
                raise Exception
        elif msg_type == 'M':
            min_proto(2)
            if msg[0] == 'l':
                logger.info("Responded to query for MBus full prefix mask (%06x ones %06x zeros)",
                        mbus_full_prefix_ones, mbus_full_prefix_zeros)
                r = chr((mbus_full_prefix_ones >> 16) & 0xff)
                r += chr((mbus_full_prefix_ones >> 8) & 0xff)
                r += chr((mbus_full_prefix_ones >> 0) & 0xff)
                r += chr((mbus_full_prefix_zeros >> 16) & 0xff)
                r += chr((mbus_full_prefix_zeros >>  8) & 0xff)
                r += chr((mbus_full_prefix_zeros >>  0) & 0xff)
                respond(r)
            elif msg[0] == 's':
                logger.info("Responded to query for MBus short prefix mask (%02x ones %06x zeros)",
                        mbus_short_prefix_ones, mbus_short_prefix_zeros)
                respond(chr(mbus_short_prefix_ones) + chr(mbus_short_prefix_zeros))
            elif msg[0] == 'L':
                logger.info("Responded to query for MBus snoop full prefix mask (%06x ones %06x zeros)",
                        mbus_snoop_full_prefix_ones, mbus_snoop_full_prefix_zeros)
                r = chr((mbus_snoop_full_prefix_ones >> 16) & 0xff)
                r += chr((mbus_snoop_full_prefix_ones >> 8) & 0xff)
                r += chr((mbus_snoop_full_prefix_ones >> 0) & 0xff)
                r += chr((mbus_snoop_full_prefix_zeros >> 16) & 0xff)
                r += chr((mbus_snoop_full_prefix_zeros >>  8) & 0xff)
                r += chr((mbus_snoop_full_prefix_zeros >>  0) & 0xff)
                respond(r)
            elif msg[0] == 'S':
                logger.info("Responded to query for MBus snoop short prefix mask (%02x ones %06x zeros)",
                        mbus_snoop_short_prefix_ones, mbus_snoop_short_prefix_zeros)
                respond(chr(mbus_snoop_short_prefix_ones) + chr(mbus_snoop_short_prefix_zeros))
            elif msg[0] == 'b':
                logger.info("Responded to query for MBus broadcast mask (%02x ones %02x zeros)",
                        mbus_broadcast_mask_ones, mbus_broadcast_mask_zeros)
                respond(chr(mbus_broadcast_mask_ones) + chr(mbus_broadcast_mask_zeros))
            elif msg[0] == 'B':
                logger.info("Responded to query for MBus snoop broadcast mask (%02x ones %02x zeros)",
                        mbus_snoop_broadcast_mask_ones, mbus_snoop_broadcast_mask_zeros)
                respond(chr(mbus_snoop_broadcast_mask_ones) + chr(mbus_snoop_broadcast_mask_zeros))
            elif msg[0] == 'm':
                logger.info("Responded to query for MBus master state (%s)",
                        ("off", "on")[mbus_ismaster])
                respond(chr(mbus_ismaster))
            elif msg[0] == 'c':
                raise NotImplementedError, "MBus clock not defined"
            elif msg[0] == 'i':
                logger.info("Responded to query for MBus should interrupt (%d)",
                        mbus_should_interrupt)
                respond(chr(mbus_should_interrupt))
            elif msg[0] == 'p':
                logger.info("Responded to query for MBus should use priority arb (%d)",
                        mbus_should_prio)
                respond(chr(mbus_should_prio))
            else:
                logger.error("bad 'M' subtype: " + msg[0])
        elif msg_type == 'm':
            min_proto(2)
            if msg[0] == 'l':
                mbus_full_prefix_ones = ord(msg[3])
                mbus_full_prefix_ones |= ord(msg[2]) << 8
                mbus_full_prefix_ones |= ord(msg[1]) << 16
                mbus_full_prefix_zeros = ord(msg[6])
                mbus_full_prefix_zeros |= ord(msg[5]) << 8
                mbus_full_prefix_zeros |= ord(msg[4]) << 16
                logger.info("MBus full prefix mask set to ones %06x zeros %06x",
                        mbus_full_prefix_ones, mbus_full_prefix_zeros)
                ack()
            elif msg[0] == 's':
                mbus_short_prefix_ones = ord(msg[1])
                mbus_short_prefix_zeros = ord(msg[2])
                logger.info("MBus short prefix mask set to ones %02x zeros %02x",
                        mbus_short_prefix_ones, mbus_short_prefix_zeros)
                ack()
            elif msg[0] == 'L':
                mbus_snoop_full_prefix_ones = ord(msg[3])
                mbus_snoop_full_prefix_ones |= ord(msg[2]) << 8
                mbus_snoop_full_prefix_ones |= ord(msg[1]) << 16
                mbus_snoop_full_prefix_zeros = ord(msg[6])
                mbus_snoop_full_prefix_zeros |= ord(msg[5]) << 8
                mbus_snoop_full_prefix_zeros |= ord(msg[4]) << 16
                logger.info("MBus snoop full prefix mask set to ones %06x zeros %06x",
                        mbus_snoop_full_prefix_ones, mbus_snoop_full_prefix_zeros)
                ack()
            elif msg[0] == 'S':
                mbus_snoop_short_prefix_ones = ord(msg[1])
                mbus_snoop_short_prefix_zeros = ord(msg[2])
                logger.info("MBus snoop short prefix mask set to ones %02x zeros %02x",
                        mbus_snoop_short_prefix_ones, mbus_snoop_short_prefix_zeros)
                ack()
            elif msg[0] == 'b':
                mbus_broadcast_mask_ones = ord(msg[1])
                mbus_broadcast_mask_zeros = ord(msg[2])
                logger.info("MBus broadcast mask set to ones %02x zeros %02x",
                        mbus_broadcast_mask_ones, mbus_broadcast_mask_zeros)
                ack()
            elif msg[0] == 'B':
                mbus_snoop_broadcast_mask_ones = ord(msg[1])
                mbus_snoop_broadcast_mask_zeros = ord(msg[2])
                logger.info("MBus snoop broadcast mask set to ones %02x zeros %02x",
                        mbus_snoop_broadcast_mask_ones, mbus_snoop_broadcast_mask_zeros)
                ack()
            elif msg[0] == 'm':
                mbus_ismaster = bool(ord(msg[1]))
                logger.info("MBus master mode set " + ("off", "on")[mbus_ismaster])
                ack()
            elif msg[0] == 'c':
                raise NotImplementedError, "MBus clock not defined"
            elif msg[0] == 'i':
                mbus_should_interrupt = ord(msg[1])
                logger.info("MBus should interrupt set to %d", mbus_should_interrupt)
                ack()
            elif msg[0] == 'p':
                mbus_should_prio = ord(msg[1])
                logger.info("MBus should use priority arbitration set to %d",
                        mbus_should_prio)
                ack()
            else:
                logger.error("bad 'm' subtype: " + msg[0])
        elif msg_type == 'O':
            if msg[0] == 'c':
                logger.info("Responded to query for FLOW clock (%.2f Hz)", flow_clock_in_hz)
                div = int(2e6 / flow_clock_in_hz)
                resp = chr((div >> 16) & 0xff)
                resp += chr((div >> 8) & 0xff)
                resp += chr(div & 0xff)
                respond(resp)
            elif msg[0] == 'o':
                if minor > 1:
                    logger.info("Responded to query for FLOW power (%s)", ('off','on')[flow_onoff])
                    respond(chr(flow_onoff))
                else:
                    logger.error("Request for protocol 0.2 command (Oo), but the")
                    logger.error("negotiated protocol was 0.1")
            else:
                logger.error("bad 'O' subtype: " + msg[0])
        elif msg_type == 'o':
            if msg[0] == 'c':
                div = (ord(msg[1]) << 16) | (ord(msg[2]) << 8) | ord(msg[3])
                flow_clock_in_hz = 2e6 / div
                logger.info("Set FLOW clock to %.2f Hz", flow_clock_in_hz)
                ack()
            elif msg[0] == 'o':
                min_proto(2)
                if minor > 1:
                    flow_onoff = bool(ord(msg[1]))
                    logger.info("Set FLOW power to %s", ('off','on')[flow_onoff])
                    ack()
            elif msg[0] == 'p':
                min_proto(2)
                ein_goc_toggle = bool(ord(msg[1]))
                logger.info("Set GOC/EIN toggle to %s mode", ('EIN','GOC')[ein_goc_toggle])
                ack()
            else:
                logger.error("bad 'o' subtype: " + msg[0])
        elif msg_type == 'P':
            pwr_idx = ord(msg[1])
            if pwr_idx not in (0,1,2):
                logger.error("Illegal power index: %d", pwr_idx)
                raise Exception
            if msg[0] == 'v':
                if pwr_idx is 0:
                    logger.info("Query 0.6V rail (vset=%d, vout=%.2f)", vset_0p6,
                            (0.537 + 0.0185 * vset_0p6) * DEFAULT_POWER_0P6)
                    respond(chr(pwr_idx) + chr(vset_0p6))
                elif pwr_idx is 1:
                    logger.info("Query 1.2V rail (vset=%d, vout=%.2f)", vset_1p2,
                            (0.537 + 0.0185 * vset_1p2) * DEFAULT_POWER_1P2)
                    respond(chr(pwr_idx) + chr(vset_1p2))
                elif pwr_idx is 2:
                    logger.info("Query VBatt rail (vset=%d, vout=%.2f)", vset_vbatt,
                            (0.537 + 0.0185 * vset_vbatt) * DEFAULT_POWER_VBATT)
                    respond(chr(pwr_idx) + chr(vset_vbatt))
            elif msg[0] == 'o':
                if pwr_idx is 0:
                    logger.info("Query 0.6V rail (%s)", ('off','on')[power_0p6_on])
                    respond(chr(power_0p6_on))
                elif pwr_idx is 1:
                    logger.info("Query 1.2V rail (%s)", ('off','on')[power_1p2_on])
                    respond(chr(power_1p2_on))
                elif pwr_idx is 2:
                    logger.info("Query vbatt rail (%s)", ('off','on')[power_vbatt_on])
                    respond(chr(power_vbatt_on))
            else:
                logger.error("bad 'p' subtype: " + msg[0])
                raise Exception
        elif msg_type == 'p':
            pwr_idx = ord(msg[1])
            if pwr_idx not in (0,1,2):
                logger.error("Illegal power index: %d", pwr_idx)
                raise Exception
            if msg[0] == 'v':
                if pwr_idx is ICE.POWER_0P6:
                    vset_0p6 = ord(msg[2])
                    logger.info("Set 0.6V rail to vset=%d, vout=%.2f", vset_0p6,
                            (0.537 + 0.0185 * vset_0p6) * DEFAULT_POWER_0P6)
                elif pwr_idx is ICE.POWER_1P2:
                    vset_1p2 = ord(msg[2])
                    logger.info("Set 1.2V rail to vset=%d, vout=%.2f", vset_1p2,
                            (0.537 + 0.0185 * vset_1p2) * DEFAULT_POWER_1P2)
                elif pwr_idx is ICE.POWER_VBATT:
                    vset_vbatt = ord(msg[2])
                    logger.info("Set VBatt rail to vset=%d, vout=%.2f", vset_vbatt,
                            (0.537 + 0.0185 * vset_vbatt) * DEFAULT_POWER_VBATT)
                ack()
            elif msg[0] == 'o':
                if pwr_idx is ICE.POWER_0P6:
                    power_0p6_on = bool(ord(msg[2]))
                    logger.info("Set 0.6V rail %s", ('off','on')[power_0p6_on])
                elif pwr_idx is ICE.POWER_1P2:
                    power_1p2_on = bool(ord(msg[2]))
                    logger.info("Set 1.2V rail %s", ('off','on')[power_1p2_on])
                elif pwr_idx is ICE.POWER_VBATT:
                    power_vbatt_on = bool(ord(msg[2]))
                    logger.info("Set VBatt rail %s", ('off','on')[power_vbatt_on])
                ack()
            else:
                logger.error("bad 'p' subtype: " + msg[0])
                raise UnknownCommandException
        else:
            logger.error("Unknown msg type: " + msg_type)
            raise UnknownCommandException
    except UnknownCommandException:
        nak()
    except NameError:
        logger.error("Commands issued before version negotiation?")
        raise
