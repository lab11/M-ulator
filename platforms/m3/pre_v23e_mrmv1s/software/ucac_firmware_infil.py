# -*- coding: utf-8 -*-
"""
Created on 09/22/2024

@author: Qirui Zhang

This script is for loading firmware into MRAM.
"""

import utils.ucac_ctrl as nc
from utils.ucac_ctrl import tmsIO as tms_ctrl
from utils.ucac_ctrl import spiIO as spi_ctrl
import time
import os
import sys
import json

print('')

'''
    Set uCAC main clock (clkb) frequency
'''
chip_no         = int(sys.argv[1])
spi_clkb_sel    = int(sys.argv[2])
spi_clkb_div    = int(sys.argv[3])
clkb_info_file = './clkb_info/clkb_info_chip' + str(chip_no) + '_s' + str(spi_clkb_sel) + 'd' + str(spi_clkb_div) + '.json'


'''
    scp uCAC firmware from EECS server to local directories
'''
print('Downloading uCAC firmware from EECS Server......')
src = "qiruizh@houghton.eecs.umich.edu:/n/houghton/z/qiruizh/SMARTePANTS/"
dst = "./"
os.system("scp -i ~\.ssh\id_rsa " + src + "binary/* " + dst + "binary/")
print('')


'''
    Generate MRAM timing configurations
'''
with open(clkb_info_file, 'r') as openfile:
    clkb_info_dict = json.load(openfile)

half_clk_period = clkb_info_dict['clkb_period']*1e9/2
R_TMC_twrs_out, R_TMC_tw2r_out, R_TMC_tprog_out, R_TMC_tcycrd_out, R_TMC_twk_out, R_TMC_trds_out,\
R_TMC_tcycrd1_out, R_TMC_trdl_out, R_TMC_rp_otp_tpg_out, R_TMC_rap_otp_tpg_out, R_TMC_rbd_otp_tpg_out = nc.mram_timing_info(half_clk_period)
print('')


'''
    Initialize GPIO bridge boards
'''
print("Initializing GPIO Boards......")
print('SPI FT232H Address is: ', nc.gpio_spi_addr)
print('JTAG FT232H Address is: ', nc.gpio_jtag_addr)

gpio_spi = spi_ctrl(nc.gpio_spi_addr, 1e7)

gpio_tms = tms_ctrl(nc.gpio_jtag_addr, 1e6)
gpio_tms.set_tms(0b1)

print('')


'''
    Chip Power-Up Sequence
'''
print('Power-up uCAC!')

# Turn-on VDD_3P3
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-on VDD_3P3 (Typ. 3.3V): enter yes/no to finish.\n")

# Turn-on VDD_1P8
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-on VDD_1P8 (Typ. 1.8V): enter yes/no to finish.\n")

# Turn-on VDD_0P8
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-on VDD_0P8 (Typ. 0.8V): enter yes/no to finish.\n")

print('')


'''
    Config spi_boot_sel to 1'b1
'''
time.sleep(0.5)
print("Host takes control of uCAC!")

print("Info: Up-stream SPIFI transaction - spi_boot_sel set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_boot_sel'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_boot_sel rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_boot_sel']))

print('')


'''
    Set TMS to 1'b0
'''
time.sleep(0.5)
gpio_tms.set_tms(0b0)




''' 
    Initialize MRAM
'''
time.sleep(0.5)
print("Power-on MRAM!")

# PDMIO set to high
print("Info: Up-stream SPIFI transaction - mram_pdmio set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_pdmio rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio']))

# Power-on VDIO
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdio_pgen_small set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen_small'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_vdio_pgen_small rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen_small']))
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdio_pgen set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_vdio_pgen rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen']))

# Power-on VDD
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdd_pgen_small set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen_small'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_vdd_pgen_small rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen_small']))
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdd_pgen set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_vdd_pgen rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen']))

# PDMIO set to low
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_pdmio set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_pdmio rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio']))

# Release MRAM isolation
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_isoln set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_isoln'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_isoln rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_isoln']))

print('')

# Config Clock B speed
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - clkb_sel set to ", spi_clkb_sel, ", clk_div set to ", spi_clkb_div)
gpio_spi.ucac_clkb_cfg(spi_clkb_sel, spi_clkb_div)

# Enable Clock B
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - clkb_en set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clkb_en'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_clkb_en rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clkb_en']))

print("Info: Up-stream SPIFI transaction - clkb_rstn set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clkb_rstn'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_clkb_rstn rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clkb_rstn']))

# Enable Clock Output
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - clko_sel set to 2'b10")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clko_sel'], 0b10)
print("Info: Up-stream SPIFI transaction - spi_clko_sel rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clko_sel']))

print("Info: Up-stream SPIFI transaction - clko_en set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clko_en'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_clko_en rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clko_en']))

# Check Clock Output
opened = 'no'
while opened != 'yes':
    opened = input("Ready to proceed?\n")

# Disable Clock Output
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - clko_en set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clko_en'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_clko_en rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clko_en']))

