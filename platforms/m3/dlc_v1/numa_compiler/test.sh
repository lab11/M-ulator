#!/usr/bin/env bash
#### generate memory footprint and instructions

####### choose which test to run
if [ -z "$1" ]; then
  echo "Please provide which test to run as argument:"
#  echo " MAC_MOV, MACNLI_MOV, MEMCPY, NLI, MACNLI"
#  echo " MAC_MOV, MEMCPY, MOV_MAC_small"
  echo " MEMCPY, MACNLI_MOV, MOV_MAC_small"
#  echo " DNN, DNN_kws, FFT_FULL, MOV_FULL"
  echo " DNN, DNN_kws, MOV_FULL"
  exit
fi
test=$1

######
#jiwang: tested test
#MOV_FULL
#MOV_ENERGY
#MOV_MAC_small: 3 dnn.weight: random/24x24/16x16
#MAC_MOV
#DNN
#FACE
#DNN_REALDATA: has wrong middle and final stage result 
#FFT_FULL
######
#possible bugs if verilog results differ:
#1. ARM code: verilog simulation needs time to finish. in ARM code, delay/halt/clock-gate must be added between start_pe and signal_done.


rm ARM_code/libs/dnn.nli.c
pushd compiler
[ -d "memory" ] || mkdir memory

if [ $test == "MEMCPY" ]; then 
  python ./gen_fake_memcpy.py

else
  if [ $test == "MAC_MOV" ]; then 
# Jigncheng tested: OK
#  	python ./gen_fake_mac_single_layer.py 403 400 8 12 6 32
#    perl ./MAC_MOV_PAR.pl > MAC_MOV_PAR.log 
    python ./dnn_gen_fake_mac.py
    perl ./DNN.pl > DNN.log
	# perl ./DNN_L4.pl > DNN_L4.log
	# perl ./DNN_L3.pl > DNN_L3.log
    python ./gen_m0_file_v2.py
  elif [ $test == "MACNLI_MOV" ]; then 
# Jigncheng tested: OK
  	python ./gen_fake_mac_single_layer.py 403 400 12 12 6 32
    perl ./gen_fake_nli.pl
    perl ./MACNLI_MOV_PAR.pl > MACNLI_MOV_PAR.log 
    cp dnn.nli.c ../ARM_code/libs/
	perl gen_m0_data_file_mem.pl MACNLI_MEM_0.data
	perl gen_m0_data_file_mem.pl MACNLI_MEM_1.data
	perl gen_m0_data_file_mem.pl MACNLI_MEM_2.data
	perl gen_m0_data_file_mem.pl MACNLI_MEM_3.data
### ./gen_fake_fft.py is not suitable for FFT_PAR.pl anymore
#  elif [ $test == "FFT" ]; then    
#  	python ./gen_fake_fft.py
#    perl ./FFT_PAR.pl > FFT_PAR.log 
#    python ./gen_m0_file.py 0
  elif [ $test == "NLI" ]; then 
  	perl ./gen_fake_nli.pl
    perl ./NLI_PAR.pl > NLI_PAR.log 
    python ./gen_m0_file.py 0 1 1
    cp dnn.nli.c ../ARM_code/libs/
  elif [ $test == "MACNLI" ]; then 
  	python ./gen_fake_mac_single_layer.py
    perl ./gen_fake_nli.pl
    perl ./MACNLI.pl > MACNLI.log 
    python ./gen_m0_file.py 0 1 1
    cp dnn.nli.c ../ARM_code/libs/
  elif [ $test == "MOV_MAC_small" ]; then 
  	python ./gen_fake_mac_single_layer.py 24 24 8 12 6 32 0
    perl ./MOV_MAC_small.pl > MOV_MAC_small.log 
    python ./gen_m0_file_v2.py
  elif [ $test == "DNN" ]; then 
    python ./dnn_gen_fake_mac.py
    perl ./DNN.pl > DNN.log
    python ./gen_m0_file_v2.py
  elif [ $test == "DNN_REALDATA" ]; then 
    perl ./DNN_realData.pl > DNN_REALDATA.log
    python ./gen_m0_file_v2.py
  elif [ $test == "FACE" ]; then 
    perl ./FACE.pl > FACE.log
    python ./gen_m0_file_v2.py
  elif [ $test == "DNN_kws" ]; then 
    python ./dnn_gen_fake_mac.py
  	perl ./gen_fake_nli.pl
    perl ./DNN_GEN_WEIGHT.pl
    perl ./DNN_kws.pl > DNN_kws.log
    python ./dnn_gen_m0_file.py
    cp dnn.nli.c ../ARM_code/libs/
  elif [ $test == "FFT_FULL" ]; then 
    python ./gen_fake_fft.py
    perl ./FFT_FULL.pl > FFT_FULL.log
    python ./gen_m0_file_v2.py
  elif [ $test == "MOV_FULL" ]; then 
    perl ./MOV_FULL.pl > MOV_FULL.log
    python ./gen_m0_file_v2.py
  elif [ $test == "MOV_ENERGY" ]; then 
  #Added by Jingcheng to test DNN memory Read/Write Energy
    perl ./MOV_Energy.pl > MOV_Energy.log
    python ./gen_m0_file_v2.py
	perl gen_m0_data_file_mem.pl MOV_INIT_MEM_0.data
  elif [ $test == "MOV_MEM" ]; then 
    perl ./MOV_MEM.pl > MOV_MEM.log
    python ./gen_m0_file_v2.py
  elif [ $test == "TEST_MOV" ]; then 
  	python ./gen_fake_mac.py 403 400 8 12 6 32
    perl ./MOV_PAR.pl > MOV_PAR.log 
  elif [ $test == "MEM_TEST_1" ]; then 
    perl ./MEM_TEST_1.pl > MEM_TEST_1.log
    python ./gen_mem_test_m0_file.py
  elif [ $test == "MOV_COLLISION" ]; then 
