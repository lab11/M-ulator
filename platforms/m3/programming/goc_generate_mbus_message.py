#!/usr/bin/env python

import sys, socket
import logging
import argparse

from m3_common import m3_common, goc_programmer

m3_common.configure_root_logger()
logger = logging.getLogger(__name__)

class mbus_message(goc_programmer):
    TITLE = "GOC MBus generator"
    MSG_TYPE = 'b+'

    def parse_args(self):
        self.parser = argparse.ArgumentParser()
        self.parser.add_argument('-g', '--goc-speed',
        help="GOC Slow Speed in Hz. The fast speed will be 8x faster."\
                " Defaults to " + str(self.SLOW_FREQ_IN_HZ) + " Hz.",
                default=self.SLOW_FREQ_IN_HZ)
        self.parser.add_argument("SERIAL", help="Path to ICE serial device", nargs='?')
        self.args = self.parser.parse_args()
        if self.args.SERIAL is None:
            self.serial_path = self.guess_serial()
        else:
            self.serial_path = self.args.SERIAL
        self.SLOW_FREQ_IN_HZ = float(self.args.goc_speed)

    def read_binfile(self):
        pass

m = mbus_message()
m.dont_do_default("Run power-on sequence", m.power_on)
m.dont_do_default("Reset M3", m.reset_m3)
logger.info("** Setting ICE MBus controller to slave mode")
m.ice.mbus_set_master_onoff(False)
m.set_slow_frequency()
m.wake_chip()
m.set_fast_frequency()
message = m.build_injection_message(hexencoded_data="a512345678")
m.send_goc_message(message)

logger.info("")
logger.info("Message Sent.")
logger.info("")

