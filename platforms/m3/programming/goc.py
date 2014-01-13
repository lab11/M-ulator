#!/usr/bin/env python

import sys, socket
from time import sleep

import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('goc_programmer')

from m3_common import m3_common

class goc_programmer(m3_common):
    TITLE = "GOC Programmer"
    SLOW_FREQ_IN_HZ = 0.625

    def set_slow_frequency(self):
        self.ice.goc_set_frequency(self.SLOW_FREQ_IN_HZ)

    def wake_chip(self):
        passcode_string = "7394"
        logger.info("Sending passcode to GOC")
        logger.debug("Sending:" + passcode_string)
        self.ice.goc_send(passcode_string.decode('hex'))
        sleep(4.0)

    def set_fast_frequency(self):
        self.ice.goc_set_frequency(8*self.SLOW_FREQ_IN_HZ)

    def build_goc_message(self, run_after):
        chip_id_mask = 0                # [0:3] Chip ID Mask
        reset = 0                       #   [4] Reset Request
        chip_id_coding = 0              #   [5] Chip ID coding
        is_i2c = 0                      #   [6] Indicates transmission is I2C message [addr+data]
        run_after = not not run_after   #   [7] Run code after programming?
        # Byte 0: Control
        control = chip_id_mask | (reset << 4) | (chip_id_coding << 5) | (is_i2c << 6) | (run_after << 7)

        # Byte 1,2: Chip ID
        chip_id = 0

        # Byte 3,4: Memory Address
        mem_addr = 0

        # Byte 5,6: Program Lengh
        length = len(self.hexencoded) >> 3   # hex exapnded -> bytes, /2
        length = socket.htons(length)

        # Byte 7: bit-wise XOR parity of header
        header_parity = 0
        for byte in (
                control,
                (chip_id >> 8) & 0xff,
                chip_id & 0xff,
                (mem_addr >> 8) & 0xff,
                mem_addr & 0xff,
                (length >> 8) & 0xff,
                length & 0xff,
                ):
            header_parity ^= byte

        # Byte 8: bit-wise XOR parity of data
        data_parity = 0
        for byte in [self.hexencoded[x:x+2] for x in xrange(0, len(self.hexencoded), 2)]:
            b = int(byte, 16)
            data_parity ^= b

        # Bytes 9+: Data

        # Assemble message:
        message = "%02X%04X%04X%04X%02X%02X%s" % (
                control,
                chip_id,
                mem_addr,
                length,
                header_parity,
                data_parity,
                self.hexencoded)

        return message

    def send_goc_message(self, message):
        logger.info("Sending GOC message")
        logger.debug("Sending: " + message)
        self.ice.goc_send(message.decode('hex'))
        sleep(1.0)

        logger.info("Sending extra blink to end transaction")
        extra = "80"
        logger.debug("Sending: " + extra)
        self.ice.goc_send(extra.decode('hex'))

class goc_programmer_for_i2c(goc_programmer):
    MSG_TYPE = 'd+'

class goc_programmer_for_mbus(goc_programmer):
    MSG_TYPE = 'b+'

g = goc_programmer_for_i2c()

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

g.power_on()
g.reset_m3()
g.set_slow_frequency()
g.wake_chip()
g.set_fast_frequency()
message = g.build_goc_message(run_after)
g.send_goc_message(message)

logger.info("")
logger.info("Programming complete.")
logger.info("")

if run_after:
    logger.info("Program is running on the chip.")
    sys.exit(0)

########################################
resp = raw_input("Would you like to read back the program to validate? [Y/n] ")
if not (len(resp) != 0 and resp[0] in ('n', 'N')):
    junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
    logger.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
    logger.debug("Sending: 0xAA " + junk_dma_done_msg)
    ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))
    sleep(1.0)
    if validate_bin(ice, hexencoded) is False:
        logger.warn("Validation failed. Dying")
        sys.exit()

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

logger.info("Sending 0x88 0x00000000")
ice.i2c_send(0x88, "00000000".decode('hex'))

sleep(10000)
