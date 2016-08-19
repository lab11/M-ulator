#!/usr/bin/env bash

APP=test_memcpy
APP=test_preprocessing_m0
APP=test_mov

#make clean
#make $APP || { exit 1;}
#./process.py $APP 
#cp $APP/*.mem test_fft_full/

pushd ../compiler
for PE in 0 1 2 3; do
  cp TEST_MOV_MEM_${PE}.data FFT_INIT_MEM_${PE}.data
  perl V2_MEM.pl FFT_INIT_MEM_${PE}.data
done