#   added on Feb 6, 2017 to test PE_STALL_MEM
    perl ./MOV_COLLISION.pl > MOV_COLLSION.log
    python ./gen_m0_file_v2.py
  else
  	echo "INVALID TEST"; exit
  fi
  
  cp dnn.inst.c ../ARM_code/libs/
  cp dnn_parameters.h ../ARM_code/include/
fi

popd

#### compile ARM code 
pushd ARM_code
make clean
if [ $test == "MAC_MOV" ]; then
	make test_mac_mov || { exit 1; }
  python ./process.py test_mac_mov
  python ./labview_arm.py test_mac_mov
  python ./process_SRAM_4K.py test_mac_mov
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
elif [ $test == "MOV_MAC_small" ]; then
	make test_mov_mac_small || { exit 1; }
  python ./process.py test_mov_mac_small
  python ./labview_arm.py test_mov_mac_small
elif [ $test == "DNN" ]; then
	make test_dnn || { exit 1; }
  python ./process.py test_dnn
  python ./labview_arm.py test_dnn
  python ./process_SRAM_4K.py test_dnn
elif [ $test == "DNN_REALDATA" ]; then
	make test_dnn_realData || { exit 1; }
  python ./process.py test_dnn_realData
  python ./labview_arm.py test_dnn_realData
#Added by Jingcheng to test face detection application
elif [ $test == "FACE" ]; then
	make test_face || { exit 1; }
	python ./process.py test_face
	python ./labview_arm.py test_face
elif [ $test == "DNN_kws" ]; then
	make test_dnn_kws || { exit 1; }
  python ./process.py test_dnn_kws
elif [ $test == "FFT_FULL" ]; then
#	make test_fft_full || { exit 1; }
#  python ./process.py test_fft_full
	make test_fft_full_quick || { exit 1; }
  python ./process.py test_fft_full_quick
elif [ $test == "MOV_FULL" ]; then
	make test_mov_full || { exit 1; }
  python ./process.py test_mov_full
  python ./labview_arm.py test_mov_full
elif [ $test == "MOV_ENERGY" ]; then
	make test_mov_energy || { exit 1; }
  python ./process.py test_mov_energy
  python ./labview_arm.py test_mov_energy
elif [ $test == "MOV_MEM" ]; then
	make test_mov_memory || { exit 1; }
  python ./process.py test_mov_memory
  python ./labview_arm.py test_mov_memory
elif [ $test == "TEST_MOV" ]; then
	make test_mov || { exit 1; }
  python ./process.py test_mov
elif [ $test == "MEM_TEST_1" ]; then
	make test_mem_1 || { exit 1; }
  python ./process.py test_mem_1
elif [ $test == "MOV_COLLISION" ]; then
#   added on Feb 6,2017 to test PE_STALL_MEM signal
	make test_mov_collision || { exit 1; }
  python ./process.py test_mov_collision
  python ./labview_arm.py test_mov_collision
  python ./process_SRAM_4K.py test_mov_collision
else
	echo "INVALID TEST"; exit
fi

popd

