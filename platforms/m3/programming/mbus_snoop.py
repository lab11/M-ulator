#!/usr/bin/python

import sys
import logging

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
        print("")
        print("Received MBus message:")
        print("  address: " + address.encode('hex'))
        print("     data: " + data.encode('hex'))
        print("was_acked: " + str(not cb1))

    def read_binfile(self):
        pass

m = mbus_message_generator()
m.power_on(wait_for_rails_to_settle=False)

m.ice.mbus_set_internal_reset(True)
m.ice.mbus_set_master_onoff(False)

m.ice.mbus_set_snoop(True)

isp = m.default_value("ICE address", "0111")
m.ice.mbus_set_short_prefix(isp)

m.ice.mbus_set_internal_reset(False)

m.hang_for_messages()

