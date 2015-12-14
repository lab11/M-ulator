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
from sh import rm, mv, cp, ls, mkdir, tup, pwd, test

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
	print("Beginning {}".format(variant_file))

	variant = os.path.basename(variant_file)

	build_variant_dir = os.path.join(BUILD_DIR, variant)
	mkdir(build_variant_dir)
	cp('-r', PRISTINE, build_variant_dir)

	with sh.pushd(os.path.join(build_variant_dir, 'simulator')):
		tup('generate', 'build.sh', '--config', os.path.join('configs', variant))
		sh.Command('./build.sh')()
		test('-x', 'simulator')

	print("Finished {}".format(variant_file))

