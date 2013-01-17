#!/usr/bin/env python

from time import sleep
import socket
import sys
import os
import time
import mimetypes
import Queue
import logging
logging.basicConfig(level=logging.DEBUG)

logging.info("-" * 80)
logging.info("-- M3 GOC Programmer")
logging.info("")

from ice import ICE
ice = ICE()

if len(sys.argv) not in (3,):
    logging.info("USAGE: %s BINFILE SERAIL_DEVICE\n" % (sys.argv[0]))
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
    logging.debug("Bin Helper got msg len" + str(len(msg)))
    if len(msg) == 0:
        logging.debug("Ignore msg of len 0")
        return
    validate_q.put(msg)

validate_q = Queue.Queue()
ice.msg_handler['d+'] = validate_bin_helper

ice.connect(sys.argv[2])

logging.info("Turning all M3 power rails on")
ice.power_set_voltage(0,0.6)
ice.power_set_voltage(1,1.2)
ice.power_set_voltage(2,3.8)
logging.info("Turning 3.8 on")
ice.power_set_onoff(2,True)
sleep(1.0)
logging.info("Turning 1.2 on")
ice.power_set_onoff(1,True)
sleep(1.0)
logging.info("Turning 0.6 on")
ice.power_set_onoff(0,True)
sleep(1.0)
logging.info("Waiting 8 seconds for power rails to settle")
sleep(8.0)

logging.info("M3 0.6V => OFF (reset controller)")
ice.power_set_onoff(0,False)
sleep(4.0)
logging.info("M3 0.6V => ON")
ice.power_set_onoff(0,True)
sleep(4.0)

#junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
#logging.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
#logging.debug("Sending: 0xAA " + junk_dma_done_msg)
#ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))
#sleep(1.0)

logging.info("")
logging.info("Would you like to run after programming? If you do not")
logging.info("have GOC start the program, you will be prompted to send")
logging.info("the start message via DMA at the end instead")
logging.info("")
resp = raw_input("Run program when GOC finishes? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    run_after = 0
else:
    run_after = 1

# Set to .625Hz
logging.info("Sending frequency setting to ICE (.625Hz)")
ice.goc_set_frequency(0.625)

def write_bin_via_goc(ice, hexencoded, run_after):
    passcode_string = "7394"
    logging.info("Sending passcode to GOC")
    logging.debug("Sending:" + passcode_string)
    ice.goc_send(passcode_string.decode('hex'))
    sleep(4.0)
    print

    # Up ICE sending frequency to 5Hz
    logging.info("Sending 8x frequency setting to ICE (5Hz)")
    ice.goc_set_frequency(5)

    # Build the GOC message:
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
    length = len(hexencoded) >> 3   # hex exapnded -> bytes, /2
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
    for byte in [hexencoded[x:x+2] for x in xrange(0, len(hexencoded), 2)]:
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
            hexencoded)

    logging.info("Sending program to GOC")
    logging.debug("Sending: " + message)
    ice.goc_send(message.decode('hex'))
    sleep(1.0)

    logging.info("Sending extra blink to end transaction")
    extra = "80"
    logging.debug("Sending: " + extra)
    ice.goc_send(extra.decode('hex'))

def validate_bin(ice, hexencoded, offset=0):
    logging.info("Configuring ICE to ACK adress 1001 100x")
    ice.i2c_set_address("1001100x") # 0x98

    logging.info("Running Validation sequence:")
    logging.info("\t DMA read at address 0x%x, length %d" % (offset, len(hexencoded)/2))
    logging.info("\t<Receive I2C message for DMA data>")
    logging.info("\tCompare received data and validate it was programmed correctly")

    length = len(hexencoded)/8
    offset = offset
    data = 0x80000000 | (length << 16) | offset
    dma_read_req = "%08X" % (socket.htonl(data))
    logging.debug("Sending: " + dma_read_req)
    ice.i2c_send(0xaa, dma_read_req.decode('hex'))

    logging.info("Chip Program Dump Response:")
    chip_bin = validate_q.get(True, ice.ONEYEAR)
    logging.debug("Raw chip bin response len " + str(len(chip_bin)))
    chip_bin = chip_bin.encode('hex')
    logging.debug("Chip bin len %d val: %s" % (len(chip_bin), chip_bin))

    #1,2-addr ...
    chip_bin = chip_bin[2:]

    # Consistent capitalization
    chip_bin = chip_bin.upper()
    hexencoded = hexencoded.upper()

    for b in range(len(hexencoded)):
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

write_bin_via_goc(ice, hexencoded, run_after)

logging.info("")
logging.info("Programming complete.")
logging.info("")

resp = raw_input("Would you like to read back the program via I2C to validate? [Y/n] ")
if not (len(resp) != 0 and resp[0] in ('n', 'N')):
    junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
    logging.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
    logging.debug("Sending: 0xAA " + junk_dma_done_msg)
    ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))
    sleep(1.0)
    if validate_bin(ice, hexencoded) is False:
        logging.warn("Validation failed. Dying")
        sys.exit()

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

logging.info("Sending 0x88 0x00000000")
ice.i2c_send(0x88, "00000000".decode('hex'))

sleep(10000)
