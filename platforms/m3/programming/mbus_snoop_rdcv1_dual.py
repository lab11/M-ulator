#!/usr/bin/python

import os
import sys
import logging
import csv
import binascii

import time
import datetime
from datetime import datetime


#Adjust the default timezone for Cygwin
from sys import platform
if platform == 'cygwin':
    os.system('export TZ="UTC+4"')

# If you version of m3-ice is out of date, 
# you might need to do: 
# $pip install -U m3
from m3.m3_common import m3_common 

logfile = open('mbus_snoop_log.txt','w')

wr = csv.writer(open('rdcv1_dual_snoop.txt','w'), delimiter=',', lineterminator='\n')
wr.writerow(['DATE','TIME','RDC1','RDC2'])


class mbus_message_generator(m3_common):
    TITLE = "MBus Message Generator"

    def __init__(self):
        super(mbus_message_generator,self).__init__()
        self.count=0
        self.rdc1_data = 0
        self.rdc2_data = 0
        self.rdc_date = 0
        self.rdc_time = 0
        self.rdc_group = False
        
        #add our handlers here, so they don't get called
        # before we're finished setup
        self.ice.msg_handler['B++'] = self.Bpp_callback
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def add_parse_args(self):
        super(mbus_message_generator, self).add_parse_args()
        self.parser.add_argument('-fc','--filename_cref')
        self.parser.add_argument('-fo','--filename_outp')
        self.parser.add_argument('-k','--killcount',default=1e6,type=float)

    def parse_args(self):
        super(mbus_message_generator, self).parse_args()

    # no longer used
    # this gets called by super().__init__() before we're finished
    # doing setup, which caused Bpp_callback to get called by another thread
    # before __init__ had completed
    #def install_handler(self):
    #    pass

    def Bpp_callback(self, address, data, cb0=-1, cb1=-1):

		print("@" + str(self.count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + binascii.hexlify(address)+ "  DATA: 0x" + binascii.hexlify(data) + "  (ACK: " + str(not cb1) + ")")
		print >> logfile, "@" + str(self.count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + binascii.hexlify(address) + "  DATA: 0x" + binascii.hexlify(data)+ "  (ACK: " + str(not cb1) + ")"
		if (str(int(binascii.hexlify(address),16))=="193"):
			self.rdc_group = True
			self.rdc1_data = int(binascii.hexlify(data),16)
			self.rdc_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
			self.rdc_date = datetime.now().strftime("%Y-%m-%d")
		elif (str(int(binascii.hexlify(address),16))=="194"):
			if self.rdc_group:
				self.rdc2_data = int(binascii.hexlify(data),16)
				wr.writerow([self.rdc_date,self.rdc_time,self.rdc1_data,self.rdc2_data])
				self.count += 1
				self.rdc_group = False
		if self.count>self.args.killcount:
			self.exit()

    def read_binfile(self):
        pass

m = mbus_message_generator()
m.power_on(wait_for_rails_to_settle=False)

m.ice.mbus_set_internal_reset(True)
m.ice.mbus_set_master_onoff(False)

m.ice.mbus_set_snoop(True)

#isp = m.default_value("ICE address", "0111")
#m.ice.mbus_set_short_prefix(isp)
m.ice.mbus_set_short_prefix("1100")
#m.ice.mbus_set_short_prefix("xxxx")

#raw_input("Pausing in reset...")

m.ice.mbus_set_internal_reset(False)

m.hang_for_messages()

