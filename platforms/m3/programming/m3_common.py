#!/usr/bin/env python

import sys
import socket
from Queue import Queue
from time import sleep

import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('m3_common')

from ice import ICE

class m3_common(object):
    TITLE = "Generic M3 Programmer"

    @staticmethod
    def printing_sleep(seconds):
        if seconds < 1:
            sleep(seconds)
            return
        while (seconds > 0):
            sys.stdout.write("\rSleeping %d s" % (int(seconds)) + ' '*20)
            sys.stdout.flush()
            sleep(min(1, seconds))
            seconds -= 1
        sys.stdout.write('\r' + ' '*80 + '\r')
        sys.stdout.flush()

    def __init__(self):
        try:
            self.print_banner()
            self.parse_args()
            self.read_binfile()
            self.ice = ICE()
            self.callback_q = Queue()
            self.install_handler()
            self.ice.connect(self.serial_path)
        except NameError:
            logger.error("Abstract element missing.")
            raise

    def install_handler(self):
        self.ice.msg_handler[self.MSG_TYPE] = self.callback_helper

    def callback_helper(self, msg_type, event_id, length, msg):
        logger.debug("Callback: msg len " + str(len(msg)))
        if len(msg) == 0:
            logger.debug("Ignore msg of len 0")
            return
        callback_q.put(msg)

    def print_banner(self):
        logger.info("-" * 80)
        logger.info(" -- " + self.TITLE)
        logger.info("")

    def parse_args(self):
        if len(sys.argv) not in (3,):
            logger.info("USAGE: %s BINFILE SERAIL_DEVICE\n" % (sys.argv[0]))
            logger.info("")
            sys.exit(2)

        self.binfile = sys.argv[1]
        self.serial_path = sys.argv[2]

    def read_binfile(self):
        def guess_type_is_hex(binfile):
            for line in open(binfile):
                for c in line.strip():
                    c = ord(c)
                    if c < 0x20 or c > 0x7a:
                        return False
            return True

        if guess_type_is_hex(self.binfile):
            logger.info("Guessing hex-encoded stream for NI setup")
            logger.info("  ** This means one byte (two hex characters) per line")
            logger.info("  ** and these are the first two characters on each line.")
            logger.info("  ** If it needs to parse something more complex, let me know.")
            binfd = open(self.binfile, 'r')
            self.hexencoded = ""
            for line in binfd:
                self.hexencoded += line[0:2].upper()
        else:
            logger.info("Guessing compiled binary")
            binfd = open(self.binfile, 'rb')
            self.hexencoded = binfd.read().encode("hex").upper()

        if (len(self.hexencoded) % 4 == 0) and (len(self.hexencoded) % 8 != 0):
            # Image is halfword-aligned. Some tools generate these, but our system
            # assumes things are word-aligned. We pad an extra nop to the end to fix
            logger.info("Padding halfword aligned image with NOP to make word-aligned.")
            self.hexencoded += '46C0' # nop; (mov r8, r8)

        if (len(self.hexencoded) % 8) != 0:
            logger.warn("Binfile is not word-aligned. This is not a valid image")
            sys.exit(3)
        else:
            logger.info("Binfile is %d bytes long\n" % (len(self.hexencoded) / 2))

    def power_on(self):
        logger.info("Turning all M3 power rails on")
        self.ice.power_set_voltage(0,0.6)
        self.ice.power_set_voltage(1,1.2)
        self.ice.power_set_voltage(2,3.8)
        logger.info("Turning 3.8 on")
        self.ice.power_set_onoff(2,True)
        sleep(1.0)
        logger.info("Turning 1.2 on")
        self.ice.power_set_onoff(1,True)
        sleep(1.0)
        logger.info("Turning 0.6 on")
        self.ice.power_set_onoff(0,True)
        sleep(1.0)
        logger.info("Waiting 8 seconds for power rails to settle")
        sleep(8.0)

    def reset_m3(self):
        logger.info("M3 0.6V => OFF (reset controller)")
        self.ice.power_set_onoff(0,False)
        sleep(2.0)
        logger.info("M3 0.6V => ON")
        self.ice.power_set_onoff(0,True)
        sleep(2.0)
