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

    def parse_args(self):
        if len(sys.argv) not in (2,):
            logger.error("Serial device required.")
            logger.info("USAGE: %s SERIAL_DEVICE\n" % (sys.argv[0]))
            logger.info("")
            sys.exit(2)

        self.serial_path = sys.argv[1]

    def install_handler(self):
        self.ice.msg_handler['B++'] = self.Bpp_callback
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def Bpp_callback(self, address, data, broadcast, was_acked):
        print("")
        print("Received MBus message:")
        print("  address: " + address.encode('hex'))
        print("     data: " + data.encode('hex'))
        print("broadcast: " + str(broadcast))
        print("was_acked: " + str(was_acked))

    def read_binfile(self):
        pass

    def set_master(self):
        self.ice.mbus_set_master_onoff(True)

    def set_slave(self):
        self.ice.mbus_set_master_onoff(False)

m = mbus_message_generator()

# sp = m3_common.default_value("Snoop short prefixes matching", "xxxx")
# m.ice.mbus_set_short_snoop_prefix(sp)
# fp = m3_common.default_value("Snoop full prefixes matching", "xxxxxxxxxxxxxxxxxxxx")
# m.ice.mbus_set_full_snoop_prefix(fp)
# bm = m3_common.default_value("Snoop broadcast messages matching", "xxxx")
# m.ice.mbus_set_broadcast_channel_snoop_mask(bm)


isp = m3_common.default_value("ICE address", "0111")
m.ice.mbus_set_short_prefix(isp)


m.hang_for_messages()

