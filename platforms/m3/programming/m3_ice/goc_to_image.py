#!/usr/bin/env python2

from __future__ import print_function

import argparse
import csv
from pprint import pprint
import os
import struct
import sys
import time

import threading
import Queue

import numpy as np

import pygame
import pygame.fastevent
from pygame.locals import *

try:
    import colorama
    print_bold = lambda x: print(colorama.Style.BRIGHT + x + colorama.Style.RESET_ALL)
except ImportError:
    print_bold = lambda x: print(x)

import m3
from m3 import m3_common
from m3 import m3_logging
logger = m3_logging.get_logger(__name__)

rotate_90 = True

################################################################################

parser = argparse.ArgumentParser()

input_group = parser.add_mutually_exclusive_group(required=True)
input_group.add_argument('-f', '--file', default=None,
		help="Read image data from a file saved from a Saleae capture")
input_group.add_argument('-s', '--serial', default=None,
		help="Snoop image data via an ICE board on this serial port")
input_group.add_argument('-w', '--wireless-file', default=None,
		help="File with image data formatted from a wireless capture")

parser.add_argument('-b', '--batch', default=None, action="store_true",
		help="Batch process a file and do not wait for user input")
parser.add_argument('-a', '--address',
		type=lambda x: int(x, 0),
		default=(0x17,),
		nargs='+',
		help="MBus address(es) image data is sent to")

parser.add_argument('-o', '--output-directory',
		default='goc-img-'+time.strftime('%Y-%m-%d--%H-%M-%S'),
		help="Save images into this directory")

parser.add_argument('-p', '--pixels', type=int, default=160,
		help="Number of pixels in a single row of the image")
parser.add_argument('-m', '--multiplier', type=float, default=1.0,
		help="Multiple each raw pixel value by this amount")
parser.add_argument('-l', '--scale', type=int, default=4,
		help="Multiplier to scale the image")

parser.add_argument('-H', '--hot-pixel-map', default=None,
		help="Dark image with a few 'hot' pixels to remove and average with neighbors")
parser.add_argument('--hot-pixel-threshold', type=int, default=25,
		help="Pixels at or above this value will be considered hot")

args = parser.parse_args()

os.mkdir(args.output_directory)

################################################################################

hot_pixel_map = np.zeros((args.pixels,args.pixels), bool)
hot_pixel_list = []
if args.hot_pixel_map:
	for i,row in enumerate(csv.reader(open(args.hot_pixel_map))):
		for j,pixel in enumerate(map(int, row)):
			if pixel >= args.hot_pixel_threshold:
				hot_pixel_map[i, j] = True
				hot_pixel_list.append((i,j))



################################################################################

pygame.init()

option_divider_height = 2 * args.scale
option_text_height = 20 * args.scale

window_width = args.pixels * args.scale
window_height = args.pixels  * args.scale + option_divider_height + option_text_height
windowSurfaceObj = pygame.display.set_mode((window_width,window_height))
pygame.display.set_caption('GOC Test')

gocSurfaceObj = windowSurfaceObj.subsurface((0, 0, args.pixels * args.scale, args.pixels * args.scale))

RED   = pygame.Color(255,   0,   0)
GREEN = pygame.Color(  0, 255,   0)
BLUE  = pygame.Color(  0,   0, 255)
WHITE = pygame.Color(255, 255, 255)
BLACK = pygame.Color(  0,   0,   0)

colors = [pygame.Color(x,x,x) for x in xrange(256)]
colors.insert(0, RED)

# http://stackoverflow.com/questions/17202232/even-with-pygame-initialized-video-system-not-initialized-gets-thrown
# possible cygwin fix?
pygame.fastevent.init()

class Option(object):
    hovered = False

    def __init__(self, text, pos):
        self.text = text
        self.pos = pos
        self.set_rect()
        self.draw()

    def __setattr__(self, name, value):
        if name == 'hovered':
            pygame.display.update(self.rect)
        object.__setattr__(self, name, value)

    def draw(self):
        self.set_rend()
        windowSurfaceObj.blit(self.rend, self.rect)

    def set_rend(self):
        self.rend = options_font.render(self.text, True, self.get_color())
        #self.rend = options_font.render(self.text, False, self.get_color())

    def get_color(self):
        if self.hovered:
            return (255, 255, 255)
        else:
            return (100, 100, 100)

    def set_rect(self):
        self.set_rend()
        self.rect = self.rend.get_rect()
        self.rect.topleft = self.pos

    def onClick(self):
        try:
            self.on_click()
        except AttributeError:
            pass

options_font = pygame.font.Font(None, option_text_height)
options = {
        'prev' : Option("PREV", (10*args.scale + -10*args.scale, args.pixels*args.scale + option_divider_height)),
        'save' : Option("SAVE", (10*args.scale +  50*args.scale, args.pixels*args.scale + option_divider_height)),
        'next' : Option("NEXT", (10*args.scale + 110*args.scale, args.pixels*args.scale + option_divider_height)),
        }

