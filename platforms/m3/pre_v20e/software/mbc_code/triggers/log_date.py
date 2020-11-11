import time
from datetime import datetime
import pytz
import os
import sys

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

# get tz
d1 = datetime.now()
d2 = datetime.now().replace(tzinfo=pytz.utc)
utcoffset = round(d1.timestamp() - d2.timestamp())
t = time.time() - utcoffset
N = (int) (t / 86400)

with open(out_dir + 'date.log', 'w') as f:
    f.write('trigger_fire_date (local):{}\ntimezone offset:+({})\nprogrammed_date:{}\n'.format(N, utcoffset, sys.argv[2]))
