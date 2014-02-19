#!/usr/bin/env python2
# vim: sts=4 ts=4 sw=4 noet:

import os
import logging

def get_logger(name):
	l = logging.getLogger(name)
	l.level = logging.DEBUG
	try:
		l.addHandler(get_logger.handler)
	except AttributeError:
		h = logging.StreamHandler()
		h.level = log_level_from_environment()
		get_logger.handler = h
		l.addHandler(get_logger.handler)
	try:
		get_logger.handler.setFormatter(get_logger.formatter)
	except AttributeError:
		f = logging.Formatter("%(levelname)s\t%(message)s")
		get_logger.formatter = f
		get_logger.handler.setFormatter(get_logger.formatter)

	return l

def log_level_from_environment():
	try:
		os.environ['ICE_DEBUG']
		return logging.DEBUG
	except KeyError:
		return logging.INFO
