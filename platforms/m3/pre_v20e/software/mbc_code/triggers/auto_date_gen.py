import time
from datetime import datetime
import os
import sys

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

while True:
    t = time.time()
    s = datetime.now().time()

    if(s.hour >= 23 and s.minute >= 27):
        print('Current time is: {}. Sleeping for 2 minutes'.format(s))
        time.sleep(120)
    else:
        break

def set_trigger(filename, val):
    print(out_dir + filename + '.bat')
    with open(out_dir + filename + '.bat', 'w') as f:
        f.write('call SET_GOC_SPEED.bat\ncall SET_COM.bat\nm3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% message 0000008C {}\n'.format(format(val, 'x').zfill(8)))

###################### 0x0A ##########################
op_name = 'epoch_days'
num = 0x0A
filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
val = (num << 24)

filename1 = 'write-auto-date'
N = (int) (t / 86400)
val1 = val | (1 << 23)
val1 |= N
set_trigger(filename1, val1)
