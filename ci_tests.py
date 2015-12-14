#!/usr/bin/env python3

import logging
log = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

import os
import pprint
import sys
import time

sys.path.insert(0, '/home/ppannuto/code/sh/')

import sh
from sh import rm, mv, cp, ls, mkdir, tup, pwd
logging.getLogger("sh").setLevel(logging.WARNING)

BUILD_DIR = 'builds'
PRISTINE = os.path.join(BUILD_DIR, 'simulator')


# Start from scratch
rm('-r', '-f', BUILD_DIR)
mkdir(BUILD_DIR)


# No fuse in the CI environment means we need a unique directory for each
# variant. We first grab a copy of the simulator folder and wipe out all things
# tup to use as a template so that the same script will work locally and in CI
cp('-r', 'simulator', PRISTINE)
with sh.pushd(PRISTINE):
	rm('-r', '-f', '.tup')
	rm('-r', '-f', sh.glob('build-*'))


for variant_file in os.listdir('simulator/configs'):
	log.info("Building {}".format(variant_file))

	variant = os.path.basename(variant_file)

	build_variant_dir = os.path.join(BUILD_DIR, variant)
	mkdir(build_variant_dir)
	cp('-r', PRISTINE, build_variant_dir)

	with sh.pushd(os.path.join(build_variant_dir, 'simulator')):
		tup('generate', 'build.sh', '--config', os.path.join('configs', variant))
		sh.Command('./build.sh')()
		sh.test('-x', 'simulator')

		log.info("Built {}".format(variant_file))

		if variant_file[:2] == 'm3':
			log.info("Skipping tests for {}".format(variant_file))
			continue

		log.info("Testing {}".format(variant_file))

		def get_test_paths():
			tests = {}
			for family in os.listdir('tests/operations/'):
				if not os.path.isdir(os.path.join('tests/operations/', family)):
					continue

				tests[family] = {}
				for test in os.listdir(os.path.join('tests/operations/', family)):
					if test[-2:] == '.s':
						tests[family][test[:-2]] = {}
			return tests

		tests = get_test_paths()
		any_fail = False

		sim = sh.Command('./simulator')
		for family in tests:
			log.info("\tfamily: %s", family)
			for test in tests[family]:
				log.info("\t\ttest: %s", test)
				image = os.path.join('tests/operations', family, test) + '.bin'
				try:
					sim('-f', image)
					tests[family][test]['exit_code'] = 0
					log.info("\t\t\tPASSED")
				except sh.ErrorReturnCode as e:
					tests[family][test]['exit_code'] = e.exit_code
					tests[family][test]['exception'] = e
					log.info("\t\t\tFAILED -- Error code %s", e.exit_code)
					log.info("\t\t\t\t%s", e)
					any_fail = True

		if any_fail:
			raise NotImplementedError("Failed test cases")

		log.info("All tests passed for {}".format(variant_file))

log.info("All built and all tests passed")

