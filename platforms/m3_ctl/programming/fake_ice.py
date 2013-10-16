#!/usr/bin/env python

MAX_GPIO = 24
DEFAULT_SERIAL = '/tmp/com2'
DEFAULT_I2C_MASK = '1001100x'
DEFAULT_I2C_SPEED_IN_KHZ = 100
DEFAULT_FLOW_CLOCK_IN_HZ = .625
DEFAULT_POWER_0P6 = 0.675
DEFAULT_POWER_1P2 = 1.2
DEFAULT_POWER_VBATT = 3.8
DEFAULT_VSET_0P6 = 19
DEFAULT_VSET_1P2 = 25
DEFAULT_VSET_VBATT = 25

import sys
import serial
import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('fake_ice')

logger.info("-" * 80)
logger.info("-- M3 ICE Interface Board Simulator")
logger.info("")

if len(sys.argv) > 1:
    serial_port = sys.argv[1]
else:
    serial_port = DEFAULT_SERIAL

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

vset_0p6 = DEFAULT_VSET_0P6
vset_1p2 = DEFAULT_VSET_1P2
vset_vbatt = DEFAULT_VSET_VBATT
power_0p6_on = False
power_1p2_on = False
power_vbatt_on = False

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
    s.write(chr(len(msg)))
    if len(msg):
        s.write(msg)
    logger.debug("Sent a response of length: " + str(len(msg)))

def ack():
    respond('')

def nak():
    respond('', ack=False)

