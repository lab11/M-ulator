#!/usr/bin/env python

from time import sleep
import socket
import sys
import os
import Queue
import logging
logging.basicConfig(level=logging.INFO)

logging.info("-" * 80)
logging.info("-- M3 Programmer")
logging.info("")

from ice import ICE
ice = ICE()

if len(sys.argv) not in (2,):
    logging.info("USAGE: %s SERIAL_DEVICE\n" % (sys.argv[0]))
    sys.exit(2)

# Callback for async I2C message
def async_helper(msg_type, event_id, length, msg):
    logging.debug("Bin Helper got msg len" + str(len(msg)))
    if len(msg) == 0:
        logging.debug("Ignore msg of len 0")
        return
    #msq_q.put(msg)
    logging.info("Got msg: " + msg.encode('hex'))

#msq_q = Queue.Queue()
ice.msg_handler['d+'] = async_helper

ice.connect(sys.argv[1])
ice.i2c_set_address("1001100x") # 0x98

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

logging.info("Sending 0x88 0x00000000")
ice.i2c_send(0x88, "00000000".decode('hex'))

logging.info("Listening...")
resp = raw_input("Press enter at any time to quit\n")
