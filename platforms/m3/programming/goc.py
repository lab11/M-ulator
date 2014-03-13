#!/usr/bin/env python

import sys, socket

import time

from m3_common import m3_common, goc_programmer

import m3_logging
logger = m3_logging.get_logger(__name__)
logger.debug('Got goc.py logger')

class goc_programmer_for_i2c(goc_programmer):
    MSG_TYPE = 'd+'

    def send(self, addr, data):
        return self.ice.i2c_send(ord(addr), data)

    def validate_bin(self):
        junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
        logger.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
        logger.debug("Sending: 0xAA " + junk_dma_done_msg)
        ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))
        sleep(1.0)
        super(goc_programmer_for_i2c, self).validate_bin()

class goc_programmer_for_mbus(goc_programmer):
    MSG_TYPE = 'b+'

    def add_parse_args(self):
        super(goc_programmer_for_mbus, self).add_parse_args()
        self.parser.add_argument('-g', '--goc-speed',
                help="GOC Slow Speed in Hz. The fast speed will be 8x faster."\
                        " Defaults to " + str(self.SLOW_FREQ_IN_HZ) + " Hz.",
                        default=self.SLOW_FREQ_IN_HZ)

    def parse_args(self):
        super(goc_programmer_for_mbus, self).parse_args()
        self.SLOW_FREQ_IN_HZ = float(self.args.goc_speed)

    def send(self, addr, data):
        return self.ice.mbus_send(addr, data)

g = goc_programmer_for_mbus()
m3_common.dont_do_default("Run power-on sequence", g.power_on)
m3_common.dont_do_default("Reset M3", g.reset_m3)
logger.info("** Setting ICE MBus controller to slave mode")
g.ice.mbus_set_master_onoff(False)

logger.info("")
logger.info("Would you like to run after programming? If you do not")
logger.info("have GOC start the program, you will be prompted to send")
logger.info("the start message via DMA at the end instead")
logger.info("")
resp = raw_input("Run program when GOC finishes? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    run_after = 0
else:
    run_after = 1

g.set_slow_frequency()
g.wake_chip()
g.set_fast_frequency()
message = g.build_injection_message(g.hexencoded, run_after)
g.send_goc_message(message)

logger.info("")
logger.info("Programming complete.")
logger.info("")

if run_after:
    logger.info("Program is running on the chip.")
else:
    m3_common.do_default("Would you like to read back the program to validate?", g.validate_bin)
    m3_common.do_default("Would you like to send the DMA start interrupt?", g.DMA_start_interrupt)

g.hang_for_messages()

