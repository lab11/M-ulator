#!/usr/bin/env python
#
# self-test: socat PTY,link=com1 PTY,link=com2

print "-" * 80
print "-- M3 Programmer"
print

from math import ceil
from time import sleep
import socket
import sys
import os
import mimetypes
import Queue

from ice import ICE
ice = ICE()


if len(sys.argv) not in (3,):
    print "USAGE: %s BINFILE SERIAL_DEVICE\n" % (sys.argv[0])
    sys.exit(2)

binfile = sys.argv[1]

if mimetypes.guess_type(binfile)[0] == 'text/plain':
    print "Guessing hex-encoded stream for NI setup"
    print "  ** This means one byte (two hex characters) per line"
    print "  ** and these are the first two characters on each line."
    print "  ** If it needs to parse something more complex, let me know."
    binfd = open(binfile, 'r')
    hexencoded = ""
    for line in binfd:
        hexencoded += line[0:2]
else:
    print "Guessing compiled binary"
    binfd = open(binfile, 'rb')
    hexencoded = binfd.read().encode("hex").upper()

if (len(hexencoded) % 8) != 0:
    print "Binfile is not word-aligned. This is not a valid image"
    sys.exit(3)
else:
    print "Binfile is %d bytes long\n" % (len(hexencoded) / 2)


ice.connect(sys.argv[2])
ice.i2c_set_address("1001100x") # 0x98

resp = raw_input("About to send I2C message to wake controller. Continue? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

junk_dma_done_msg = "%08X" % (socket.htonl(0x20000000))
print "Sending junk message (DMA Done, 0 bytes to addr 0) to ensure chip is awake"
print "Sending: 0xAA", junk_dma_done_msg
ice.i2c_send(0xaa, junk_dma_done_msg.decode('hex'))

def write_bin(ice, hexencoded, offset=0):
    print "Running programming sequence:"
    print "\tI2C message for start DMA write at address 0x%x, length %d" % (offset, len(hexencoded)/2)
    print "\tI2C message for DMA data"
    print "\tI2C message for end of DMA at address 0x%x, length %d" % (offset, len(hexencoded)/2)
    print

    data = 0x40000000 | ((len(hexencoded)/8) << 16) | offset
    dma_write_req = "%08X" % (socket.htonl(data))
    print "Sending:", dma_write_req
    ice.i2c_send(0xaa, dma_write_req.decode('hex'))

    print "Sending data to address 0xA8"
    ice.i2c_send(0xa8, hexencoded.decode('hex'))

    data = 0x20000000 | ((len(hexencoded)/8) << 16) | offset
    dma_done_msg = "%08X" % (socket.htonl(data))
    print "Sending:", dma_done_msg
    ice.i2c_send(0xaa, dma_done_msg.decode('hex'))

def validate_bin_helper(msg_type, event_id, length, msg):
    validate_q.put(msg)

validate_q = Queue.Queue()
ice.msg_handler['d+'] = validate_bin_helper

def validate_bin(ice, hexencoded, offset=0):
    print "Running Validation sequence:"
    print "\tI2C message for start DMA read at address 0x%x, length %d" % (offset, len(hexencoded)/2)
    print "\t<Receive I2C message for DMA data>"
    print "\tCompare received data and validate it was programmed correctly"
    print

    data = 0x80000000 | ((len(hexencoded)/8) << 16) | offset
    dma_read_req = "%08X" % (socket.htonl(data))
    print "Sending:", dma_read_req
    ice.i2c_send(0xaa, dma_read_req.decode('hex'))

    print "Chip Program Dump Response:"
    chip_bin = validate_q.get()
    chip_bin = chip_bin.encode('hex')
    print chip_bin

    #1,2-addr ...
    chip_bin = chip_bin[2:]

    # Consistent capitalization
    chip_bin = chip_bin.upper()
    hexencoded = hexencoded.upper()

    for b in range(2, len(hexencoded)):
        try:
            if hexencoded[b] != chip_bin[b]:
                print "ERR: Mismatch at half-byte", b
                print "Expected:", hexencoded[b]
                print "Got:", chip_bin[b]
                return False
        except IndexError:
            print "ERR: Length mismatch"
            print "Expected %d bytes" % (len(hexencoded)/2)
            print "Got %d bytes" % (len(chip_bin)/2)
            print "All prior bytes validated correctly"
            return False

    print "Programming validated successfully"
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
        print
        print
        print '=' * 80
        print "Maximum number of tries exceeded. Programming Failed."
        sys.exit(-1)

print
print
print '=' * 80
print "Programming complete."
print

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

print "Sending 0x88 0x00000000"
ice.i2c_send(0x88, "00000000".decode('hex'))
