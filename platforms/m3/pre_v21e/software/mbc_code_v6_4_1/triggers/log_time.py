import time
import os
import sys
import pytz
from datetime import datetime

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

d1 = datetime.now()
d2 = datetime.now().replace(tzinfo=pytz.utc)
utcoffset = round(d1.timestamp() - d2.timestamp())

t = time.time()

with open(out_dir + 'time.log', 'w') as f:
    f.write('trigger_fire_time_utc:{}\nprogrammed_day_time:{}+({})\n'.format(t, int(sys.argv[2]) * 3600 + int(sys.argv[3]) * 60, utcoffset))
