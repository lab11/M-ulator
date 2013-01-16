#!/usr/bin/env python

from time import sleep
import socket
import sys
import os
import time
import argparse
import logging
logging.basicConfig(level=logging.DEBUG)

from ice import ICE
ice = ICE()

parser = argparse.ArgumentParser()
parser.description = "Helper script to set ICE power rails"

parser.add_argument("SERIAL", help="Path to ICE serial device")
parser.add_argument("-1", "--V1P2",  help="Set 1.2V rail val (0=off)", type=float)
parser.add_argument("-0", "--V0P6",  help="Set 0.6V rail val (0=off)", type=float)
parser.add_argument("-b", "--VBATT", help="Set VBatt rail val (0=off)", type=float)

args = parser.parse_args()

ice.connect(args.SERIAL)

if args.V1P2 is not None:
    if args.V1P2 == 0:
        logging.info("Turning off 1.2V")
        ice.power_set_onoff(ICE.POWER_1P2, False)
    else:
        logging.info("Turning on 1.2V to " + str(args.V1P2))
        ice.power_set_voltage(ICE.POWER_1P2, args.V1P2)
        ice.power_set_onoff(ICE.POWER_1P2, True)

if args.V0P6 is not None:
    if args.V0P6 == 0:
        logging.info("Turning off 0.6V")
        ice.power_set_onoff(ICE.POWER_0P6, False)
    else:
        logging.info("Turning on 0.6V to " + str(args.V0P6))
        ice.power_set_voltage(ICE.POWER_0P6, args.V0P6)
        ice.power_set_onoff(ICE.POWER_0P6, True)

if args.VBATT is not None:
    if args.VBATT == 0:
        logging.info("Turning off VBatt")
        ice.power_set_onoff(ICE.POWER_VBATT, False)
    else:
        logging.info("Turning on VBatt to " + str(args.VBATT))
        ice.power_set_voltage(ICE.POWER_VBATT, args.VBATT)
        ice.power_set_onoff(ICE.POWER_VBATT, True)

