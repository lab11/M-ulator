#!/usr/bin/env python

import sys
import struct
import socket
import logging
logging.basicConfig(level=logging.INFO)

from ice import ICE
ice = ICE()

if len(sys.argv) not in (4,):
	logging.info("USAGE: %s ADDR DATA SERIAL_DEVICE\n" % (sys.argv[0]))
	sys.exit(2)

addr = int(sys.argv[1], 16)
data = int(sys.argv[2], 16)

if addr > 255:
	logging.warn("ERR: Value %02x for address is > 0xff" % (addr))
	sys.exit(3)

ice.connect(sys.argv[3])

junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
logging.info("Sending junk message to ensure chip is awake")
logging.info("Sending: 0xAA" + junk_dma_done_msg)
ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))

data = socket.htonl(data)
cmd_string = "%08X" % (data)
logging.info("Sending: %02x %s" % (addr, cmd_string))
ice.i2c_send(addr, cmd_string.decode('hex'))

logging.info("If no exception was thrown, message was sent successfully.")
