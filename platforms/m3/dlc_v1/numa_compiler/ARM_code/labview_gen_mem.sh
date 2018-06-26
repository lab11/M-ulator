#usage: labview_gen_mem.sh test_read_dnn_sram/golden_memory/MEMCPY_0 .goldendata crap
python labview_gen_mem.py $1 $2 $3
perl -pi -e 's/\n/\r\n/' $1"L4B1"$2 
perl -pi -e 's/\n/\r\n/' $1"L4B2"$2 
perl -pi -e 's/\n/\r\n/' $1"L4B3"$2 
perl -pi -e 's/\n/\r\n/' $1"L4B4"$2 
perl -pi -e 's/\n/\r\n/' $1"L3"$2 
perl -pi -e 's/\n/\r\n/' $1"L1L2"$2 
