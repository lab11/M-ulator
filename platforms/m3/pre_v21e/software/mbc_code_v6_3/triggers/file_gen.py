import yaml
import sys
import os
import glob
import re
import pytz
from datetime import datetime

def set_trigger(trigger_dir, filename, val):
    ppath = os.path.abspath(sys.path[0]) + '/'
    out_dir = ppath + trigger_dir + '/'

    # print(trigger_dir + filename + '.bat')
    with open(out_dir + filename + '.bat', 'w') as f:
        print(out_dir + filename + '.bat')
        f.write('call SET_GOC_SPEED.bat\ncall SET_COM.bat\nm3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% message 0000008C {}\n'.format(format(val, 'x').zfill(8)))

        # special hooks
        if((val >> 24) == 0x0A and (val >> 23) & 1 == 1):
            # epoch days write
            f.write('python ../log_date.py {} {}\n'.format(trigger_dir, val & 0xFFFF))

        elif((val >> 24) == 0x0B and (val >> 23) & 1 == 1):
            # epoch days write
            f.write('python ../log_time.py {} {} {}\n'.format(trigger_dir, (val >> 6) & 0x1F, val & 0x3F))

def parse_date(date):
    d = int(datetime(date['YYYY'], date['MM'], date['DD'], 0, 0, tzinfo=pytz.utc).timestamp() / 86400)
    return d

