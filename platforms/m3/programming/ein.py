#!/usr/bin/env python

import sys, socket
import time
import Queue
import logging

from m3_common import m3_common

#m3_common.configure_root_logger()
#logger = logging.getLogger(__name__)

from m3_logging import get_logger
logger = get_logger(__name__)

class ein_programmer(m3_common):
    TITLE = "EIN Programmer"
    MSG_TYPE = 'b+'

    def DMA_start_interrupt(self):
        logger.info("Sending 0x88 0x00000000")
        self.ice.mbus_send("88".decode('hex'), "00000000".decode('hex'))

# Callback for async MBus message
#ice.msg_handler['b+'] = validate_bin_helper
#validate_q = Queue.Queue()
def validate_bin_helper(msg_type, event_id, length, msg):
    logger.debug("Bin Helper got msg len" + str(len(msg)))
    if len(msg) == 0:
        logger.debug("Ignore msg of len 0")
        return
    validate_q.put(msg)

e = ein_programmer()
m3_common.dont_do_default("Run power-on sequence", e.power_on)
m3_common.dont_do_default("Reset M3", e.reset_m3)
logger.info("** Setting ICE MBus controller to slave mode")
e.ice.mbus_set_master_onoff(False)

logger.info("")
logger.info("Would you like to run after programming? If you do not")
logger.info("have EIN Debug start the program, you will be prompted")
logger.info("to send the start message via MBus at the end instead")
logger.info("")
resp = raw_input("Run program when programming finishes? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    run_after = 0
else:
    run_after = 1

message = e.build_injection_message(e.hexencoded, run_after)
logger.debug("Sending: " + message)
e.ice.ein_send(message.decode('hex'))

logger.info("")
logger.info("Programming complete.")
logger.info("")

if run_after:
    logger.info("Program is running on the chip")
else:
    m3_common.do_default("Would you like to read back the program to validate?", e.validate_bin)
    m3_common.do_default("Would you like to send the DMA start interrupt?", e.DMA_start_interrupt)

