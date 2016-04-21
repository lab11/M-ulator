#!/usr/bin/python

import os
import sys
import logging
import csv

import time
import datetime
from datetime import datetime


from m3_common import m3_common

#m3_common.configure_root_logger()
#logger = logging.getLogger(__name__)

from m3_logging import get_logger
logger = get_logger(__name__)

cdc_cmeas = []
cdc_crev = []
cdc_cpar= []
cdc_cref = []
cdc_date = []
cdc_time = []


class mbus_message_generator(m3_common):
    TITLE = "MBus Message Generator"

    def __init__(self):
        super(mbus_message_generator,self).__init__()
        self.count=0
    
    def add_parse_args(self):
        super(mbus_message_generator, self).add_parse_args(require_binfile=False)
        self.parser.add_argument('-fc','--filename_cref')
        self.parser.add_argument('-fo','--filename_outp')
        self.parser.add_argument('-k','--killcount',default=1e6,type=float)

    def parse_args(self):
        super(mbus_message_generator, self).parse_args()

    def install_handler(self):
        self.ice.msg_handler['B++'] = self.Bpp_callback
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def Bpp_callback(self, address, data, cb0, cb1):
        print("@" + str(self.count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")")

    def read_binfile(self):
        pass

m = mbus_message_generator()
# Speed up
#m.ice.ice_set_baudrate_to_3_megabaud()
m.ice.ice_set_baudrate_to_2000000()

m.power_on(wait_for_rails_to_settle=False)

m.ice.mbus_set_internal_reset(True)
m.ice.mbus_set_master_onoff(False)

m.ice.mbus_set_snoop(False)

#isp = m.default_value("ICE address", "0111")
#m.ice.mbus_set_short_prefix(isp)
m.ice.mbus_set_short_prefix("0111")

#raw_input("Pausing in reset...")

m.ice.mbus_set_internal_reset(False)

m.hang_for_messages()