print("Info: Up-stream SPIFI transaction - clko_sel set to 2'b00")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_clko_sel'], 0b00)
print("Info: Up-stream SPIFI transaction - spi_clko_sel rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_clko_sel']))

print('')

# Change SPI MRAM TCYCRD according to uCAC clkb freq.
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - SPI_MRAM_TCYCRD")
write_buf = gpio_spi.SPI_CMD['SPI_MRAM_TCYCRD']
write_buf += R_TMC_tcycrd_out.to_bytes(1, "big")
write_buf += b'\x00'
gpio_spi.spi_exchange(write_buf)

# Check if MRAM busy is properly 1'b1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_busy rdata is ", gpio_spi.read_mram_busy())

# Release MRAM reset 
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_rstn set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_rstn'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_rstn rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_rstn']))

# Release MRAM clock
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_clk_en set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_clk_en'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_clk_en rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_clk_en']))

# Wait for MRAM busy to go low
gpio_spi.mram_wait_busy()

print('')

''' 
    Configure MRAM registers before waking it up
'''
time.sleep(0.5)
print("Enter MRAM test mode and configure MRAM registers!")

# Enter Test Mode
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to enter test mode")
gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, nc.MRAM_TEST_PW.to_bytes(4, 'big'))

# Enable MRAM ECC
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM_R_OP to enable ECC")
gpio_spi.mram_wrcfg(nc.MRAM_R_OP, nc.MRAM_R_OP_DV.to_bytes(4, 'big'))
print("Info: Up-stream SPIFI transaction - MRAM_R_OP rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_OP))

# Configure MRAM TIMER0
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_TIMER0")
wdata = R_TMC_tprog_out.to_bytes(1, "big") + (19).to_bytes(1, "big") + R_TMC_tw2r_out.to_bytes(1, "big") + R_TMC_twrs_out.to_bytes(1, "big")
gpio_spi.mram_wrcfg(nc.MRAM_R_TIMER0, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_TIMER0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_TIMER0))

# Configure MRAM TIMER1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_TIMER1")
wdata = int(f'{R_TMC_trdl_out:07b}' + f'{R_TMC_tcycrd1_out:04b}' + '1' + f'{R_TMC_trds_out:08b}' + f'{R_TMC_twk_out:08b}' + f'{R_TMC_tcycrd_out:04b}', 2).to_bytes(4, 'big')
gpio_spi.mram_wrcfg(nc.MRAM_R_TIMER1, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_TIMER1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_TIMER1))

# Configure MRAM TIMER1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_OTP_TPG")
wdata = int(13*'0' + f'{R_TMC_rbd_otp_tpg_out:07b}' + f'{R_TMC_rap_otp_tpg_out:06b}' + f'{R_TMC_rp_otp_tpg_out:06b}', 2).to_bytes(4, 'big')
gpio_spi.mram_wrcfg(nc.MRAM_R_OTP_TPG, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_OTP_TPG rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_OTP_TPG))

# Exit Test Mode
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to exit test mode")
gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, (0).to_bytes(4, 'big'))

print('')

'''
    Auto Wake-up MRAM
    ATTENTION: VBL_2P2 must not be 2.2V when waking-up MRAM. It can be either GND or floating.
'''
time.sleep(0.5)
print("Auto Wake-up MRAM!")
print("Info: Up-stream SPIFI transaction - SPI_MRAM_AUTOWKUP")
write_buf = gpio_spi.SPI_CMD['SPI_MRAM_AUTOWKUP']
write_buf += b'\x00'
gpio_spi.spi_exchange(write_buf)

# Wait for MRAM busy to go low
gpio_spi.mram_wait_busy()

print('')




'''
    Enter MRAM test mode and set MRAM CFG registers back to default
'''
time.sleep(0.5)
print("Enter MRAM test mode and set MRAM CFG registers back to default!")

# Enter Test Mode
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to enter test mode")
gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, nc.MRAM_TEST_PW.to_bytes(4, 'big'))

# MRAM_R_4PHASE_WRITE0
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_4PHASE_WRITE0")
gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_WRITE0, (0).to_bytes(4, 'big'))
print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_WRITE0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_WRITE0))

# MRAM_R_4PHASE_WRITE1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_4PHASE_WRITE1")
gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_WRITE1, (0).to_bytes(4, 'big'))
print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_WRITE1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_WRITE1))

# MRAM_R_4PHASE_MRG0
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_4PHASE_MRG0")
wdata = int('0000100000' + 7*'0' + '000' + '100101' + '100', 2).to_bytes(4, 'big')
gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_MRG0, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_MRG0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_MRG0))

