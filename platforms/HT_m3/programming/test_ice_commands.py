#!/usr/bin/env python

import sys
import struct
import socket
import logging
logging.basicConfig(level=logging.INFO, format="%(message)s")
logger = logging.getLogger('program')

import inspect

from ice import ICE

class ICETests(object):
    class TestFailedException(Exception):
        pass

    def test_query_capabilities(self, ice):
        logger.info("Test ??")
        caps = ice.ice_query_capabilities()
        logger.debug("ICE Capability String: " + caps)

    def test_baudrate(self, ice):
        if sys.platform.lower() == 'darwin':
            logger.warn("test_baudrate skipped on OS X")
            return
        logger.info("Test ?b")
        ice.ice_set_baudrate_to_3_megabaud()
        baud = ice.ice_get_baudrate()
        if baud != 3000000:
            logger.error("Set/get baudrate mismatch")
            logger.error("Expected 3000000, got " + str(baud))
            raise self.TestFailedException

    def test_discrete_i2c(self, ice):
        logger.info("Test d")
        ret = ice.i2c_send(0xa5, "12345678".decode('hex'))
        if ret != 5:
            logger.error("Failed to send whole short I2C message")
            logger.info("  Did you set fake_ice to ACK all addresses?")
            logger.info("  That is: ./fake_ice.py /tmp/com2 xxxxxxxx")
        ret = ice.i2c_send(0x69, ("ab"*511).decode('hex'))
        if ret != (1+511):
            logger.error("Failed to send whole long I2C message")

    def test_i2c_speed(self, ice):
        logger.info("Test ic")
        TARGET_SPEED = 80
        ice.i2c_set_speed(TARGET_SPEED)
        speed = ice.i2c_get_speed()
        if speed != TARGET_SPEED:
            logger.error("Set/Get speed mismatch")

    def test_i2c_address(self, ice):
        logger.info("Test ia")
        TARGET_ADDRESS = "1x01010x"
        ice.i2c_set_address(TARGET_ADDRESS)
        address = ice.i2c_get_address()
        if address != TARGET_ADDRESS:
            logger.error("Set/get mismatch i2c address")
            logger.error("Expected: " + TARGET_ADDRESS + "  Got: " + address)

    def test_goc(self, ice):
        logger.info("Test f")
        ret = ice.goc_send("a5".decode('hex'), show_progress=False)
        if ret != 1:
            logger.error("Failed to send whole short GOC message")
        ret = ice.goc_send(("96"+"ba"*511).decode('hex'), show_progress=False)
        if ret != (1+511):
            logger.error("Failed to send whole long GOC message")

    def test_goc_speed(self, ice):
        logger.info("Test oc")
        TARGET_FREQ = 12
        ice.goc_set_frequency(TARGET_FREQ)
        freq = ice.goc_get_frequency()
        if freq != TARGET_FREQ:
            logger.error("Set/get mismatch on GOC frequency")

    def test_goc_onoff(self, ice):
        logger.info("Test oo")
        ice.goc_set_onoff(True)
        onoff = ice.goc_get_onoff()
        if onoff != True:
            logger.error("Set/get mismatch on GOC onoff")

    def test_mbus_message(self, ice):
        logger.info("Test b")
        ret = ice.mbus_send("5a".decode('hex'), "87654321".decode('hex'))
        # ret value from addr is always 4
        if ret != 8:
            logger.error("Failed to send whole short MBus message")
        ret = ice.mbus_send("69".decode('hex'), ("ab"*511).decode('hex'))
        if ret != (4+511):
            logger.error("Failed to send whole long MBus message")

    def test_mbus_full_prefix(self, ice):
        logger.info("Test ml")
        TARGET_FULL_PREFIX = "10x0x0x11xx00xx1110x"
        ice.mbus_set_full_prefix(TARGET_FULL_PREFIX)
        fp = ice.mbus_get_full_prefix()
        if fp != TARGET_FULL_PREFIX:
            logger.error("Set/get mismatch mbus full prefix")

    def test_mbus_short_prefix(self, ice):
        logger.info("Test ms")
        TARGET_SHORT_PREFIX = "1x0x"
        ice.mbus_set_short_prefix(TARGET_SHORT_PREFIX)
        sp = ice.mbus_get_short_prefix()
        if sp != TARGET_SHORT_PREFIX:
            logger.error("Set/get mismatch mbus short prefix")

    def test_mbus_full_snoop_prefix(self, ice):
        logger.info("Test mL")
        TARGET_FULL_SNOOP_PREFIX = "10xx0x110x1x01100x1x"
        ice.mbus_set_full_snoop_prefix(TARGET_FULL_SNOOP_PREFIX)
        fsp = ice.mbus_get_full_snoop_prefix()
        if fsp != TARGET_FULL_SNOOP_PREFIX:
            logger.error("Set/get mismatch mbus full snoop prefix")

    def test_mbus_short_snoop_prefix(self, ice):
        logger.info("Test mS")
        TARGET_SHORT_SNOOP_PREFIX = "xx01"
        ice.mbus_set_short_snoop_prefix(TARGET_SHORT_SNOOP_PREFIX)
        ssp = ice.mbus_get_short_snoop_prefix()
        if ssp != TARGET_SHORT_SNOOP_PREFIX:
            logger.error("Set/get mismatch mbus short snoop prefix")

    def test_mbus_broadcast_mask(self, ice):
        logger.info("Test mb")
        TARGET_BROADCAST_PREFIX = "1xx0"
        ice.mbus_set_broadcast_channel_mask(TARGET_BROADCAST_PREFIX)
        bp = ice.mbus_get_broadcast_channel_mask()
        if bp != TARGET_BROADCAST_PREFIX:
            logger.error("Set/get mismatch mbus broadcast mask")

    def test_mbus_broacast_snoop_mask(self, ice):
        logger.info("Test mB")
        TARGET_BROADCAST_SNOOP_PREFIX = "x01x"
        ice.mbus_set_broadcast_channel_snoop_mask(TARGET_BROADCAST_SNOOP_PREFIX)
        bsp = ice.mbus_get_broadcast_channel_snoop_mask()
        if bsp != TARGET_BROADCAST_SNOOP_PREFIX:
            logger.error("Set/get mismatch mbus broadcast mask")

    def test_mbus_master_onoff(self, ice):
        logger.info("Test mm")
        ice.mbus_set_master_onoff(True)
        master = ice.mbus_get_master_onoff()
        if master != True:
            logger.error("Set/get mismatch mbus master mode")

    def test_mbus_clock_speed(self, ice):
        logger.info("Test mc")
        logger.warning("MBus clock setting not implemented -- skipping")
        return
        TARGET_FREQ = 600
        ice.mbus_set_clock(TARGET_FREQ)
        clock = ice.mbus_get_clock()
        if clock != TARGET_FREQ:
            logger.error("Set/get mismatch mbus clock")

    def test_mbus_should_interrupt(self, ice):
        logger.info("Test mi")

        ice.mbus_set_should_interrupt(1)
        i = ice.mbus_get_should_interrupt()
        if i != 1:
            logger.error("Set/get mismatch mbus should int (1)")
            logger.error("Expected 1  Got " + str(i))
        ice.mbus_send("ec".decode('hex'), "beef".decode('hex'))
        i = ice.mbus_get_should_interrupt()
        if i != 0:
            logger.error("Should interrupt clear failed")

        ice.mbus_set_should_interrupt(2)
        i = ice.mbus_get_should_interrupt()
        if i != 2:
            logger.error("Set/get mismatch mbus should int (2)")
        ice.mbus_send("ec".decode('hex'), "beef".decode('hex'))
        i = ice.mbus_get_should_interrupt()
        if i != 2:
            logger.error("Should interrupt persistance failed")

    def test_mbus_use_priority(self, ice):
        logger.info("Test mp")

        ice.mbus_set_use_priority(1)
        i = ice.mbus_get_use_priority()
        if i != 1:
            logger.error("Set/get mismatch mbus should int (1)")
        ice.mbus_send("db".decode('hex'), "bead".decode('hex'))
        i = ice.mbus_get_use_priority()
        if i != 0:
            logger.error("Should use_priority clear failed")

        ice.mbus_set_use_priority(2)
        i = ice.mbus_get_use_priority()
        if i != 2:
            logger.error("Set/get mismatch mbus should int (2)")
        ice.mbus_send("ec".decode('hex'), "beef".decode('hex'))
        i = ice.mbus_get_use_priority()
        if i != 2:
            logger.error("Should use_priority persistance failed")

    def test_ein(self, ice):
        logger.info("Test e")
        ret = ice.ein_send("a5".decode('hex'))
        if ret != 1:
            logger.error("Failed to send whole short EIN message")
        ret = ice.ein_send(("96"+"ba"*511).decode('hex'))
        if ret != (1+511):
            logger.error("Failed to send whole long EIN message")

    def test_gpio_level(self, ice):
        logger.info("Test gl")
        ice.gpio_set_level(3, True)
        if ice.gpio_get_level(3) != True:
            logger.error("Set/get mismatch gpio 3")
        ice.gpio_set_level(6, False)
        if ice.gpio_get_level(6) != False:
            logger.error("Set/get mismatch gpio 6")

    def test_gpio_direction(self, ice):
        logger.info("Test gd")
        ice.gpio_set_direction(1, ice.GPIO_INPUT)
        if ice.gpio_get_direction(1) != ice.GPIO_INPUT:
            logger.error("Set/get mismatch gpio 1")
        ice.gpio_set_direction(5, ice.GPIO_OUTPUT)
        if ice.gpio_get_direction(5) != ice.GPIO_OUTPUT:
            logger.error("Set/get mismatch gpio 5")
        ice.gpio_set_direction(1, ice.GPIO_TRISTATE)
        if ice.gpio_get_direction(1) != ice.GPIO_TRISTATE:
            logger.error("Set/get mismatch gpio 1 (to tri)")

    def test_gpio_interrupt_mask(self, ice):
        logger.info("Test gi")
        TARGET_GPIO_INT_MASK = 0xa53
        ice.gpio_set_interrupt_enable_mask(TARGET_GPIO_INT_MASK)
        if ice.gpio_get_interrupt_enable_mask != TARGET_GPIO_INT_MASK:
            logger.error("Set/get mismatch gpio interrupt mask")

    def test_voltage_state(self, ice):
        logger.info("Test pv")
        ice.power_set_voltage(ice.POWER_0P6, 0.6)
        p = ice.power_get_voltage(ice.POWER_0P6)
        # power rail settings aren't round, resp is actual value, so check if
        # it's within ~2% of what we asked for (2% b/c 3.7 -> 1.1% error)
        if (abs(0.6 - p) / 0.6) > 0.02:
            logger.error("Set/get mismatch 0.6 V rail")
            logger.error("Expected 0.6  Got " + str(p))
        ice.power_set_voltage(ice.POWER_1P2, 1.2)
        p = ice.power_get_voltage(ice.POWER_1P2)
        if (abs(1.2 - p) / 1.2) > 0.02:
            logger.error("Set/get mismatch 1.2 V rail")
            logger.error("Expected 1.2  Got " + str(p))
        ice.power_set_voltage(ice.POWER_VBATT, 3.7)
        p = ice.power_get_voltage(ice.POWER_VBATT)
        if (abs(3.7 - p) / 3.7) > 0.02:
            logger.error("Set/get mismatch VBATT rail")
            logger.error("Expected 3.7  Got " + str(p))

    def test_power_onoff(self, ice):
        logger.info("Test po")
        ice.power_set_onoff(ice.POWER_VBATT, True)
        if ice.power_get_onoff(ice.POWER_VBATT) != True:
            logger.error("Set/get mismatch VBATT power")
        ice.power_set_onoff(ice.POWER_1P2, True)
        if ice.power_get_onoff(ice.POWER_1P2) != True:
            logger.error("Set/get mismatch 1.2 V power")
        ice.power_set_onoff(ice.POWER_0P6, True)
        if ice.power_get_onoff(ice.POWER_0P6) != True:
            logger.error("Set/get mismatch 0.6 V power")



ice = ICE()

if len(sys.argv) not in (2,):
	logger.info("USAGE: %s SERIAL_DEVICE\n" % (sys.argv[0]))
	sys.exit(2)

ice.connect(sys.argv[1])

logger.info('')
logger.info('Begin running tests')
try:
    logger.info('  Attached ICE supports: %s', ice.capabilities)
except AttributeError:
    logger.info('  Attached ICE does not support querying capabilities')
logger.info('')

i = ICETests()
all_functions = inspect.getmembers(i, inspect.ismethod)
for f in all_functions:
    if f[0][0:5] == "test_":
        try:
            f[1](ice)
        except ice.VersionError:
            logger.info("%s skipped. Not supported in attached ICE version", f[0])
        except ice.CapabilityError as e:
            logger.info("%s skipped. Required capability %s is not supported",
                    f[0], e.required_capability)
    else:
        logger.warn("Non-test method: " + f[0])

logger.info('')
logger.info("All tests completed")
