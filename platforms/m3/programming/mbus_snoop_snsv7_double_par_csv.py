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

wr = csv.writer(open('snsv7_double_snoop.txt','w'), delimiter=',', lineterminator='\n')
wr.writerow(['DATE','TIME','C_MEAS','C_REF','C_REV','C_PAR','C_MEAS2','C_REF2','C_REV2','C_PAR2'])


class mbus_message_generator(m3_common):
    TITLE = "MBus Message Generator"

    def __init__(self):
        super(mbus_message_generator,self).__init__()
        self.count=0
        self.cdc_cmeas = 0
        self.cdc_crev = 0
        self.cdc_cpar = 0
        self.cdc_cref = 0
        self.cdc2_cmeas = 0
        self.cdc2_crev = 0
        self.cdc2_cpar = 0
        self.cdc2_cref = 0
        self.cdc_date = 0
        self.cdc_time = 0
        self.cdc_group = False

    
    def add_parse_args(self):
        super(mbus_message_generator, self).add_parse_args()
        self.parser.add_argument('-fc','--filename_cref')
        self.parser.add_argument('-fo','--filename_outp')
        self.parser.add_argument('-k','--killcount',default=1e6,type=float)

    def parse_args(self):
        super(mbus_message_generator, self).parse_args()

    def install_handler(self):
        self.ice.msg_handler['B++'] = self.Bpp_callback
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def Bpp_callback(self, address, data, cb0=-1, cb1=-1):
		print("@" + str(self.count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + binascii.hexlify(address)+ "  DATA: 0x" + binascii.hexlify(data) + "  (ACK: " + str(not cb1) + ")")
		print >> logfile, "@" + str(self.count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + binascii.hexlify(address) + "  DATA: 0x" + binascii.hexlify(data)+ "  (ACK: " + str(not cb1) + ")"
		if (str(int(binascii.hexlify(address),16))=="116"):
			self.cdc_group = True
			self.cdc_cmeas = int(binascii.hexlify(data),16)
			self.cdc_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
			self.cdc_date = datetime.now().strftime("%Y-%m-%d")
		elif (str(int(binascii.hexlify(address),16))=="118"):
			if self.cdc_group:
				self.cdc_cref = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="119"):
			if self.cdc_group:
				self.cdc_crev = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="121"):
			if self.cdc_group:
				self.cdc_cpar = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="122"):
			if self.cdc_group:
				self.cdc2_cmeas = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="123"):
			if self.cdc_group:
				self.cdc2_cref = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="124"):
			if self.cdc_group:
				self.cdc2_crev = int(binascii.hexlify(data),16)
		elif (str(int(binascii.hexlify(address),16))=="125"):
			if self.cdc_group:
				self.cdc2_cpar = int(binascii.hexlify(data),16)
				wr.writerow([self.cdc_date,self.cdc_time,self.cdc_cmeas,self.cdc_cref,self.cdc_crev,self.cdc_cpar,self.cdc2_cmeas,self.cdc2_cref,self.cdc2_crev,self.cdc2_cpar])
				self.count += 1
				self.cdc_group = False
		if self.count>self.args.killcount:
			self.exit()

    def read_binfile(self):
        pass

m = mbus_message_generator()
m.power_on(wait_for_rails_to_settle=False)

m.ice.mbus_set_internal_reset(True)
m.ice.mbus_set_master_onoff(False)

m.ice.mbus_set_snoop(False)

#isp = m.default_value("ICE address", "0111")
#m.ice.mbus_set_short_prefix(isp)
m.ice.mbus_set_short_prefix("0111")
#m.ice.mbus_set_short_prefix("xxxx")

#raw_input("Pausing in reset...")

m.ice.mbus_set_internal_reset(False)

m.hang_for_messages()

