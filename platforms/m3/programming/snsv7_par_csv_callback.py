import csv
from datetime import datetime
import sys

logfile = open('mbus_snoop_log.txt','w')

wr = csv.writer(open('snsv7_snoop.txt','w'), delimiter=',', lineterminator='\n')
wr.writerow(['DATE','TIME','C_MEAS','C_REF','C_REV','C_PAR'])

count = 0

cdc_cmeas = 0
cdc_crev = 0
cdc_cpar = 0
cdc_cref = 0
cdc_date = 0
cdc_time = 0
cdc_group = False

def callback(time, address, data, cb0=-1, cb1=-1):
    global count
    global cdc_cmeas, cdc_crev, cdc_cpar, cdc_cref, cdc_date, cdc_time, cdc_group

    # m3_ice snoop prints this for you now [though needs to add count - can do]
    #print("@" + str(count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")")
    print >> logfile, "@" + str(count) + " Time: " + datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3] + "  ADDR: 0x" + address.encode('hex') + "  DATA: 0x" + data.encode('hex') + "  (ACK: " + str(not cb1) + ")"
    if (str(int(address.encode('hex'),16))=="116"):
        cdc_group = True
        cdc_cmeas = int(data.encode('hex'),16)
        cdc_time = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        cdc_date = datetime.now().strftime("%Y-%m-%d")
    elif (str(int(address.encode('hex'),16))=="118"):
        if cdc_group:
            cdc_cref = int(data.encode('hex'),16)
    elif (str(int(address.encode('hex'),16))=="119"):
        if cdc_group:
            cdc_crev = int(data.encode('hex'),16)
    elif (str(int(address.encode('hex'),16))=="121"):
        if cdc_group:
            cdc_cpar = int(data.encode('hex'),16)
            wr.writerow([cdc_date,cdc_time,cdc_cmeas,cdc_cref,cdc_crev,cdc_cpar])
            count += 1
            cdc_group = False
    #if count>args.killcount:
    #    sys.exit()
    # TODO(Pat): I need to add support for custom arguments, which I will do
    if count > 100000:
        sys.exit()
