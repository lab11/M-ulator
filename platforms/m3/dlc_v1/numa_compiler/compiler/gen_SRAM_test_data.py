#!/usr/bin/env python

###generate random data to test 32KB compiled sram

import sys
import random


## default value
# size = 1024;
# size = 5760;
size = 2048;
# size = 1706;
# size = 1365;
data = 0xAAAAAAAA;
# data = 0x55555555;
## 

############# weights
test_file = open("test_SRAM_A5.data", 'w');
for i in range(size):
	# string = str(random.randint(-214783648, 2147483647)); 
	# string = '{0:08X}'.format(random.randint(0, 4294967295)); #Uppercase
	# string = '{0:08x}'.format(random.randint(0, 4294967295)); #Lowercase
	
	string = '{0:08x}'.format(data); #Lowercase
	# data = ~data;
	data = (~data) & 0xffffffff; # convert to unsigned 32-bit
	test_file.write(string + string + string + '\n');
  
test_file.close()
