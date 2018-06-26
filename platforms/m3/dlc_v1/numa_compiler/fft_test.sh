#!/usr/bin/env bash

pushd ARM_code
make test_fft_full_quick
./process.py test_fft_full_quick
popd 
./cp_2_mac_mov.sh