options_divider = pygame.Rect(0, args.pixels*args.scale, window_width, option_divider_height)
windowSurfaceObj.fill(BLUE, options_divider)
################################################################################


def get_addr17_msg_file():
    data = []
    searching = True
    for line in open(args.file):
        line = line.strip()
        if searching:
            if line in map(lambda a: 'Address {:x}'.format(a), args.address):
                searching = False
            continue
        if line.split()[0] == 'Data':
            data.append(int(line.split()[1], 16))
            continue
        yield data
        data = []
        if line not in map(lambda a: 'Address {:x}'.format(a), args.address):
            searching = True


# Generators are expected to send one row at a time, but the wireless file is
# formatted as rows of (possibly missing) 12 bit entires, so we throw another
# generator layer here. The first always returns the "next" 12 bits (either
# padded from missing packets or from the file), the next level then returns
# those as rows
# to work out the padding correctly for, so we grab a whole image

PIXELS_PER_PACKET = 12
EXPECTED_NUMBER_OF_PACKETS = (args.pixels * args.pixels) / PIXELS_PER_PACKET
EXPECTED_LAST_PACKET_NUMBER = EXPECTED_NUMBER_OF_PACKETS - 1

def get_12bytes_from_wireless_file():
    START_OF_IMAGE = "004000003000002000001ABC000"
    END_OF_IMAGE = "004000003000002000001ABCFFF"

    prior_packet_number = -1
    for line in open(args.wireless_file):
        line = line.strip()
        packet_number = int(line[:3], 16)

        # New Image?
        #if packet_number < prior_packet_number:
        if line == START_OF_IMAGE:
            print_bold("Found START_OF_IMAGE")
            # Now reset the prior_packet_number to start of next frame
            prior_packet_number = -1
            yield 'START'
            continue

        # End of Image?
        if line == END_OF_IMAGE:
            print_bold("Found END_OF_IMAGE")
            # Now reset the prior_packet_number to start of next frame
            prior_packet_number = -1
            yield 'END'
            continue

        # Garbage packet?
        # This a bit of a tricky thing to estimate, but we make the assumption
        # here that not _too_ many packets were dropped.
        if abs(prior_packet_number - packet_number) > 10:
            print('Warn: Dropping packet "number" {}, the last packet was {}'.\
                    format(packet_number, prior_packet_number))
            print('      and this is likely a corrupted packet')
            continue

        # Drop any packets?
        if (prior_packet_number + 1) != packet_number:
            print("Warn: Dropped packet (prior packet number: {}, current: {})".\
                    format(prior_packet_number, packet_number))
            print("      Padding missed pixels with red")

            while (prior_packet_number + 1) != packet_number:
                yield (-1,) * 12
                prior_packet_number += 1

        # Parse this packet
        data = line[3:]
        if len(data) != 2*PIXELS_PER_PACKET:
            print("Warn: Expected {} bytes in packet, got {}".\
                    format(PIXELS_PER_PACKET, len(data)))
        row_data = []
        for b in range(0, len(data), 2):
            row_data.append(int(data[b:b+2], 16))
        # Each packet is 3 words, the words are in the reverse order, fix:
        rd = [
                row_data[8], row_data[9], row_data[10], row_data[11],
                row_data[4], row_data[5], row_data[6], row_data[7],
                row_data[0], row_data[1], row_data[2], row_data[3],
                ]
        yield rd

        prior_packet_number = packet_number

def get_individual_bytes_from_wireless_file():
    EXPECTED_PIXEL_COUNT = args.pixels * args.pixels

    packet_gen = get_12bytes_from_wireless_file()
    pixel_count = -1
    while True:
        packet = packet_gen.next()
        if packet == 'START':
            if pixel_count != -1:
                print("Warn: Unexpected START_OF_IMAGE - images may be wrong after this")
        elif packet == 'END':
            if pixel_count < EXPECTED_PIXEL_COUNT:
                print("Warn: Not enough pixels. Padding end of image")
            while pixel_count < EXPECTED_PIXEL_COUNT:
                pixel_count += 1
                yield -1
            pixel_count = -1
        else:
            for byte in packet:
                pixel_count += 1
                if pixel_count >= EXPECTED_PIXEL_COUNT:
                    break
                yield byte

def get_addr17_msg_wireless_file():
    bytes_gen = get_individual_bytes_from_wireless_file()

    while True:
        # Grab one image's worth of rows (generator assures sync to image
        # boundaries)
        for rows in range(args.pixels):
            row = []
            while len(row) < args.pixels:
                row.append(bytes_gen.next())
            yield row

        # Generate an end-of-image message
        # ~0x0000CFFF
        eoi = (0xff, 0xff, 0x30, 0x00)
        yield eoi


