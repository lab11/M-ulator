#!/usr/bin/env python3

import logging
log = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

import glob
import os
import pprint
import sys
import time

# Or write fallback os.walk code for glob ** usage
assert sys.version_info >= (3,5)

import sh
from sh import rm, mv, cp, ls, mkdir, tup, pwd
logging.getLogger("sh").setLevel(logging.WARNING)

BUILD_DIR = 'builds'
PRISTINE = os.path.join(BUILD_DIR, 'simulator')

log.info("Starting CI Build")
log.info(" - Using {} for BUILD_DIR".format(BUILD_DIR))
log.info(" - Using {} for BUILD_DIR".format(BUILD_DIR))

# Start from scratch
log.info(" - Clearing BUILD_DIR")
rm('-r', '-f', BUILD_DIR)
mkdir(BUILD_DIR)


# Grab some system information
log.info(" - System compiller information:\n{}'".format(sh.cc('--version')))


# No fuse in the CI environment means we need a unique directory for each
# variant. We first grab a copy of the simulator folder and wipe out all things
# tup to use as a template so that the same script will work locally and in CI
cp('-r', 'simulator', PRISTINE)
with sh.pushd(PRISTINE):
	rm('-r', '-f', '.tup')
	rm('-r', '-f', sh.glob('build-*'))

variants = {}

for variant_file in os.listdir('simulator/configs'):
	log.info("Building {}".format(variant_file))

	variant = os.path.basename(variant_file)

	build_variant_dir = os.path.join(BUILD_DIR, variant)
	mkdir(build_variant_dir)
	cp('-r', PRISTINE, build_variant_dir)

	with sh.pushd(os.path.join(build_variant_dir, 'simulator')):
		tup('init')
		tup('generate', 'build.sh', '--config', os.path.join('configs', variant))
		sh.Command('./build.sh')()
		sh.test('-x', 'simulator')

		log.info("Built {}".format(variant_file))

		any_fail = False

		sim = sh.Command('./simulator')
		for test in glob.iglob('tests/**/*.bin', recursive=True):
			log.info("\t\ttest: %s", test)
			try:
				sim('-f', test)
				log.info("\t\t\tPASSED")
			except sh.ErrorReturnCode as e:
				# XXX: This is a hack
				if 'exceeds ram size' in e.stderr.decode('utf-8'):
					log.info("\t\t\tSKIPPED -- Test too large for variant")
					continue

				log.info("\t\t\tFAILED -- Error code %s", e.exit_code)
				log.info("\t\t\t\t%s", e)
				any_fail = True

		if any_fail:
			raise NotImplementedError("Failed test cases")

		log.info("All tests passed for {}".format(variant_file))

log.info("All built and all tests passed")

