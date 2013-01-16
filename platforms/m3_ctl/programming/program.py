#!/usr/bin/env python
#
# self-test: socat PTY,link=com1 PTY,link=com2

from math import ceil
from time import sleep
import socket
import sys
import os
import mimetypes
import Queue
import logging
logging.basicConfig(level=logging.DEBUG)

logging.info("-" * 80)
logging.info("-- M3 Programmer")
logging.info("")

from ice import ICE
ice = ICE()


if len(sys.argv) not in (3,):
    logging.info("USAGE: %s BINFILE SERIAL_DEVICE\n" % (sys.argv[0]))
    sys.exit(2)

binfile = sys.argv[1]

if mimetypes.guess_type(binfile)[0] == 'text/plain':
    logging.info("Guessing hex-encoded stream for NI setup")
    logging.info("  ** This means one byte (two hex characters) per line")
    logging.info("  ** and these are the first two characters on each line.")
    logging.info("  ** If it needs to parse something more complex, let me know.")
    binfd = open(binfile, 'r')
    hexencoded = ""
    for line in binfd:
        hexencoded += line[0:2]
else:
    logging.info("Guessing compiled binary")
    binfd = open(binfile, 'rb')
    hexencoded = binfd.read().encode("hex").upper()

if (len(hexencoded) % 8) != 0:
    logging.warn("Binfile is not word-aligned. This is not a valid image")
    sys.exit(3)
else:
    logging.info("Binfile is %d bytes long\n" % (len(hexencoded) / 2))

# Callback for async I2C message
def validate_bin_helper(msg_type, event_id, length, msg):
    validate_q.put(msg)

validate_q = Queue.Queue()
ice.msg_handler['d+'] = validate_bin_helper

ice.connect(sys.argv[2])
ice.i2c_set_address("1001100x") # 0x98

logging.info("Turning all M3 power rails on")
ice.power_set_voltage(0,0.6)
ice.power_set_voltage(1,1.2)
ice.power_set_voltage(2,3.8)
ice.power_set_onoff(0,True)
ice.power_set_onoff(1,True)
ice.power_set_onoff(2,True)
sleep(1.0)

logging.info("M3 0.6V => OFF (reset controller)")
ice.power_set_onoff(0,False)
sleep(1.0)
logging.info("M3 0.6V => ON")
ice.power_set_onoff(0,True)
sleep(1.0)

resp = raw_input("About to send I2C message to wake controller. Continue? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
logging.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
logging.debug("Sending: 0xAA " + junk_dma_done_msg)
ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))

def write_bin(ice, hexencoded, offset=0):
    logging.info("Running programming sequence:")
    logging.info("\tI2C message for start DMA write at address 0x%x, length %d"
            % (offset, len(hexencoded)/2))
    logging.info("\tI2C message for DMA data")
    logging.info("\tI2C message for end of DMA at address 0x%x, length %d" %
            (offset, len(hexencoded)/2))
    logging.info("")

    length = socket.htons(len(hexencoded)/8)
    offset = socket.htons(offset)
    data = 0x40000000 | (length << 16) | offset
    dma_write_req = "%08X" % (socket.htonl(data))
    logging.debug("Sending: " + dma_write_req)
    ice.i2c_send(0xaa, dma_write_req.decode('hex'))

    logging.info("Sending data to address 0xA8")
    ice.i2c_send(0xa8, hexencoded.decode('hex'))

    data = 0x20000000 | (length << 16) | offset
    dma_done_msg = "%08X" % (socket.htonl(data))
    logging.debug("Sending: " + dma_done_msg)
    ice.i2c_send(0xaa, dma_done_msg.decode('hex'))

def validate_bin(ice, hexencoded, offset=0):
    logging.info("Running Validation sequence:")
    logging.info("\tI2C message for start DMA read at address 0x%x, length %d" %
            (offset, len(hexencoded)/2))
    logging.info("\t<Receive I2C message for DMA data>")
    logging.info("\tCompare received data and validate it was programmed correctly")
    logging.info("")

    length = len(hexencoded)/8
    offset = socket.htons(offset)
    data = 0x80000000 | (length << 16) | offset
    dma_read_req = "%08X" % (socket.htonl(data))
    logging.debug("Sending: " + dma_read_req)
    ice.i2c_send(0xaa, dma_read_req.decode('hex'))

    logging.info("Chip Program Dump Response:")
    chip_bin = validate_q.get(True, ICE.ONEYEAR)
    chip_bin = chip_bin.encode('hex')
    logging.debug(chip_bin)

    #1,2-addr ...
    chip_bin = chip_bin[2:]

    # Consistent capitalization
    chip_bin = chip_bin.upper()
    hexencoded = hexencoded.upper()

    for b in range(2, len(hexencoded)):
        try:
            if hexencoded[b] != chip_bin[b]:
                logging.warn("ERR: Mismatch at half-byte" + str(b))
                logging.warn("Expected:" + hexencoded[b])
                logging.warn("Got:" + chip_bin[b])
                return False
        except IndexError:
            logging.warn("ERR: Length mismatch")
            logging.warn("Expected %d bytes" % (len(hexencoded)/2))
            logging.warn("Got %d bytes" % (len(chip_bin)/2))
            logging.warn("All prior bytes validated correctly")
            return False

    logging.info("Programming validated successfully")
    return True

resp = raw_input("About to send program data to I2C. Continue? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

tries = 0
while True:
    write_bin(ice, hexencoded)
    sleep(1)
    if validate_bin(ice, hexencoded):
        break
    tries += 1
    if tries > 2:
        logging.info("")
        logging.info("")
        logging.info('=' * 80)
        logging.info("Maximum number of tries exceeded. Programming Failed.")
        sys.exit(-1)

logging.info("")
logging.info("")
logging.info('=' * 80)
logging.info("Programming complete.")
logging.info("")

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

logging.info("Sending 0x88 0x00000000")
ice.i2c_send(0x88, "00000000".decode('hex'))

