#!/usr/bin/python

import sys
import logging
import time

import threading

from m3_common import m3_common

#m3_common.configure_root_logger()
#logger = logging.getLogger(__name__)

from m3_logging import get_logger
logger = get_logger(__name__)

class mbus_message_generator(m3_common):
    TITLE = "MBus Message Generator"

    def add_parse_args(self):
        super(mbus_message_generator, self).add_parse_args(require_binfile=False)

    def install_handler(self):
        self.ice.msg_handler['B++'] = self.Bpp_callback
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def Bpp_callback(self, address, data, cb0, cb1):
        global reset_event
        reset_event.set()

        #print("  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")")
        print(("@ Time: " + time.strftime("%Y-%m-%d %H:%M:%S") + "  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")"))

    def read_binfile(self):
        pass

m = mbus_message_generator()
m.power_on(wait_for_rails_to_settle=False)

m.ice.mbus_set_internal_reset(True)
m.ice.mbus_set_master_onoff(False)

# Set this to "True" to record all messages 
m.ice.mbus_set_snoop(False)

# For ACK from ICE board & filtering
#isp = m.default_value("ICE address", "0111")
m.ice.mbus_set_short_prefix("0111")

m.ice.mbus_set_internal_reset(False)

# Separate thread that tries to reset ICE if it hasn't seen a message in the
# last 10 seconds
reset_event = threading.Event()

def reset_mbus():
    while True:
        if reset_event.wait(10):
            reset_event.clear()
        else:
            logger.warn("No messages for 10 seconds, resetting ICE")
            m.ice.mbus_set_internal_reset(True)
            m.ice.mbus_set_internal_reset(False)
            m.ice.mbus_set_internal_reset(True)
            m.ice.mbus_set_internal_reset(False)

thread = threading.Thread(target=reset_mbus)
thread.daemon = True
thread.start()

m.hang_for_messages()

