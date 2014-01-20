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
        self.ice.msg_handler['b++'] = self.Bpp_callback

    def Bpp_callback(self, address, data, broadcast, success):
        logger.info("")
        logger.info("Received MBus message:")
        logger.info("  address: " + address.encode('hex'))
        logger.info("     data: " + data.encode('hex'))
        logger.info("broadcast: " + str(broadcast))
        logger.info("  success: " + str(success))
        logger.info("")

    def read_binfile(self):
        pass

    def set_master(self):
        self.ice.mbus_set_master_onoff(True)

    def set_slave(self):
        self.ice.mbus_set_master_onoff(False)

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

def do_default(prompt, fn, else_fn=None):
    y = default_value(prompt, 'Y', '/n')
    if y[0] not in ('n', 'N'):
        fn()
    else:
        if else_fn:
            else_fn()

do_default("Run power-on sequence", m.power_on)
do_default("Reset M3", m.reset_m3)
do_default("Act as MBus master", m.set_master, m.set_slave)

def build_mbus_message():
    logging.info("Build your MBus message. All values hex. Leading 0x optional. Ctrl-C to Quit.")
    addr = default_value("Address      ", "0xA5").replace('0x','').decode('hex')
    data = default_value("   Data", "0x12345678").replace('0x','').decode('hex')
    return add, data

def get_mbus_message_to_send():
    logging.info("Which message would you like to send?")
    logging.info("\t0) Custom")
    logging.info("\t1) Enumerate          (0xF0000000, 0x24000000)")
    logging.info("\t2) SNS Config Bits    (0x40, 0x0423dfef)")
    logging.info("\t2) SNS Sample Setup   (0x40, 0x030bf0f0)")
    logging.info("\t3) SNS Sample Start   (0x40, 0x030af0f0)")
    selection = default_value("Choose a message type", "-1")
    if selection == '0':
        return build_mbus_message()
    elif selection == '1':
        return ("F0000000".decode('hex'), "24000000".decode('hex'))
    elif selection == '2':
        return ("40".decode('hex'), "0423dfef".decode('hex'))
    elif selection == '3':
        return ("40".decode('hex'), "030bf0f0".decode('hex'))
    elif selection == '4':
        return ('40'.decode('hex'), '030af0f0'.decode('hex'))
    else:
        logging.info("Please choose one of the numbered options")
        return get_mbus_message_to_send()

while True:
    try:
        addr, data = get_mbus_message_to_send()
        m.ice.mbus_send(addr, data)
    except KeyboardInterrupt:
        break

logging.info('')
logging.info("Exiting.")
