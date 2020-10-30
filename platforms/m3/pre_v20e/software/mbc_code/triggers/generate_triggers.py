#############################################################
#  Trigger generator: v1.0.0
#  Author: Roger Hsiao
#  
#  Usage: python generate_triggers.py [out_dir]
#  Takes in a directory name, parses [path to out_dir]/trigger_configs.yaml and generates the triggers in [path to out_dir]
#  
#  v1.0.0: Starts version number
#  
#############################################################


from check_template import check_template
from file_gen import set_trigger
from file_gen import parse_date

import yaml
import sys
import os
import glob
import re

if(check_template(sys.argv[1]) != 0):
    print('Template check failed')
    exit()

TIMEOUT_AFTER_PROGRAM = 20
TIMEOUT_TIME = 5
TIMEOUT_AFTER_RADIO = 10

trigger_dir = sys.argv[1]
ppath = os.path.abspath(sys.path[0]) + '/'
out_dir = ppath + trigger_dir + '/'
config_file = out_dir + 'trigger_configs.yaml'
# remove previous triggers except for programming
print('Removing old triggers')
for f in glob.glob('{}*.bat'.format(out_dir)):
    if(re.match('.*GOCx15_mbc_code.*', f)):
        pass
    else:
        os.remove(f)

# def set_trigger(filename, val):
#     with open(out_dir + filename + '.bat', 'w') as f:
#         f.write('call SET_GOC_SPEED.bat\ncall SET_COM.bat\nm3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% message 0000008C {}\n'.format(format(val, 'x').zfill(8)))

def check_bounds(val, num_bits):
    if(val < 0 or val >= (1 << num_bits)):
        raise Exception('Value out of bounds')

os.system('cp \'{}\' \'{}\''.format(ppath + 'goc_res.bat', out_dir + 'goc_res.bat'))

