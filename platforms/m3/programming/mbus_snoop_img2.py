#!/usr/bin/python

import os
import sys
import logging
import csv

import time
import datetime
from datetime import datetime


import m3_common

#m3_common.configure_root_logger()
#logger = logging.getLogger(__name__)

from m3_logging import get_logger
logger = get_logger(__name__)


def Bpp_callback(address, data, cb0, cb1):
	print(" Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")")

m = m3_common.mbus_snooper(Bpp_callback)
m.hang_for_messages()

