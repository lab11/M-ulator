#!/usr/bin/python
# Python script to parse Pstack radio output
# Looks for alternating transmissions of C_MEAS and C_REF data
# 9/2/2015 Gyouho Kim
# 11/15/2015 Gyouho Kim: Added date/time logging

import sys
import csv
import matplotlib.pyplot as plt
import numpy as np

print()

text_file = open(sys.argv[1], "r")
lines = text_file.readlines()
text_file.close()
output_file = sys.argv[1].replace('.txt','')
output_file += '.csv'
#print lines
#print len(lines)

cmeas = []
cref = []
date = []
time = []

cmeas_int = []
cref_int = []

cmeas_cal = []
i=0
j=0

for line in lines:
	if line[0]=='0' or line[0]=='1':
		data = line.strip()
		data = data.replace(',','')
		data = data.replace(' ','')
		if i%2:
			cref.append(data)
			cref_int.append(int(data,2))
		else:
			cmeas.append(data)
			cmeas_int.append(int(data,2))
			
			timestamp_split = lines[j-2].split()
			time.append(timestamp_split[3])
			date.append(timestamp_split[1] + " " + timestamp_split[2])

		i += 1 # data index
	j += 1 # line index

for i in range(0,min(len(cmeas_int),len(cref_int))):
	cmeas_cal.append(float(cmeas_int[i])/float(cref_int[i])*100000)

print "C_MEASURE:\n"
print cmeas
print "\nC_REF:\n"
print cref

print "\nC_MEASURE:\n"
print cmeas_int
print len(cmeas_int)
print "\nC_REF:\n"
print cref_int
print len(cref_int)

print "\nC_MEAS_CALIBRATED:\n"
print cmeas_cal
print len(cref_int)


# Export to CSV
rows = zip(date,time,cmeas_int,cref_int,cmeas_cal)

wr = csv.writer(open(output_file,'w'), delimiter=',', lineterminator='\n')
wr.writerow(['DATE','TIME','C_MEAS','C_REF','C_MEAS_CAL'])
for row in rows:
	wr.writerow(row)

fig1 = plt.figure()

plt.plot(range(1,len(cmeas_cal)+1),list(reversed(cmeas_cal)))
plt.ylabel('CMEAS/CREF*1E5')
plt.xlabel('Time (pts)')

plt.show()

