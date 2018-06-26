#!/usr/bin/env bash
#### generate memory footprint and instructions

####### choose which test to run
if [ -z "$1" ]; then
  echo "Please provide which test to run as argument:"
  exit
fi
test=$1
######

#### compile ARM code 
pushd ARM_code
make clean
if [ $test == "MAC_MOV" ]; then
	make test_mac_mov || { exit 1; }
  python ./process.py test_mac_mov
elif [ $test == "MACNLI_MOV" ]; then
	make test_macnli_mov || { exit 1; }
  python ./process.py test_macnli_mov
elif [ $test == "MEMCPY" ]; then
	make test_memcpy || { exit 1; }
  python ./process.py test_memcpy
elif [ $test == "FFT" ]; then
	make test_fft || { exit 1; }
  python ./process.py test_fft
elif [ $test == "NLI" ]; then
	make test_nli || { exit 1; }
  python ./process.py test_nli
elif [ $test == "MACNLI" ]; then
	make test_macnli || { exit 1; }
  python ./process.py test_macnli
elif [ $test == "DNN" ]; then
	make test_dnn || { exit 1; }
  python ./process.py test_dnn
elif [ $test == "FFT_FULL" ]; then
	make test_fft_full || { exit 1; }
  python ./process.py test_fft_full
elif [ $test == "FFT_F8" ]; then
	make test_fft_full_quick || { exit 1; }
  python ./process.py test_fft_full_quick
elif [ $test == "PRE_FFT" ]; then 
	make test_preprocessing_fft || { exit 1; }
  python ./process.py test_preprocessing_fft
elif [ $test == "PRE_M0" ]; then 
	make test_preprocessing_m0 || { exit 1; }
  python ./process.py test_preprocessing_m0
else
	echo "INVALID TEST"; exit
fi

popd

./cp_2_fft_full.sh