i2c_msg = ''
i2c_match = False
flow_msg = ''
while True:
    msg_type, event_id, length = s.read(3)
    logger.debug("Got a message of type: " + msg_type)
    event_id = ord(event_id)
    length = ord(length)
    msg = s.read(length)

    if msg_type == 'V':
        respond('0001'.decode('hex'))
    elif msg_type == 'v':
        if msg == '0001'.decode('hex'):
            ack()
            logger.info("Negotiated to protocol version 0.1")
        else:
            logger.error("Request for unknown version: " + msg)
            raise Exception
    elif msg_type == 'd':
        i2c_msg += msg
        if not i2c_match:
            if not match_mask(msg[0], i2c_mask_ones, i2c_mask_zeros):
                nak()
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
    elif msg_type == 'I':
        if len(msg) < 3:
            logger.error("bad 'I' message length: " + str(len(msg)))
            raise Exception
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
        if len(msg) < 3:
            logger.error("bad 'i' message length: " + str(len(msg)))
            raise Exception
        if msg[0] == 'c':
            i2c_speed_in_khz = ord(msg[1]) * 2
            logger.info("I2C Bus Speed set to %d kHz", i2c_speed_in_khz)
            ack()
        elif msg[0] == 'a':
            i2c_mask_ones = ord(msg[1])
            i2c_mask_zeros = ord(msg[2])
            logger.info("ICE I2C Mask set to %02x ones, %02x zeros",
                    i2c_mask_ones, i2c_mask_zeros)
            ack()
        else:
            logger.error("bad 'i' subtype: " + msg[0])
            raise Exception
    elif msg_type == 'f':
        flow_msg += msg
        if len(msg) != 255:
            logger.info("Got flow message:")
            logger.info("  addr: " + flow_msg[0].encode('hex'))
            logger.info("  data: " + flow_msg[1:].encode('hex'))
            flow_msg = ''
        else:
            logger.debug("Got flow fragment")
        ack()
    elif msg_type == 'O':
        if len(msg) != 5:
            logger.error("bad 'O' message length: " + str(len(msg)))
            raise Exception
        if msg[0] == 'c':
            logger.info("Responded to query for FLOW clock (%.2f Hz)", flow_clock_in_hz)
            div = 2e6 / flow_clock_in_hz
            resp = chr((div >> 16) & 0xff)
            resp += chr((div >> 8) & 0xff)
            resp += chr(div & 0xff)
            respond(resp)
        else:
            logger.error("bad 'O' subtype: " + msg[0])
    elif msg_type == 'o':
        if len(msg) != 5:
            logger.error("bad 'o' message length: " + str(len(msg)))
            raise Exception
        if msg[0] == 'c':
            div = (ord(msg[0]) << 16) | (ord(msg[1]) << 8) | ord(msg[2])
            flow_clock_in_hz = 2e6 / div
            logger.info("Set FLOW clock to %.2f Hz", flow_clock_in_hz)
            ack()
        else:
            logger.error("bad 'o' subtype: " + msg[0])
    elif msg_type == 'G':
        if len(msg) != 3:
            logger.error("bad 'G' message length: " + str(len(msg)))
            raise Exception
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
        if len(msg) != 3:
            logger.error("bad 'g' message length: " + str(len(msg)))
            raise Exception
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
    elif msg_type == 'P':
        if len(msg) < 3:
            logger.error("Bad 'P' message length: " + str(len(msg)))
            raise Exception
        pwr_idx = ord(msg[0])
        if pwr_idx not in (0,1,2):
            logger.error("Illegal power index: %d", pwr_idx)
            raise Exception
        if msg[0] == 'v':
            if pwr_idx is 0:
                logger.info("Query 0.6V rail (vset=%d, vout=%.2f)", vset_0p6,
                        (0.537 + 0.0185 * vset_0p6) * DEFAULT_POWER_0P6)
                respond(chr(vset_0p6))
            elif pwr_idx is 1:
                logger.info("Query 1.2V rail (vset=%d, vout=%.2f)", vset_1p2,
                        (0.537 + 0.0185 * vset_1p2) * DEFAULT_POWER_1P2)
                respond(chr(vset_1p2))
            elif pwr_idx is 2:
                logger.info("Query VBatt rail (vset=%d, vout=%.2f)", vset_vbatt,
                        (0.537 + 0.0185 * vset_vbatt) * DEFAULT_POWER_VBATT)
                respond(chr(vset_vbatt))
            ack()
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
        if len(msg) < 3:
            logger.error("Bad 'p' message length: " + str(len(msg)))
            raise Exception
        pwr_idx = ord(msg[0])
        if pwr_idx not in (0,1,2):
            logger.error("Illegal power index: %d", pwr_idx)
            raise Exception
        if msg[0] == 'v':
            if pwr_idx is 0:
                vset_0p6 = ord(msg[1])
                logger.info("Set 0.6V rail to vset=%d, vout=%.2f", vset_0p6,
                        (0.537 + 0.0185 * vset_0p6) * DEFAULT_POWER_0P6)
            elif pwr_idx is 1:
                vset_1p2 = ord(msg[1])
                logger.info("Set 1.2V rail to vset=%d, vout=%.2f", vset_1p2,
                        (0.537 + 0.0185 * vset_1p2) * DEFAULT_POWER_1P2)
            elif pwr_idx is 2:
                vset_vbatt = ord(msg[1])
                logger.info("Set VBatt rail to vset=%d, vout=%.2f", vset_vbatt,
                        (0.537 + 0.0185 * vset_vbatt) * DEFAULT_POWER_VBATT)
            ack()
        elif msg[0] == 'o':
            if pwr_idx is 0:
                power_0p6_on = bool(msg[1])
                logger.info("Set 0.6V rail %s", ('off','on')[power_0p6_on])
            elif pwr_idx is 1:
                power_1p2_on = bool(msg[1])
                logger.info("Set 1.2V rail %s", ('off','on')[power_1p2_on])
            elif pwr_idx is 2:
                power_vbatt_on = bool(msg[1])
                logger.info("Set vbatt rail %s", ('off','on')[power_vbatt_on])
        else:
            logger.error("bad 'p' subtype: " + msg[0])
            raise Exception
    else:
        logger.error("Unknown msg type: " + msg_type)
        raise Exception
