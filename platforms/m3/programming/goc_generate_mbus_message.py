#!/usr/bin/env python

import socket
import sys
import os
import time
import mimetypes
import Queue
import logging

from m3_common import m3_common
from ice import ICE

m3_common.configure_root_logger()
logger = logging.getLogger(__name__)

logger.info("-" * 80)
logger.info("-- M3 GOC MBus Message Generator")
logger.info("")

ice = ICE()
try:
    ice.connect(sys.argv[1])
except IndexError:
    logger.error("USAGE: %s ICE_SERIAL_PORT" % (sys.argv[0]))
    sys.exit(1)

ice.goc_set_onoff(False)
m3_common.printing_sleep(2)
ice.goc_set_onoff(True)
m3_common.printing_sleep(2)
ice.goc_set_onoff(False)
m3_common.printing_sleep(2)

logger.info("Turning all M3 power rails on")
ice.power_set_voltage(0,0.6)
ice.power_set_voltage(1,1.2)
ice.power_set_voltage(2,3.8)
logger.info("Turning 3.8 on")
ice.power_set_onoff(2,True)
m3_common.printing_sleep(1.0)
logger.info("Turning 1.2 on")
ice.power_set_onoff(1,True)
m3_common.printing_sleep(1.0)
logger.info("Turning 0.6 on")
ice.power_set_onoff(0,True)
m3_common.printing_sleep(1.0)
logger.info("Waiting 8 seconds for power rails to settle")
m3_common.printing_sleep(8)

logger.info("M3 0.6V => OFF (reset controller)")
ice.power_set_onoff(0,False)
m3_common.printing_sleep(4.0)
logger.info("M3 0.6V => ON")
ice.power_set_onoff(0,True)
m3_common.printing_sleep(4)

# Set to .625Hz
try:
    slow_freq = float(sys.argv[3])
except IndexError:
    slow_freq = 0.625
logger.info("Sending frequency setting to ICE (%f Hz)" % (slow_freq))
ice.goc_set_frequency(slow_freq)

def write_msg_via_goc(ice, hexencoded, run_after=False):
    passcode_string = "7394"
    logger.info("Sending passcode to GOC")
    logger.debug("Sending:" + passcode_string)
    ice.goc_send(passcode_string.decode('hex'))
    m3_common.printing_sleep(4.0)
    print

    # Up ICE sending frequency to 5Hz
    fast_freq = slow_freq * 8
    logger.info("Sending 8x frequency setting to ICE (%f Hz)" % (fast_freq))
    ice.goc_set_frequency(fast_freq)

    # Build the GOC message:
    chip_id_mask = 0                # [0:3] Chip ID Mask
    reset = 0                       #   [4] Reset Request
    chip_id_coding = 0              #   [5] Chip ID coding
    is_i2c = 1                      #   [6] Indicates transmission is I2C message [addr+data]
    run_after = not not run_after   #   [7] Run code after programming?
    # Byte 0: Control
    control = chip_id_mask | (reset << 4) | (chip_id_coding << 5) | (is_i2c << 6) | (run_after << 7)

    # Byte 1,2: Chip ID
    chip_id = 0

    # Byte 3,4: Memory Address
    mem_addr = 0

    # Byte 5,6: Program Lengh
    length = len(hexencoded) >> 3   # hex exapnded -> bytes, /2
    length = socket.htons(length)

    # Byte 7: bit-wise XOR parity of header
    header_parity = 0
    for byte in (
            control,
            (chip_id >> 8) & 0xff,
            chip_id & 0xff,
            (mem_addr >> 8) & 0xff,
            mem_addr & 0xff,
            (length >> 8) & 0xff,
            length & 0xff,
            ):
        header_parity ^= byte

    # Byte 8: bit-wise XOR parity of data
    data_parity = 0
    for byte in [hexencoded[x:x+2] for x in xrange(0, len(hexencoded), 2)]:
        b = int(byte, 16)
        data_parity ^= b

    # Bytes 9+: Data

    # Assemble message:
    message = "%02X%04X%04X%04X%02X%02X%s" % (
            control,
            chip_id,
            mem_addr,
            length,
            header_parity,
            data_parity,
            hexencoded)

    logger.info("Sending program to GOC")
    logger.debug("Sending: " + message)
    ice.goc_send(message.decode('hex'))
    m3_common.printing_sleep(1.0)

    logger.info("Sending extra blink to end transaction")
    extra = "80"
    logger.debug("Sending: " + extra)
    ice.goc_send(extra.decode('hex'))

hexencoded = "a512345678"
write_msg_via_goc(ice, hexencoded)

logger.info("")
logger.info("Message Sent.")
logger.info("")