# MRAM_R_4PHASE_MRG1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_4PHASE_MRG1")
wdata = int('0000011000' + 7*'0' + '000' + '000000' + '000', 2).to_bytes(4, 'big')
gpio_spi.mram_wrcfg(nc.MRAM_R_4PHASE_MRG1, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_4PHASE_MRG1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_4PHASE_MRG1))

# MRAM_R_CFG0
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_CFG0")
wdata = b'\xa0\x54\x21\x08'
gpio_spi.mram_wrcfg(nc.MRAM_R_CFG0, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_CFG0 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG0))

# MRAM_R_CFG1
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_CFG1")
wdata = b'\x00\x2a\x1e\x2e'
gpio_spi.mram_wrcfg(nc.MRAM_R_CFG1, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_CFG1 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG1))

# MRAM_R_CFG5
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM register MRAM_R_CFG5")
wdata = b'\x0c\x00\x8d\xd3'
gpio_spi.mram_wrcfg(nc.MRAM_R_CFG5, wdata)
print("Info: Up-stream SPIFI transaction - MRAM_R_CFG5 rdata is ", gpio_spi.mram_rdcfg(nc.MRAM_R_CFG5))

# Exit Test Mode
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - write MRAM_R_TEST to exit test mode")
gpio_spi.mram_wrcfg(nc.MRAM_R_TEST, (0).to_bytes(4, 'big'))

print('')

'''
    Write data into MRAM
'''
time.sleep(0.5)
print("Writing data into MRAM......")

# Get file length
mram_file = open('./binary/mram_firmware.hex', 'r')
file_len = sum(1 for line in mram_file)
mram_file.close()

# Write MRAM
mram_file = open('./binary/mram_firmware.hex', 'r')
for addr in range(file_len):
    wdata = mram_file.readline().replace('\n', '')
    # First Load
    gpio_spi.mram_load(addr, wdata)
    # Then Write
    if ((addr + 1) % 8 == 0) or (addr == (file_len - 1)):
        gpio_spi.mram_write(addr)
    # print("MRAM word #" + str(addr) + " is written!")    
mram_file.close()

# Read MRAM
print("Reading data from MRAM to check integrity......")
mram_file = open('./binary/mram_firmware.hex', 'r')
ber_cnt = 0
for addr in range(file_len):
    # Read a word
    rdata_hex = gpio_spi.mram_read(addr)
    rdata_bin = f'{int(rdata_hex, 16):0128b}'
    # print("MRAM word read from addr " + str(addr) + ": " + rdata_hex)
    # Check agains golden brick
    rdata_gb = mram_file.readline().replace('\n', '')
    rdata_gb = f'{int(rdata_gb, 16):0128b}'
    if rdata_gb != rdata_bin:
        print("Bit error happens at addr " + str(addr) + "!")
        for b in range(128):
            if rdata_bin[b] != rdata_gb[b]:
                ber_cnt += 1
mram_file.close()

if ber_cnt != 0:
    print('Some bits are not loaded properly. Please repeat!')

print('')




'''
    Power-off MRAM
'''
time.sleep(0.5)
print("Power-off MRAM!")

# Disable MRAM clock
print("Info: Up-stream SPIFI transaction - mram_clk_en set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_clk_en'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_clk_en rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_clk_en']))

# Reset MRAM
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_rstn set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_rstn'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_rstn rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_rstn']))

# Isolate MRAM
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_isoln set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_isoln'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_isoln rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_isoln']))

# PDMIO set to high
print("Info: Up-stream SPIFI transaction - mram_pdmio set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_pdmio rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio']))

# Power-off VDD
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdd_pgen set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_vdd_pgen rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen']))
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdd_pgen_small set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen_small'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_vdd_pgen_small rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdd_pgen_small']))

# Power-off VDIO
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdio_pgen set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_vdio_pgen rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen']))
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_vdio_pgen_small set to 1'b1")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen_small'], 0b1)
print("Info: Up-stream SPIFI transaction - spi_mram_vdio_pgen_small rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_vdio_pgen_small']))

# PDMIO set to low
time.sleep(0.1)
print("Info: Up-stream SPIFI transaction - mram_pdmio set to 1'b0")
gpio_spi.ucac_wrcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio'], 0b0)
print("Info: Up-stream SPIFI transaction - spi_mram_pdmio rdata is ", gpio_spi.ucac_rdcfg(gpio_spi.uCAC_CFG['spi_mram_pdmio']))

print('')




'''
    Chip Power-off Sequence
'''
print('Power-off uCAC!')

# Turn-off VDD_0P8
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-off VDD_0P8: enter yes/no to finish.\n")

# Turn-off VDD_1P8
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-off VDD_1P8: enter yes/no to finish.\n")

# Turn-off VDD_3P3
opened = 'no'
while opened != 'yes':
    opened = input("Please turn-off VDD_3P3: enter yes/no to finish.\n")

print('')


'''
    Close GPIO Drivers
'''
gpio_tms.close()
gpio_spi.close()
