#!/usr/bin/env bash

pushd ARM_code
#cp test_dnn/*.mem test_mov_full/
cp test_preprocessing_m0/*.mem test_mov_full/
#cp test_dnn_kws/*.mem test_mov_full/
#cp test_mov_full/*.mem test_mov_full/
#cp test_mem_1/*.mem test_mov_full/
popd

#pushd compiler
#for PE in 0 1 2 3; do
##  cp MEM_TEST_1_INIT_MEM_${PE}.data MAC_MEM_${PE}.data
#  cp DNN_INIT_MEM_${PE}.data MAC_MEM_${PE}.data
#  perl V2_MEM.pl MAC_MEM_${PE}.data
#done