serial_queue = Queue.Queue()
def get_addr17_msg_serial():
    while True:
        addr, data = serial_queue.get()
        addr = int(addr.encode('hex'), 16)
        if addr in args.address:
            data = list(map(ord, data))
            yield data
        else:
            logger.debug("bad addr {:x}".format(addr))

def Bpp_callback(address, data):
    serial_queue.put((address, data))

class preparsed_snooper(m3_common.mbus_snooper):
    def __init__(self, args, *pyargs, **kwargs):
        self.args = args
        super(preparsed_snooper,self).__init__(*pyargs, **kwargs)

    def parse_args(self):
        self.serial_path = self.args.serial

if args.file:
    get_addr17_msg = get_addr17_msg_file
elif args.wireless_file:
    get_addr17_msg = get_addr17_msg_wireless_file
else:
    snooper = preparsed_snooper(args=args, callback=Bpp_callback)
    get_addr17_msg = get_addr17_msg_serial

def is_end_of_image_msg(m):
    if len(m) == 4:
        data = struct.unpack(">I", struct.pack("BBBB", *m))[0]
        data = data & 0x0000CFFF
        if data == 0:
            return True
    return False

# Decent guess for now
def is_motion_detect_msg(m):
    if len(m) == 4:
        if not is_end_of_image_msg(m):
            return True

def get_image_g(data_generator):
    class UnexpectedAddressException(Exception):
        pass

    while True:
        data = np.zeros((args.pixels,args.pixels), int)
        end_of_image = False

        # Grab an image
        for row in xrange(args.pixels):
            r = data_generator.next()
            while is_motion_detect_msg(r):
                print("Skipping motion detect message")
                r = data_generator.next()
            if is_end_of_image_msg(r):
                print("Unexpected end-of-image. Expecting row", row + 1)
                print("Returning partial image")
                end_of_image = True
                break
            if len(r) != args.pixels:
                print("Row %d message incorrect length: %d" % (row, len(r)))
                print("Using first %d pixel(s)" % (min(len(r), args.pixels)))
            for p in xrange(min(len(r), args.pixels)):
                data[row][p] = r[p] + 1

        while not end_of_image:
            m = data_generator.next()
            if is_end_of_image_msg(m):
                break
            print("Expected end-of-image. Got message of length:", len(m))

            # If imager sends more rows than expected, discard this earliest
            # received rows. Works around wakeup bug.
            data = np.roll(data, -1, axis=0)
            for p in xrange(min(len(m), args.pixels)):
                data[-1][p] = m[p] + 1
            if len(m) != args.pixels:
                print("Extra row message incorrect length: %d" % (len(m)))
                print("Zeroing remaining pixels")
                for p in xrange(len(m), args.pixels):
                    data[-1][p] = 0

        yield data

def correct_endianish_thing_old(data, array):
    for row in xrange(args.pixels):
        for colset in xrange(0, args.pixels, 4):
            for i in xrange(4):
                if rotate_90:
                    val = data[colset+3-i][row]
                else:
                    val = data[row][colset+3-i]
                val = int(val)
                if (val):
                    val -= 1
                    color = pygame.Color(val, val, val)
                    rgb = gocSurfaceObj.map_rgb(color)
                else:
                    rgb = gocSurfaceObj.map_rgb(RED)
                array[row][colset+i] = rgb

def correct_endianish_thing(data, array):
    for rowbase in xrange(0, args.pixels, 4):
        for rowi in xrange(4):
            for col in xrange(0, args.pixels):
                if rotate_90:
                    val = data[col][rowbase + 3-rowi]
                else:
                    val = data[rowbase + 3-rowi][col]
                val = int(val)
                val = int(val * args.multiplier)
                if (val):
                    val -= 1
                    if val > 255:
                        logger.warn("Pixel value > 255, capping at 255")
                        val = 255
                    color = pygame.Color(val, val, val)
                    rgb = gocSurfaceObj.map_rgb(color)
                else:
                    rgb = gocSurfaceObj.map_rgb(RED)
                array[rowbase+rowi][col] = rgb


images_q = Queue.Queue()


def process_hot_pixels(img):
	img = img.copy()
	if args.hot_pixel_map is None:
		return img
	ret = []
	for hp in hot_pixel_list:
		neighbors = []
		for i in (-1,0,1):
			for j in (-1, 0, 1):
				if i == j == 0:
					continue
				try:
					neighbors.append(img[hp[0]+i,hp[1]+j])
				except IndexError:
					pass
		img[hp[0], hp[1]] = np.mean(neighbors)
	return img


