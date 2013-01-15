#!/usr/bin/env python

print "-" * 80
print "-- M3 GOC Programmer"
print

from time import sleep
import socket
import sys
import os
import time
import mimetypes
import Queue

from ice import ICE
ice = ICE()

if len(sys.argv) not in (3,):
    print "USAGE: %s BINFILE SERAIL_DEVICE\n" % (sys.argv[0])
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


# Callback for async I2C message
def validate_bin_helper(msg_type, event_id, length, msg):
    validate_q.put(msg)

validate_q = Queue.Queue()
ice.msg_handler['d+'] = validate_bin_helper

ice.connect(sys.argv[2])

print "Turning all M3 power rails on"
ice.power_set_voltage(0,0.6)
ice.power_set_voltage(1,1.2)
ice.power_set_voltage(2,3.8)
ice.power_set_onoff(0,True)
ice.power_set_onoff(1,True)
ice.power_set_onoff(2,True)
sleep(1.0)

print "M3 0.6V => OFF (reset controller)"
ice.power_set_onoff(0,False)
sleep(1.0)
print "M3 0.6V => ON"
ice.power_set_onoff(0,True)
sleep(1.0)

print "Would you like to run after programming? If you do not"
print "have GOC start the program, you will be prompted to send"
print "the start message via DMA at the end instead"
print
resp = raw_input("Run program when GOC finishes? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    run_after = 0
else:
    run_after = 1

# Set to .625Hz
print "Sending frequency setting to ICE (.625Hz)"
ice.goc_set_frequency(0.625)
print

#def goc_delay(byte_string, div8 = False):
#    # byte_string is cmd + hexencoded bytes (4 bits each) + \r\n, hence the -3 and the *4
#    num_bits = (len(byte_string) - 3) * 4
#    t = num_bits / 5.0 + 1
#    if div8:
#        t *= 8
#        print "Sleeping for %f seconds while it blinks at 1/8th speed..." % (t)
#    else:
#        print "Sleeping for %f seconds while it blinks..." % (t)
#    while (t > 1):
#        sys.stdout.write("\r\t\t\t\t\t\t")
#        sys.stdout.write("\r\t%f remaining..." % (t))
#        sys.stdout.flush()
#        t -= 1
#        time.sleep(1)
#    time.sleep(t)

def write_bin_via_goc(ice, hexencoded, run_after):
    passcode_string = "7394"
    print "Sending passcode to GOC"
    print "Sending:", passcode_string
    ice.goc_send(passcode_string.decode('hex'))
#goc_delay(passcode_string, True)
    print

    # Up ICE sending frequency to 5Hz
    print "Sending 8x frequency setting to ICE (5Hz)"
    ice.goc_set_frequency(5)
    print

    # Build the GOC message:
    chip_id_mask = 0		# [0:3] Chip ID Mask
    reset = 0			#   [4] Reset Request
    chip_id_coding = 0		#   [5] Chip ID coding
    is_i2c = 0			#   [6] Indicates transmission is I2C message [addr+data]
    run_after = not not run_after	#   [7] Run code after programming?
    # Byte 0: Control
    control = chip_id_mask | (reset << 4) | (chip_id_coding << 5) | (is_i2c << 6) | (run_after << 7)

    # Byte 1,2: Chip ID
    chip_id = 0

    # Byte 3,4: Memory Address
    mem_addr = 0

    # Byte 5,6: Program Lengh
    length = len(hexencoded) >> 3	# hex exapnded -> bytes, /2; word count /4

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

    print "Sending program to GOC"
    print "Sending:", message
    ice.goc_send(message.decode('hex'))
# goc_delay(message)
    print

    print "Sending extra blink to end transaction"
    extra = "80"
    print "Sending:", extra
    ice.goc_send(extra.decode('hex'))
#    goc_delay(extra)
    print

def validate_bin(ice, hexencoded, offset=0):
    ice.i2c_set_address("1001100x") # 0x98

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

write_bin_via_goc(ice, hexencoded, run_after)

print "Programming complete."
print

resp = raw_input("Would you like to read back the program via I2C to validate? [Y/n] ")
if not (len(resp) != 0 and resp[0] in ('n', 'N')):
    if validate_bin(ice, hexencoded) is False:
        print "Validation failed. Dying"
        sys.exit()

resp = raw_input("Would you like to send the DMA start interrupt? [Y/n] ")
if len(resp) != 0 and resp[0] in ('n', 'N'):
    sys.exit()

print "Sending 0x88 0x00000000"
ice.i2c_send(0x88, "00000000".decode('hex'))

