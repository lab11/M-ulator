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

#m.do_default("Run power-on sequence", m.power_on)
#m.do_default("Reset M3", m.reset_m3)
m.power_on(wait_for_rails_to_settle=False)
m.do_default("Act as MBus master", m.set_master, m.set_slave)

def build_mbus_message():
    logger.info("Build your MBus message. All values hex. Leading 0x optional. Ctrl-C to Quit.")
    addr = m.default_value("Address      ", "0xA5").replace('0x','').decode('hex')
    data = m.default_value("   Data", "0x12345678").replace('0x','').decode('hex')
    return addr, data

def get_mbus_message_to_send():
    logger.info("Which message would you like to send?")
    logger.info("\t0) Custom")
    logger.info("\t1) Enumerate          (0xF0000000, 0x24000000)")
    logger.info("\t2) SNS Config Bits    (0x40, 0x0423dfef)")
    logger.info("\t3) SNS Sample Setup   (0x40, 0x030bf0f0)")
    logger.info("\t4) SNS Sample Start   (0x40, 0x030af0f0)")
    selection = m.default_value("Choose a message type", "-1")
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
        logger.info("Please choose one of the numbered options")
        return get_mbus_message_to_send()

while True:
    try:
        addr, data = get_mbus_message_to_send()
        m.ice.mbus_send(addr, data)
    except KeyboardInterrupt:
        break

logger.info('')
logger.info("Exiting.")
