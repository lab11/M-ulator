#!/usr/bin/env python3

import colorlog
import logging
log = colorlog.getLogger(__name__)
colorlog.basicConfig(level=logging.INFO)

import argparse
from contextlib import contextmanager
import glob
import os
import pprint
import sys

# Or write fallback os.walk code for glob ** usage
assert sys.version_info >= (3,5)

import sh
logging.getLogger('sh').setLevel(logging.WARN)

parser = argparse.ArgumentParser()
parser.add_argument('-X', '--stop', action='store_true',
		help='Stop on first error')
args = parser.parse_args()

# n.b. pushd is coming to sh in v1.10
@contextmanager
def pushd(path):
	cwd = os.getcwd()
	os.chdir(path)
	yield
	os.chdir(cwd)

# Make sure everything is up-to-date
sh.tup()

def tests():
	any_fail = False

	variants = {}

	for variant in os.listdir('.'):
		if not variant[:6] == 'build-':
			continue

		log.info("Running tests for variant: %s", variant)
		with pushd(variant):
			sim = sh.Command('./simulator')
			variants[variant] = {}

			for test in glob.iglob('tests/**/*.bin', recursive=True):
				variants[variant][test] = {}
				log.info("\t\ttest: %s", test)
				try:
					sim('-f', test)
					variants[variant][test]['exit_code'] = 0
					log.info("\t\t\tPASSED")
				except sh.ErrorReturnCode as e:
					variants[variant][test]['exit_code'] = e.exit_code
					variants[variant][test]['exception'] = e
					
					# XXX: This is a hack
					if 'exceeds ram size' in e.stderr.decode('utf-8'):
						log.info("\t\t\tSKIPPED -- Test too large for variant")
						continue

					log.info("\t\t\tFAILED -- Error code %s", e.exit_code)
					log.info("\t\t\t\t%s", e)
					any_fail = True
					if args.stop:
						sys.exit(-1)

	if not any_fail:
		print("All tests passed.")

tests()
