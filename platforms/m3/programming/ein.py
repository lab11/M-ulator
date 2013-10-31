#!/usr/bin/env python

from time import sleep
import socket
import sys
import os
import time
import mimetypes
import Queue
import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('goc')

logger.info("-" * 80)
logger.info("-- M3 EIN Debug Port Programmer")
logger.info("")

from ice import ICE
ice = ICE()

if len(sys.argv) not in (3,):
    logger.info("USAGE: %s BINFILE ICE_SERAIL_DEVICE\n" % (sys.argv[0]))
    logger.info("")
    sys.exit(2)

binfile = sys.argv[1]

if mimetypes.guess_type(binfile)[0] == 'text/plain':
    logger.info("Guessing hex-encoded stream for NI setup")
    logger.info("  ** This means one byte (two hex characters) per line")
    logger.info("  ** and these are the first two characters on each line.")
    logger.info("  ** If it needs to parse something more complex, let me know.")
    binfd = open(binfile, 'r')
    hexencoded = ""
    for line in binfd:
        hexencoded += line[0:2].upper()
else:
    logger.info("Guessing compiled binary")
    binfd = open(binfile, 'rb')
    hexencoded = binfd.read().encode("hex").upper()

if (len(hexencoded) % 4 == 0) and (len(hexencoded) % 8 != 0):
    # Image is halfword-aligned. Some tools generate these, but our system
    # assumes things are word-aligned. We pad an extra nop to the end to fix
    hexencoded += '46C0' # nop; (mov r8, r8)

if (len(hexencoded) % 8) != 0:
    logger.warn("Binfile is not word-aligned. This is not a valid image")
    sys.exit(3)
else:
    logger.info("Binfile is %d bytes long\n" % (len(hexencoded) / 2))


# Callback for async MBus message
def validate_bin_helper(msg_type, event_id, length, msg):
    logger.debug("Bin Helper got msg len" + str(len(msg)))
    if len(msg) == 0:
        logger.debug("Ignore msg of len 0")
        return
    validate_q.put(msg)

validate_q = Queue.Queue()
ice.msg_handler['b+'] = validate_bin_helper

ice.connect(sys.argv[2])

logger.info("Turning all M3 power rails on")
ice.power_set_voltage(0,0.6)
ice.power_set_voltage(1,1.2)
ice.power_set_voltage(2,3.8)
logger.info("Turning 3.8 on")
ice.power_set_onoff(2,True)
sleep(1.0)
logger.info("Turning 1.2 on")
ice.power_set_onoff(1,True)
sleep(1.0)
logger.info("Turning 0.6 on")
ice.power_set_onoff(0,True)
sleep(1.0)
logger.info("Waiting 8 seconds for power rails to settle")
sleep(8.0)

logger.info("M3 0.6V => OFF (reset controller)")
ice.power_set_onoff(0,False)
sleep(4.0)
logger.info("M3 0.6V => ON")
ice.power_set_onoff(0,True)
sleep(4.0)

#junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
#logger.info("Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake")
#logger.debug("Sending: 0xAA " + junk_dma_done_msg)
#ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))
#sleep(1.0)

logger.info("")
logger.info("Would you like to run after programming? If you do not")
logger.info("have EIN Debug start the program, you will be prompted")
logger.info("to send the start message via MBus at the end instead")
logger.info("")
logger.info("NOTE: MBus not yet supported, so run it now or hold your peace")
logger.info("")
resp = raw_input("Run program when programming finishes? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    run_after = 0
else:
    run_after = 1

def write_bin_via_ein(ice, hexencoded, run_after):
    #No passcode in EIN
    #passcode_string = "7394"
    #logger.info("Sending passcode to GOC")
    #logger.debug("Sending:" + passcode_string)
    #ice.goc_send(passcode_string.decode('hex'))
    #sleep(4.0)
    #print

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

    logger.info("Sending program to EIN")
    logger.debug("Sending: " + message)
    ice.ein_send(message.decode('hex'))
    sleep(1.0)

    if run_after:
        logger.info("Run After Programming specified. Chip should be running.  Exiting.")
        sys.exit(0)

def validate_bin(ice, hexencoded, offset=0):
    logger.error("Need to update to MBus")
    sys.exit(-1)
    logger.info("Configuring ICE to ACK adress 1001 100x")
    ice.i2c_set_address("1001100x") # 0x98

    logger.info("Running Validation sequence:")
    logger.info("\t DMA read at address 0x%x, length %d" % (offset, len(hexencoded)/2))
    logger.info("\t<Receive I2C message for DMA data>")
    logger.info("\tCompare received data and validate it was programmed correctly")

    length = len(hexencoded)/8
    offset = offset
    data = 0x80000000 | (length << 16) | offset
    dma_read_req = "%08X" % (socket.htonl(data))
    logger.debug("Sending: " + dma_read_req)
    ice.i2c_send(0xaa, dma_read_req.decode('hex'))

    logger.info("Chip Program Dump Response:")
    chip_bin = validate_q.get(True, ice.ONEYEAR)
    logger.debug("Raw chip bin response len " + str(len(chip_bin)))
    chip_bin = chip_bin.encode('hex')
    logger.debug("Chip bin len %d val: %s" % (len(chip_bin), chip_bin))

    #1,2-addr ...
    chip_bin = chip_bin[2:]

    # Consistent capitalization
    chip_bin = chip_bin.upper()
    hexencoded = hexencoded.upper()

    for b in range(len(hexencoded)):
        try:
            if hexencoded[b] != chip_bin[b]:
                logger.warn("ERR: Mismatch at half-byte" + str(b))
                logger.warn("Expected:" + hexencoded[b])
                logger.warn("Got:" + chip_bin[b])
                return False
        except IndexError:
            logger.warn("ERR: Length mismatch")
            logger.warn("Expected %d bytes" % (len(hexencoded)/2))
            logger.warn("Got %d bytes" % (len(chip_bin)/2))
            logger.warn("All prior bytes validated correctly")
            return False

    logger.info("Programming validated successfully")
    return True

write_bin_via_ein(ice, hexencoded, run_after)

logger.info("")
logger.info("Programming complete.")
logger.info("")

resp = raw_input("Would you like to read back the program via I2C to validate? [Y/n] ")
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
