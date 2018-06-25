#!/usr/bin/env python
import sys
import numpy as np

fft_inst = 8
fft_size = 512

golden_data = [None] * 8
real_data = [None] * 8
for iter in range(fft_inst):  ## ugly
  golden_data[iter] = []
  real_data[iter] = []

golden_result_filename = "../fft/audio_dft_fft.log"
golden_result = open(golden_result_filename, 'r')
fft_no = -1
is_data = 0
for line in golden_result:
  if line.startswith("## fft "):
    if (fft_no >= 0):
      is_data = 1
    else:
      fft_no += 1
  elif is_data:
      data = line.split("\t")
      idx = int(data[0])
      real = float(data[1])
      imag = float(data[2])
      golden_data[fft_no].append(real)
      golden_data[fft_no].append(imag)
      if (idx == (fft_size - 1)):
          fft_no += 1
          is_data = 0
golden_result.close()
#if (fft_no != fft_inst):
#  print "error"
#  sys.exit()

for fft_no in range(fft_inst):
  real_result_filename = "fft_" + str(fft_no) + ".outputsdebug"
  real_result = open(real_result_filename, 'r')
  line_no = 0
  for line in real_result:
#      print line
      if (line_no == 0):
          line_no += 1
          continue
      else:
        data = line.split(" ")
        idx = int(data[0])
        real = float(data[1])
        imag = float(data[2].replace("\n", ""))
        real_data[fft_no].append(real)
        real_data[fft_no].append(imag)
  real_result.close()

for fft_no in range(fft_inst):
  diff = []
  diff_no = 0
  for iter in range(fft_size * 2):
#    if golden_data[fft_no][iter] == 0:
#      diff.append(0)
#    else:
##      diff.append( ( (golden_data[fft_no])[iter] - (real_data[fft_no])[iter] ) / (golden_data[fft_no])[iter] )
    if (golden_data[fft_no][iter] - real_data[fft_no][iter]) > 0.0001:
      diff_no += 1
      diff.append( ( (golden_data[fft_no])[iter] - (real_data[fft_no])[iter] ) )
#  print "#################\nfft_no:%d, average diff:%d%%, number different:%d " % (fft_no, np.mean(diff) * 100, diff_no)
  print "#################\nfft_no:%d, average diff:%.2f, average number:%2.f, number different:%d " % (fft_no, np.mean(diff), np.mean(np.absolute(golden_data[fft_no])), diff_no)
  for item in diff:
#    print "%d%%" % (item * 100) , 
    print "%f " % (item) , 
  print
  for item in range(len(diff)):
    if diff[item] > 1:
        print "%d, golden:%.2f, real:%d" % (item, golden_data[fft_no][item], real_data[fft_no][item])
  print
