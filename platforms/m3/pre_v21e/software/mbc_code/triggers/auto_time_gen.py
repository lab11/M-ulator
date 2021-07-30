import time
from datetime import datetime
import os
import sys
from file_gen import set_trigger

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

while True:
    s = datetime.now().time()

    if(s.hour >= 23 and s.minute >= 58):
        print('Current time is: {}. Sleeping for 2 minutes'.format(s))
        time.sleep(120)
    else:
        break

if(s.second >= 54):
    print('Waiting for next minute')
    time.sleep(10)

s = datetime.now().time()

time.sleep(55 - (s.second + s.microsecond / 1000000))

# def set_trigger(filename, val):
#     print(out_dir + filename + '.bat')
#     with open(out_dir + filename + '.bat', 'w') as f:
#         f.write('call SET_GOC_SPEED.bat\ncall SET_COM.bat\nm3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% message 0000008C {}\n'.format(format(val, 'x').zfill(8)))


###################### 0x0B ##########################
op_name = 'xo_day_time_in_sec'
num = 0x0B
filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
val = (num << 24)

filename1 = 'write-auto-time'
H = s.hour
M = s.minute + 1
val1 = val | (1 << 23)
val1 |= (H << 6)
val1 |= M
set_trigger(trigger_dir, filename1, val1)

