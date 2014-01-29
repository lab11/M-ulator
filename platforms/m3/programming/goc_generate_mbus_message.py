#!/usr/bin/env python

import sys, socket
import logging

from m3_common import m3_common, goc_programmer

m3_common.configure_root_logger()
logger = logging.getLogger(__name__)

class mbus_message(goc_programmer):
    TITLE = "GOC MBus generator"
    MSG_TYPE = 'b+'

    def parse_args(self):
        if len(sys.argv) not in (2,):
            logger.error("USAGE: %s ICE_SERIAL_PORT" % (sys.argv[0]))
            sys.exit(2)
        self.serial_path = sys.argv[1]

    def read_binfile(self):
        pass

m = mbus_message()
m3_common.dont_do_default("Run power-on sequence", m.power_on)
m3_common.dont_do_default("Reset M3", m.reset_m3)
logger.info("** Setting ICE MBus controller to slave mode")
m.ice.mbus_set_master_onoff(False)
m.set_slow_frequency()
m.wake_chip()
m.set_fast_frequency()
message = m.build_injection_message("a512345678")
m.send_goc_message(message)

logger.info("")
logger.info("Message Sent.")
logger.info("")

m.hang_for_messages()