def get_images():
	if args.file:
		print("Processing static file")
		images_g = get_image_g(get_addr17_msg_file())
		for img in images_g:
			hot = process_hot_pixels(img)
			images_q.put((img, hot))
		print("Done reading file")
		event = pygame.event.Event(pygame.USEREVENT)
		pygame.fastevent.post(event)
	elif args.wireless_file:
		print("Processing wireless file")
		images_g = get_image_g(get_addr17_msg_wireless_file())
		for img in images_g:
			hot = process_hot_pixels(img)
			images_q.put((img, hot))
		print("Done reading file")
		event = pygame.event.Event(pygame.USEREVENT)
		pygame.fastevent.post(event)
	elif args.serial:
		images_g = get_image_g(get_addr17_msg_serial())
		for img in images_g:
			hot = process_hot_pixels(img)
			images_q.put((img, hot))
			event = pygame.event.Event(pygame.USEREVENT)
			pygame.fastevent.post(event)
		print("ERR: Should never get here [serial image_g terminated]")

get_images_thread = threading.Thread(target=get_images)
get_images_thread.daemon = True
get_images_thread.start()

images = []
images_raw = []
def get_image_idx(idx):
	global images
	global images_raw
	while True:
		try:
			raw,hot = images_q.get_nowait()
			images_raw.append(raw)
			images.append(hot)
		except Queue.Empty:
			break
	return images[idx]

################################################################################

pygame.event.set_allowed(None)
pygame.event.set_allowed((QUIT, KEYUP, MOUSEBUTTONUP, MOUSEMOTION))

goc_raw_Surface = pygame.Surface((args.pixels, args.pixels))

def render_raw_goc_data(data):
    print("Request to render raw goc data")
    print("Correcting pixel order bug")
    surface_array = pygame.surfarray.pixels2d(goc_raw_Surface)
    correct_endianish_thing(data, surface_array)
    del surface_array
    print("Scaling %d pixel --> %d pixel%s" % (1, args.scale, ('s','')[args.scale == 1]))
    pygame.transform.scale(goc_raw_Surface, (args.pixels*args.scale, args.pixels*args.scale), gocSurfaceObj)
    print("Rendering Image")
    pygame.display.update()
    print()

def render_image_idx(idx):
    print("Request to render image", idx)
    render_raw_goc_data(get_image_idx(idx))

def save_image(filename):
    pygame.image.save(gocSurfaceObj, filename)
    print('Image saved to', filename)

def save_image_hack():
	imgname = "capture%02d.jpeg" % (current_idx)
	imgname = os.path.join(args.output_directory, imgname)
	save_image(imgname)

	csvname = "capture%02d.csv" % (current_idx)
	csvname = os.path.join(args.output_directory, csvname)
	ofile = csv.writer(open(csvname, 'w'), dialect='excel')
	ofile.writerows(get_image_idx(current_idx))

	if args.hot_pixel_map:
		raw_csvname = "raw_capture%02d.csv" % (current_idx)
		raw_csvname = os.path.join(args.output_directory, raw_csvname)
		raw_ofile = csv.writer(open(raw_csvname, 'w'), dialect='excel')
		raw_ofile.writerows(images_raw[current_idx])
	print('CSV of image saved to', csvname)
options['save'].on_click = save_image_hack

def advance_image(bubble_index_error=False):
    global current_idx
    current_idx += 1
    try:
        render_image_idx(current_idx)
        save_image_hack()
    except IndexError:
        if bubble_index_error:
            raise
        current_idx -= 1
        print("At last image. Display left at image", current_idx)
        print()
options['next'].on_click = advance_image

def rewind_image():
    global current_idx
    current_idx = max(0, current_idx-1)
    render_image_idx(current_idx)
options['prev'].on_click = rewind_image

def quit():
    pygame.quit()
    sys.exit()

current_idx = -1
advance_image()


if args.batch:
    # Don't go into the event loop, instead just process until you can't
    # HACK: Need the file reading process to get started
    time.sleep(1)
    try:
        while True:
            advance_image(bubble_index_error=True)
    except IndexError:
        print("Last image reached. Exiting.")
        sys.exit()

while True:
    event = pygame.event.wait()

    if event.type == QUIT:
        quit()

    if event.type == MOUSEBUTTONUP:
        for option in options.values():
            if option.rect.collidepoint(pygame.mouse.get_pos()):
                option.onClick()

    if event.type == MOUSEMOTION:
        for option in options.values():
            if option.rect.collidepoint(pygame.mouse.get_pos()):
                option.hovered = True
            else:
                option.hovered = False
            option.draw()

    if event.type == KEYUP:
        if event.key == K_RIGHT:
            advance_image()
        elif event.key == K_LEFT:
            rewind_image()
        elif event.key == K_RETURN:
            save_image_hack()
        elif event.key == K_ESCAPE:
            quit()

    if event.type == USEREVENT:
        advance_image()

