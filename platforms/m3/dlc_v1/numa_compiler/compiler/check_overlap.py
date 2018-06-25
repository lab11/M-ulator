#!/usr/bin/env python
import sys
import math

#####
def print_all_banks(to_start, to_end, i_start, i_end, o_start, o_end, os_start, os_end, w_start, w_end):
  print "to:", to_start, to_end
  print "i:", i_start, i_end
  print "o:", o_start, o_end
  print "os:", os_start, os_end
  print "w:", w_start, w_end

def end_bank(start, total_size):
  size = total_size
#  banks = [5728, 5696, 5664, 5632,  5504, 5376, 5428, 5120,  4864, 4608, 4352, 4096,  3072, 2048, 1024, 0]
  banks = [32]*4 + [128]*4 + [256]*4 + [1024]*4
  for i in range(start, len(banks)):
    size -= banks[i]
    if (size < 0):
      return i
  print "error: out of memory range\n", start, total_size
  sys.exit()

def check_legal_precision(prec, if_to = False):
  if (not if_to):
    if (prec == 6) or (prec == 8) or (prec == 12) or (prec == 16):
      return True
  else:
    if (prec == 16) or (prec == 24) or (prec == 32):
      return True
  print "illgel precision:", prec
  sys.exit()
#####
def check_overlap(input_size = 403, output_size = 100, \
                  i_prec = 12, o_prec = 12, w_prec = 6, to_prec = 32, \
                  to_start = 0, i_start = 4, o_start = 8, os_start = 9, w_start = 11): 
#  to_start = 4* 0 + 0     ## H1B1
#  i_start  = 4* 1 + 0     ## H2B1
#  o_start  = 4* 2 + 0     ## H3B1
#  os_start = 4* 2 + 1     ## H3B2
#  w_start  = 4* 2 + 3     ## H3B3
  #####
  i_datapword = 96 / i_prec ## data per word
  o_datapword = 96 / o_prec
  w_datapword = 96 / w_prec
  to_datapword = 96 / to_prec
  
  ROW_NUM_MIN_FACTOR = 48   ## magic number for zero padding
  
  output_zp_size = math.ceil(output_size/ROW_NUM_MIN_FACTOR)*ROW_NUM_MIN_FACTOR
  input_zp_size  = math.ceil(input_size/i_datapword)*i_datapword
  
  to_size = int(output_zp_size / to_datapword)
  i_size  = int(input_zp_size / i_datapword)
  o_size  = int(output_zp_size / o_datapword)
  os_size = int(output_zp_size / o_datapword)
  w_size  = int(output_zp_size * input_zp_size / w_datapword)
  
  to_end = end_bank(to_start, to_size) 
  i_end  = end_bank(i_start, i_size) 
  o_end  = end_bank(o_start, o_size) 
  os_end = end_bank(os_start, os_size) 
  w_end  = end_bank(w_start, w_size) 
  
  start_banks = [to_start, i_start, o_start, os_start, w_start]
  end_banks   = [to_end, i_end, o_end, os_end, w_end]
  for bank_a in range(len(start_banks)):
    for bank_b in range(len(start_banks)):
      if (start_banks[bank_a] < start_banks[bank_b]) \
        and (end_banks[bank_a] >= start_banks[bank_b]):
        print "bank overlap!!"
        print_all_banks(to_start, to_end, i_start, i_end, o_start, o_end, os_start, os_end, w_start, w_end)
        sys.exit()

if __name__ == "__main__":
  if len(sys.argv) == 1:
    check_overlap()
  elif len(sys.argv) == 3:
    check_overlap(int(sys.argv[1]), int(sys.argv[2]))
  elif len(sys.argv) == 7:
    check_overlap(int(sys.argv[1]), int(sys.argv[2]), \
                  int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6]))
  elif len(sys.argv) == 12:
    check_overlap(int(sys.argv[1]), int(sys.argv[2]), \
                  int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6]), \
                  int(sys.argv[8]), int(sys.argv[9]), int(sys.argv[10]), int(sys.argv[11]), int(sys.argv[12]), int(sys.argv[13]))
  else:
    print "please provide correct number of arguments"
  print_all_banks(to_start, to_end, i_start, i_end, o_start, o_end, os_start, os_end, w_start, w_end)
