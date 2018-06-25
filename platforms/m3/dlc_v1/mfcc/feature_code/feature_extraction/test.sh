#!/usr/bin/env bash

scons

./examples/example ../../sr584.wav > sr584.out
./examples/example ../../sr597.wav > sr597.out
./examples/example ../../sr598.wav > sr598.out

cat sr584.out > all.out
cat sr597.out >> all.out
cat sr598.out >> all.out

./parse_test_out.py