with open(config_file, 'r') as file:
    l = yaml.load(file, Loader=yaml.FullLoader)

    with open(out_dir + 'GOC_PROGRAM_mbc_code.bat', 'w') as f:
        f.write('call SET_COM.bat\ncall SET_GOC_SPEED.bat\n')
        f.write('m3_ice -y -s %COM% goc -d %GOC_DELAY% -V3 -g %GOC_SPEED_PR% flash {}\n'.format(l['hex_path']))
        f.write(';set ICE_DEBUG=1\ntimeout {}\n'.format(TIMEOUT_AFTER_PROGRAM))

    with open(out_dir + 'SET_COM.bat', 'w') as f:
        f.write('SET COM=COM' + str(l['COM_PORT']) + '\n')

    with open(out_dir + 'SET_GOC_SPEED.bat', 'w') as f:
        f.write('set GOC_SPEED=60\nset GOC_SPEED_PR=80\nset GOC_SPEED_FAST=80\nset GOC_DELAY=0.5\n')

    # suppress radio trigger # this is just 0x12 with num of hops = 0
    filename = 'suppress_radio'
    num = 0x12
    val = (num << 24)

    N = 0
    check_bounds(N, 5)
    M = 0
    check_bounds(M, 17)
    val = val | (1 << 23)
    val |= (1 << 22)
    val |= (N << 17)
    val |= M
    set_trigger(trigger_dir, filename, val)

    ###################### 0x00 ##########################
    op_name = 'stop_operation'
    num = 0x00
    if(l[op_name]['generate_read_trigger']):
        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
        val = 0x00000000
        set_trigger(trigger_dir, filename, val)
    
    ###################### 0x01 ##########################
    op_name = 'alive_beacon'
    num = 0x01
    if(l[op_name]['generate_read_trigger']):
        option = l[op_name]['val']['safe_sleep_mode']
        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-safe_sleep_mode=' + format(option)
        val = (0x01 << 24)
        if(option):
            val |= (1 << 23)
        set_trigger(trigger_dir, filename, val)

    ###################### 0x02 ##########################
    op_name = 'packet_blaster'
    num = 0x02
    if(l[op_name]['generate_read_trigger']):
        N = l[op_name]['val']['radio_duration']
        check_bounds(N, 8)
        M = l[op_name]['val']['sleep_duration']
        check_bounds(M, 8)
        signal = l[op_name]['val']['signal']
        check_bounds(signal, 8)

        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-radio_duration={}-sleep_duration={}-signal=0x{}'.format(N, M, format(signal, 'x').zfill(2))

        val = (num << 24)
        val |= (N << 16)
        val |= (M << 8)
        val |= signal
        set_trigger(trigger_dir, filename, val)

    ###################### 0x03 ##########################
    op_name = 'characterization'
    num = 0x03
    if(l[op_name]['generate_read_trigger']):
        option = l[op_name]['val']['light_char']
        N = l[op_name]['val']['period']
        check_bounds(N, 16)

        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-light_char={}-period={}'.format(option, N)

        val = (num << 24)
        if(option):
            val |= (1 << 23)
        val |= N
        set_trigger(trigger_dir, filename, val)

    ###################### 0x05 ##########################
    op_name = 'battery_drain_test'
    num = 0x05
    if(l[op_name]['generate_read_trigger']):
        N = l[op_name]['val']['sleep_duration']
        check_bounds(N, 16)

        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-sleep_duration={}'.format(N)

        val = (num << 24)
        val |= N
        set_trigger(trigger_dir, filename, val)

    ###################### 0x06 ##########################
    op_name = 'timeout'
    num = 0x06
    if(l[op_name]['generate_read_trigger']):
        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
        val = (num << 24)
        set_trigger(trigger_dir, filename, val)

    ###################### 0x07 ##########################
    op_name = 'start_operation'
    num = 0x07
    if(l[op_name]['generate_read_trigger']):
        filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
        val = (num << 24)
        set_trigger(trigger_dir, filename, val)

    ###################### 0x01 ##########################
    op_name = 'CHIP_ID'
    num = 0x01
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)
    if(l[op_name]['write']):
        N = l[op_name]['val']
        check_bounds(N, 5)
        filename1 = filename + '-write-chip_id=0x{}'.format(format(N, 'x'))
        val1 = val | (1 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x08 ##########################
    op_name = 'light_huffman_code'
    num = 0x08
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 7);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(format(idx).zfill(2))
            val2 = val
            val2 |= (idx << 16)
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x09 ##########################
    op_name = 'temp_huffman_code'
    num = 0x09
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 7);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(format(idx).zfill(2))
            val2 = val
            val2 |= (idx << 16)
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0A ##########################
    op_name = 'epoch_days'
    num = 0x0A
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        if(l[op_name]['auto']):
            filename1 = filename + '-write-auto-date_gen'
            with open(out_dir + filename1 + '.bat', 'w') as f:
                f.write('python ../auto_date_gen.py {}\ncall write-auto-date.bat\n'.format(trigger_dir))
        else:
            d = l[op_name]['val']
            N = parse_date(d)
            check_bounds(N, 16)
            filename1 = filename + '-write-epoch_days_offset={}'.format(N)
            val1 = val | (1 << 23)
            val1 |= N
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0B ##########################
    op_name = 'xo_day_time_in_sec'
    num = 0x0B
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        if(l[op_name]['auto']):
            filename1 = filename + '-write-auto-time_gen'
            with open(out_dir + filename1 + '.bat', 'w') as f:
                f.write('python ../auto_time_gen.py {}\ncall write-auto-time.bat\n'.format(trigger_dir))
        else:
            H = l[op_name]['val']['H']
            check_bounds(H, 5)
            M = l[op_name]['val']['M']
            check_bounds(M, 6)
            filename1 = filename + '-write-H={}-M={}'.format(H, M)
            val1 = val | (1 << 23)
            val1 |= (H << 6)
            val1 |= M
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0C ##########################
    op_name = 'timer_constants'
    num = 0x0C
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['xo_to_sec_mplier']
        check_bounds(N, 16)
        filename1 = filename + '-write-xo_to_sec_mplier={}'.format(N)
        val1 = val | (1 << 21)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        N = l[op_name]['val']['xo_to_lnt_mplier']
        check_bounds(N, 16)
        filename1 = filename + '-write-xo_to_lnt_mplier=0x{}'.format(format(N, 'x'))
        val1 = val | (2 << 21)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        N = l[op_name]['val']['XO_TO_SEC_MPLIER_SHIFT']
        check_bounds(N, 16)
        filename1 = filename + '-write-XO_TO_SEC_MPLIER_SHIFT={}'.format(N)
        val1 = val | (3 << 21)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        N = l[op_name]['val']['LNT_MPLIER_SHIFT']
        check_bounds(N, 16)
        filename1 = filename + '-write-LNT_MPLIER_SHIFT={}'.format(N)
        val1 = val | (4 << 21)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0D ##########################
    op_name = 'sample_indices'
    num = 0x0D
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 2)
            if(counter != 3):
                check_bounds(num, 7)
                filename1 = filename + '-write_index={}-value={}'.format(counter, value)
                val1 = val | (1 << 23)
                val1 |= (counter << 21)
                val1 |= value
                set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 2);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (idx << 21)
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0E ##########################
    op_name = 'sunrise_sunset'
    num = 0x0E
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['cur_sunrise']
        check_bounds(N, 11)
        M = l[op_name]['val']['cur_sunset']
        check_bounds(M, 11)
        filename1 = filename + '-write-cur_sunrise={}-cur_sunset={}'.format(N, M)
        val1 = val | (1 << 23)
        val1 |= (N << 11)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0F ##########################
    op_name = 'sliding_window'
    num = 0x0F
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['EDGE_MARGIN1']
        check_bounds(N, 8)
        M = l[op_name]['val']['EDGE_MARGIN2']
        check_bounds(M, 8)
        filename1 = filename + '-write-EDGE_MARGIN1={}-EDGE_MARGIN2={}'.format(N, M)
        val1 = val | (1 << 22)
        val1 |= (N << 8)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

        M = l[op_name]['val']['MAX_EDGE_SHIFT_IN_MIN']
        check_bounds(M, 8)
        filename1 = filename + '-write-MAX_EDGE_SHIFT_IN_MIN={}'.format(M)
        val1 = val | (2 << 22)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x0F ##########################
    op_name = 'PMU_ACTIVE_SETTINGS'
    num = 0x10
    setting = 0
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 32)
            upper = value >> 16
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 1, format(upper, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (1 << 16)
            val1 |= upper
            set_trigger(trigger_dir, filename1, val1)

            lower = value & 0xFFFF
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 0, format(lower, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_RADIO_SETTINGS'
    num = 0x10
    setting = 1
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 32)
            upper = value >> 16
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 1, format(upper, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (1 << 16)
            val1 |= upper
            set_trigger(trigger_dir, filename1, val1)

            lower = value & 0xFFFF
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 0, format(lower, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_SLEEP_SETTINGS'
    num = 0x10
    setting = 2
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 32)
            upper = value >> 16
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 1, format(upper, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (1 << 16)
            val1 |= upper
            set_trigger(trigger_dir, filename1, val1)

            lower = value & 0xFFFF
            filename1 = filename + '-write_index={}-U={}-value=0x{}'.format(counter, 0, format(lower, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_ACTIVE_SAR_SETTINGS'
    num = 0x10
    setting = 3
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 8)

            lower = value
            filename1 = filename + '-write_index={}-value={}'.format(counter, lower)
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_RADIO_SAR_SETTINGS'
    num = 0x10
    setting = 4
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 8)

            lower = value
            filename1 = filename + '-write_index={}-value={}'.format(counter, lower)
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_SLEEP_SAR_SETTINGS'
    num = 0x10
    setting = 5
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 8)

            lower = value
            filename1 = filename + '-write_index={}-value={}'.format(counter, lower)
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_TEMP_THRESH'
    num = 0x10
    setting = 6
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 16)

            lower = value
            filename1 = filename + '-write_index={}-value=0x{}'.format(counter, format(lower, 'x').zfill(4))
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    op_name = 'PMU_ADC_THRESH'
    num = 0x10
    setting = 7
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for counter, value in enumerate(l[op_name]['val']):
            check_bounds(counter, 3)
            check_bounds(value, 8)

            lower = value
            filename1 = filename + '-write_index={}-value={}'.format(counter, lower)
            val1 = val | (1 << 23)
            val1 |= (setting << 20)
            val1 |= (counter << 17)
            val1 |= (0 << 16)
            val1 |= lower
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(idx)
            val2 = val
            val2 |= (setting << 20)
            val2 |= (idx << 17)
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x11 ##########################
    op_name = 'day_time_config'
    num = 0x11
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['DAY_START_TIME']
        check_bounds(N, 11)
        M = l[op_name]['val']['DAY_END_TIME']
        check_bounds(M, 11)
        filename1 = filename + '-write-DAY_START_TIME={}-DAY_END_TIME={}'.format(N, M)
        val1 = val | (1 << 23)
        val1 |= (N << 11)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x12 ##########################
    op_name = 'radio_config'
    num = 0x12
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['num_of_hops']
        check_bounds(N, 5)
        M = l[op_name]['val']['RADIO_PACKET_DELAY']
        check_bounds(M, 17)
        filename1 = filename + '-write-num_of_hops={}-RADIO_PACKET_DELAY={}'.format(N, M)
        val1 = val | (1 << 23)
        val1 |= (1 << 22)
        val1 |= (N << 17)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x13 ##########################
    op_name = 'light_threshold'
    num = 0x13
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['THRESHOLD_IDX_SHIFT']
        check_bounds(N, 5)
        M = l[op_name]['val']['EDGE_THRESHOLD']
        check_bounds(M, 16)
        filename1 = filename + '-write-THRESHOLD_IDX_SHIFT={}-EDGE_THRESHOLD={}'.format(N, M)
        val1 = val | (1 << 23)
        val1 |= (1 << 22)
        val1 |= (N << 17)
        val1 |= M
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x14 ##########################
    op_name = 'radio_config2'
    num = 0x14
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    for count, key in enumerate(l[op_name]['val']):
        check_bounds(count, 4)
        if(l[op_name]['write']):
            N = l[op_name]['val'][key]
            check_bounds(N, 20)
            filename1 = filename + '-write-{}={}'.format(key, N)
            val1 = val | ((count + 1) << 20)
            val1 |= N
            set_trigger(trigger_dir, filename1, val1)

        if(l[op_name]['generate_read_trigger']):
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
            val2 = val
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x15 ##########################
    op_name = 'debug_config'
    num = 0x15
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        radio_debug = l[op_name]['val']['radio_debug']
        if(radio_debug):
            N = l[op_name]['val']['RADIO_DEBUG_PERIOD']
            check_bounds(N, 16)
            filename1 = filename + '-write-radio_debug={}-RADIO_DEBUG_PERIOD={}'.format(True, N)
            val1 = val | (1 << 23)
            val1 |= (1 << 22)
            val1 |= N
            set_trigger(trigger_dir, filename1, val1)
        else:
            filename1 = filename + '-write-radio_debug={}'.format(False)
            val1 = val | (1 << 23)
            val1 |= (0 << 22)
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x16 ##########################
    op_name = 'radio_config3'
    num = 0x16
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['MRR_TEMP_THRESH_LOW']
        check_bounds(N, 22)
        filename1 = filename + '-write-MRR_TEMP_THRESH_LOW=0x{}'.format(format(N, 'x'))
        val1 = val | (1 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        N = l[op_name]['val']['MRR_TEMP_THRESH_HIGH']
        check_bounds(N, 22)
        filename1 = filename + '-write-MRR_TEMP_THRESH_HIGH=0x{}'.format(format(N, 'x'))
        val1 = val | (2 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        N = l[op_name]['val']['OVERRIDE_RAD']
        filename1 = filename + '-write-OVERRIDE_RAD={}'.format(N)
        val1 = val | (3 << 22)
        if N:
            val1 |= 1
        else:
            val1 |= 0
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x17 ##########################
    op_name = 'error_code'
    num = 0x17
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x18 ##########################
    op_name = 'addr_read'
    num = 0x18
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['generate_read_trigger']):
        length = l[op_name]['len']
        check_bounds(length, 8)
        N = l[op_name]['addr']
        check_bounds(N, 16)
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read-addr=0x{}-len={}'.format(format(N, 'x').zfill(4), length)
        val2 = val
        val2 |= (length << 16)
        val2 |= N
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x19 ##########################
    op_name = 'low_power_config'
    num = 0x19
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        for i in range(0, 7):
            N = l[op_name]['val']['LOW_PWR_LOW_ADC_THRESH'][i]
            check_bounds(N, 8)
            filename1 = filename + '-write-LOW_PWR_LOW_ADC_THRESH-index={}-value=0x{}'.format(i, format(N, 'x'))
            val1 = val | (1 << 22)
            val1 |= (i << 19)
            val1 |= N
            set_trigger(trigger_dir, filename1, val1)

            N = l[op_name]['val']['LOW_PWR_HIGH_ADC_THRESH'][i]
            check_bounds(N, 8)
            filename1 = filename + '-write-LOW_PWR_HIGH_ADC_THRESH-index={}-value=0x{}'.format(i, format(N, 'x'))
            val1 = val | (2 << 22)
            val1 |= (i << 19)
            val1 |= N
            set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        for idx in l[op_name]['read_indices']:
            check_bounds(idx, 3);
            filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read_index={}'.format(format(idx).zfill(2))
            val2 = val
            val2 |= (idx << 19)
            set_trigger(trigger_dir, filename2, val2)

    ###################### 0x1A ##########################
    op_name = 'mrr_coarse_cap_tune'
    num = 0x1A
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        N = l[op_name]['val']['MRR_CAP_TRX_ANTX_TUNE_COARSE']
        check_bounds(N, 10)
        filename1 = filename + '-write-MRR_CAP_TRX_ANTX_TUNE_COARSE={}'.format(N)
        val1 = val | (1 << 23)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x1B ##########################
    op_name = 'operation_config'
    num = 0x1B
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['write']):
        d = l[op_name]['val']['start_day_count']
        N = parse_date(d)
        check_bounds(N, 16)
        filename1 = filename + '-write-start_day_count={}'.format(N)
        val1 = val | (1 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        d = l[op_name]['val']['end_day_count']
        N = parse_date(d)
        check_bounds(N, 16)
        filename1 = filename + '-write-end_day_count={}'.format(N)
        val1 = val | (2 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

        d = l[op_name]['val']['radio_day_count']
        N = parse_date(d)
        check_bounds(N, 16)
        filename1 = filename + '-write-radio_day_count={}'.format(N)
        val1 = val | (3 << 22)
        val1 |= N
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read'
        val2 = val
        set_trigger(trigger_dir, filename2, val2)

    ###################### 0x1C ##########################
    op_name = 'radio_out_data'
    num = 0x1C
    filename = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name)
    val = (num << 24)

    if(l[op_name]['read_code_addr']):
        filename1 = filename + '-read-code_addr'
        val1 = val
        set_trigger(trigger_dir, filename1, val1)

    if(l[op_name]['generate_read_trigger']):
        N1 = l[op_name]['val']['start_unit_count']
        check_bounds(N1, 12)
        N2 = l[op_name]['val']['len']
        check_bounds(N2, 12)
        filename2 = 'GOC-0x{}-{}'.format(format(num, 'x').zfill(2).upper(), op_name) + '-read-start_unit_count={}-len={}'.format(N1, N2)
        val2 = val
        val2 |= (N1 << 12)
        val2 |= N2
        set_trigger(trigger_dir, filename2, val2)

# Generate master trigger
bat_names = glob.glob('{}*write*.bat'.format(out_dir))
bat_names.sort()
with open(out_dir + 'write_all.bat', 'w') as master_file:
    master_file.write('call suppress_radio.bat\n')
    master_file.write('timeout {}\n'.format(TIMEOUT_TIME))
    radio_config_bat = ''
    for bat_file in bat_names:
        if(re.match('.*GOC-0x12.*', bat_file)):
            radio_config_bat = bat_file
        else:
            master_file.write('call \"' + os.path.basename(bat_file) + '\"\n')
            master_file.write('timeout {}\n'.format(TIMEOUT_TIME))

    # add radio config 1 back in
    master_file.write('call \"' + os.path.basename(radio_config_bat) + '\"\n')
    master_file.write('timeout {}\n'.format(TIMEOUT_AFTER_RADIO))

# Generate one tap trigger
with open(out_dir + 'program-write_all-start_operation.bat', 'w') as out_file:
    out_file.write('call GOC_PROGRAM_mbc_code.bat\ncall write_all.bat\ncall GOC-0x07-start_operation.bat\n')

with open(out_dir + 'program-write_all.bat', 'w') as out_file:
    out_file.write('call GOC_PROGRAM_mbc_code.bat\ncall write_all.bat\n')
