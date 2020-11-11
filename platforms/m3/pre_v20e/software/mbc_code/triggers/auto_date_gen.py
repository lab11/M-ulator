#############################################################
#  Trigger generator: v1.0.1
#  Author: Roger Hsiao
#  
#  Usage: python generate_triggers.py [out_dir]
#  Takes in a directory name, parses [path to out_dir]/trigger_configs.yaml and generates the triggers in [path to out_dir]
#  
#  v1.0.0: Starts version number
#  v1.0.1: When python takes the system epoch time, it doesn't account for the timezone, so we must fix the timezone in date.log and time.log
#  
#############################################################

import time
from datetime import datetime
import os
import sys
import pytz
from file_gen import set_trigger

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

while True:
    s = datetime.now().time()

    if(s.hour >= 23 and s.minute >= 59):
        print('Current time is: {}. Sleeping for 2 minutes'.format(s))
        time.sleep(120)
    else:
        # get tz
        d1 = datetime.now()
        d2 = datetime.now().replace(tzinfo=pytz.utc)
        utcoffset = d1.timestamp() - d2.timestamp()
        t = time.time() - utcoffset
        break

# def set_trigger(filename, val):
#     print(out_dir + filename + '.bat')
#     with open(out_dir + filename + '.bat', 'w') as f:
#         f.write('call SET_GOC_SPEED.bat\ncall SET_COM.bat\nm3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% message 0000008C {}\n'.format(format(val, 'x').zfill(8)))



###################### 0x0A ##########################
op_name = 'epoch_days'
num = 0x0A
filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
val = (num << 24)

filename1 = 'write-auto-date'
d = {'YYYY': datetime.now().date().year, 'MM': datetime.now().date().month, 'DD': datetime.now().date().day}
print(d)
N = (int) (t / 86400)
val1 = val | (1 << 23)
val1 |= N
set_trigger(trigger_dir, filename1, val1)
