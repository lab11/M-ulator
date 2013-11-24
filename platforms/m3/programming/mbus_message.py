#!/usr/bin/python

import sys
from time import sleep

import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('mbus_message')

from m3_common import m3_common

class mbus_message_generator(m3_common):
    TITLE = "MBus Message Generator"

    def parse_args(self):
        if len(sys.argv) not in (2,):
            logger.info("USAGE: %s SERAIL_DEVICE\n" % (sys.argv[0]))
            logger.info("")
            sys.exit(2)

        self.serial_path = sys.argv[1]

    def install_handler(self):
        self.ice.msg_handler['B++'] = self.Bpp_callback

    def Bpp_callback(address, data, control_bit_0, control_bit_1):
        logger.info("Received MBus message:")
        logger.info("  address: " + address)
        logger.info("     data: " + data)
        logger.info("      CB0: " + control_bit_0)
        logger.info("      CB1: " + control_bit_1)

    def read_binfile(self):
        pass

    def set_master(self):
        self.ice.mbus_set_master_onoff(True)

m = mbus_message_generator()

def default_value(prompt, default, extra=None):
    if extra:
        r = raw_input(prompt + ' [' + default + extra + ']: ')
    else:
        r = raw_input(prompt + ' [' + default + ']: ')
    if len(r) == 0:
        return default
    else:
        return r

def do_default(prompt, fn):
    y = default_value(prompt, 'Y', '/n')
    if y[0] not in ('n', 'N'):
        fn()

do_default("Run power-on sequence", m.power_on)
do_default("Reset M3", m.reset_m3)
do_default("Act as MBus master", m.set_master)

while True:
    try:
        logging.info("Build your MBus message. All values hex. Leading 0x optional. Ctrl-C to Quit.")
        addr = default_value("Address      ", "0xA5").replace('0x','').decode('hex')
        data = default_value("   Data", "0x12345678").replace('0x','').decode('hex')
        m.ice.mbus_send(addr, data)
    except KeyboardInterrupt:
        break

logging.info('')
logging.info("Exiting.")
