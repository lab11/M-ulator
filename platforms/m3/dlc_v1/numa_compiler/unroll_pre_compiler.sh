#!/usr/bin/env bash
#### generate memory footprint and instructions

####### choose which test to run
test=PRE_FFT
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
  ####
  ## revert
  git checkout dd0c54f libs/utils.c || { exit 1; }
  git checkout dd0c54f include/utils.h || { exit 1; }
  ####
	make test_fft_full || { exit 1; }
  python ./process.py test_fft_full
elif [ $test == "PRE_FFT" ]; then 
  ####
  ## revert
#  git checkout a5a9f44 libs/utils.c || { exit 1; }
#  git checkout a5a9f44 include/utils.h || { exit 1; }
  git checkout dd0c54f libs/utils.c || { exit 1; }
  git checkout dd0c54f include/utils.h || { exit 1; }
  ####
	make test_preprocessing_fft_unroll || { exit 1; }
  python ./process.py test_preprocessing_fft_unroll
  ####
  cp test_preprocessing_fft_unroll/*.mem test_fft_full/
  ####
else
	echo "INVALID TEST"; exit
fi

popd

