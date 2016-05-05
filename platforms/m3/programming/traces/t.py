#!/usr/bin/env python3

import time

ofile = open('o', 'w')

t = time.time()

data = None
for line in open('20160321.out'):
	if 'Address' in line:
		if data is not None:
			ofile.write('{},{:02x},'.format(t,addr))
			for d in data:
				ofile.write('{:02x}'.format(d))
			ofile.write('\n')
		addr = int(line.split()[1], 16)
		data = []
		t += .1
	else:
		assert 'Data' in line
		data.append(int(line.split()[1], 16))

