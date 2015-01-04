#!/usr/bin/env python3

import logging
log = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

from contextlib import contextmanager
import os
import pprint
import sys

import sh

# n.b. pushd is coming to sh in v1.10
@contextmanager
def pushd(path):
	cwd = os.getcwd()
	os.chdir(path)
	yield
	os.chdir(cwd)

# Make sure everything is up-to-date
sh.tup()

def test_operations():
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

	for variant in os.listdir('.'):
		if not variant[:6] == 'build-':
			continue

		log.info("Running tests for variant: %s", variant)
		with pushd(variant):
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

	if not any_fail:
		print("All tests passed.")

test_operations()
