#!/usr/bin/env python

from time import sleep
import socket
import sys
import os
import queue
import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('program')

logger.info("-" * 80)
logger.info("-- M3 Programmer")
logger.info("")

from ice import ICE
ice = ICE()

if len(sys.argv) not in (2,):
    logger.info("USAGE: %s SERIAL_DEVICE\n" % (sys.argv[0]))
    sys.exit(2)

# Callback for async I2C message
def async_helper(msg_type, event_id, length, msg):
    logger.debug("Bin Helper got msg len" + str(len(msg)))
    if len(msg) == 0:
        logger.debug("Ignore msg of len 0")
        return
    #msq_q.put(msg)
    logger.info("Got msg: " + msg.encode('hex'))

#msq_q = Queue.Queue()
ice.msg_handler['d+'] = async_helper

ice.connect(sys.argv[1])
ice.i2c_set_address("1001100x") # 0x98

resp = input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

logger.info("Sending 0x88 0x00000000")
ice.i2c_send(0x88, "00000000".decode('hex'))

logger.info("Listening...")
resp = input("Press enter at any time to quit\n")
