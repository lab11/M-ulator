#!/usr/bin/env bash

pushd ARM_code
#cp test_dnn/*.mem test_mac_mov/
cp test_macnli_mov/*.mem test_mac_mov/
#cp test_preprocessing_m0/*.mem test_mac_mov/
#cp test_dnn_kws/*.mem test_mac_mov/
#cp test_mov_full/*.mem test_mac_mov/
#cp test_mov_mac_small/*.mem test_mac_mov/
#cp test_fft_full/*.mem test_mac_mov/
#cp test_fft_full_quick/*.mem test_mac_mov/
#cp test_mem_1/*.mem test_mac_mov/
popd

pushd compiler
for PE in 0 1 2 3; do
#  cp FFT_INIT_MEM_${PE}.data MAC_MEM_${PE}.data
#  cp MOV_MAC_small_INIT_MEM_${PE}.data MAC_MEM_${PE}.data
#  cp DNN_INIT_MEM_${PE}.data MAC_MEM_${PE}.data
  cp MACNLI_MEM_${PE}.data MAC_MEM_${PE}.data
  perl V2_MEM.pl MAC_MEM_${PE}.data
done
