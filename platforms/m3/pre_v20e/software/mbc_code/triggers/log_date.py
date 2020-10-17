import time
from datetime import datetime
import os
import sys

trigger_dir = sys.argv[1]
out_dir = os.path.dirname(os.path.abspath(__file__)) + '/' + trigger_dir + '/'

t = time.time()
N = (int) (t / 86400)

with open(out_dir + 'date.log', 'w') as f:
    f.write('trigger_fire_date_utc:{}\nprogrammed_date:{}\n'.format(N, sys.argv[2]))
