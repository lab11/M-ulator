#!/usr/bin/env python

MAX_GPIO = 24

import serial

s = serial.Serial('/tmp/com2', 115200)
if not s.isOpen():
    raise IOError, "Failed to open serial port"

GPIO_INPUT    = 0
GPIO_OUTPUT   = 1
GPIO_TRISTATE = 2

event = 0
gpio_level = [False for x in xrange(MAX_GPIO)]
gpio_dir   = [GPIO_TRISTATE for x in xrange(MAX_GPIO)]

def respond(msg, ack=True):
    global s
    global event

    if (ack):
        s.write(chr(0))
    else:
        s.write(chr(1))
    s.write(chr(event))
    event += 1
    s.write(chr(len(msg)))
    if len(msg):
        s.write(msg)
    print "Sent a response of length", len(msg)

def ack():
    respond('')

while True:
    msg_type, event_id, length = s.read(3)
    print "Got a message of type", msg_type
    event_id = ord(event_id)
    length = ord(length)
    msg = s.read(length)

    if msg_type == 'V':
        respond('0001'.decode('hex'))
    elif msg_type == 'v':
        if msg == '0001'.decode('hex'):
            ack()
        else:
            print "Unknown version:", msg
            raise Exception
    elif msg_type == 'g':
        if len(msg) != 3:
            print "bad 'g' message length:", len(msg)
            raise Exception
        if msg[0] == 'l':
            gpio_level[ord(msg[1])] = ord(msg[2])
            ack()
        elif msg[0] == 'd':
            gpio_dir[ord(msg[1])] = ord(msg[2])
            ack()
        else:
            print "bad 'g' subtype:", msg[0]
            raise Exception
    else:
        print "Unknown msg type:", msg_type
        raise Exception
