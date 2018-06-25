#!/usr/bin/env perl

use warnings;
use POSIX;
use MAC_sub;
use MAC_gen_weight;
use MOV_sub;

### Three instructions:
### MOV input 
### MOV offset 
### MAC
### Initial memory footprint: MOV_MAC_small_INIT_MEM_*.data
### Final result footprint: MAC_small_MEM_*.data
### Intermediate memory footprint: MOV_0_*.data, MOV__1_*.data
### memory addresses can be changed in the "configurations" section

##########
######## memory constants -- do not modify
my $MEM_H4_START 	= 0;
my $MEM_H3_START 	= 4096;
my $MEM_H2_START 	= 5120;
my $MEM_H1_START 	= 5632;
my $MEM_O_BOUND 	= 5760;
my $MEM_OUT_BOUND 	= 5760;

my $MEM_H4_LINESpBANK 	= 1024;
my $MEM_H3_LINESpBANK 	= 256;
my $MEM_H2_LINESpBANK 	= 128;
my $MEM_H1_LINESpBANK 	= 32;

my $MEM_H4B1_START	= $MEM_H4_START + 0*$MEM_H4_LINESpBANK;
my $MEM_H4B2_START	= $MEM_H4_START + 1*$MEM_H4_LINESpBANK;
my $MEM_H4B3_START	= $MEM_H4_START + 2*$MEM_H4_LINESpBANK;
my $MEM_H4B4_START	= $MEM_H4_START + 3*$MEM_H4_LINESpBANK;

my $MEM_H3B1_START	= $MEM_H3_START + 0*$MEM_H3_LINESpBANK;
my $MEM_H3B2_START	= $MEM_H3_START + 1*$MEM_H3_LINESpBANK;
my $MEM_H3B3_START	= $MEM_H3_START + 2*$MEM_H3_LINESpBANK;
my $MEM_H3B4_START	= $MEM_H3_START + 3*$MEM_H3_LINESpBANK;

my $MEM_H2B1_START	= $MEM_H2_START + 0*$MEM_H2_LINESpBANK;
my $MEM_H2B2_START	= $MEM_H2_START + 1*$MEM_H2_LINESpBANK;
my $MEM_H2B3_START	= $MEM_H2_START + 2*$MEM_H2_LINESpBANK;
my $MEM_H2B4_START	= $MEM_H2_START + 3*$MEM_H2_LINESpBANK;

my $MEM_H1B1_START	= $MEM_H1_START + 0*$MEM_H1_LINESpBANK;
my $MEM_H1B2_START	= $MEM_H1_START + 1*$MEM_H1_LINESpBANK;
my $MEM_H1B3_START	= $MEM_H1_START + 2*$MEM_H1_LINESpBANK;
my $MEM_H1B4_START	= $MEM_H1_START + 3*$MEM_H1_LINESpBANK;
#########
######## configurations
my $input_size = 24;  ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
my $output_size = 24; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
my $input_prec = 8;   ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
my $output_prec = 12; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
my $weight_prec = 6;  ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
my $tempout_prec = 32; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL
#
# my $input_addr = $MEM_H1B2_START;
# my $output_addr = $MEM_H1B3_START;
# my $weight_addr = $MEM_H4B1_START;
# my $offset_addr = $MEM_H1B4_START;
# my $tempout_addr = $MEM_H1B1_START;

# my $input_size = 16;  ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
# my $output_size = 16; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
# my $input_prec = 6;   ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
# my $output_prec = 16; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
# my $weight_prec = 6;  ## FIXED -- NEED TO CHANGE ./test.sh AS WELL 
# my $tempout_prec = 32; ## FIXED -- NEED TO CHANGE ./test.sh AS WELL

# my $input_addr = $MEM_H3B1_START;
# my $output_addr = $MEM_H1B1_START;
# my $weight_addr = $MEM_H4B1_START;
# my $offset_addr = $MEM_H1B4_START;
# my $tempout_addr = $MEM_H2B1_START;

my $input_addr = $MEM_H3B1_START;
my $output_addr = $MEM_H1B1_START;
my $weight_addr = $MEM_H4B1_START;
my $offset_addr = $MEM_H1B4_START;
my $tempout_addr = $MEM_H2B1_START;
#################################
#
#################################
#### INIT
my $inst = "MOV_MAC_small_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "dnn.weights";
my $input_filename = "dnn.inputs";
my $offset_filename = "dnn.offsets";
my $import_input_file = 0b0001;
my $PE_on = 0b0001;
my $MAC_OFFSET = 0b0001; 
my $weight_mem_size = ceil($input_size * $output_size / (96 / $weight_prec));
my $input_mem_size = ceil($input_size / (96 / $input_prec));
my $output_mem_size = ceil($output_size / (96 / $output_prec));
my $tempout_mem_size = ceil($output_size / (96 / $tempout_prec));

my $W_MEM_START_BASE = $weight_addr; 
my $I_MEM_START_BASE = $W_MEM_START_BASE + $weight_mem_size;
my $OS_MEM_START_BASE = $I_MEM_START_BASE + $input_mem_size;
my $O_MEM_START_BASE = $OS_MEM_START_BASE + $output_mem_size;
my $TO_MEM_START_BASE = $O_MEM_START_BASE + $output_mem_size + $tempout_mem_size;
my @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight(@argument);
print "0\n";

########################
system("rm dnn.inst.c");
# #### MOV0: mov inputs
# $INPUT_MEM_FILE = $inst . "_MEM";
# $inst = "MOV_0";
# @PE_I_MEM_START = $I_MEM_START_BASE;
# @PE_O_MEM_START = $input_addr;
# @argument = ($inst, $INPUT_MEM_FILE, 0b0001, 0, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_mem_size); 
# MOV_sub(@argument);
# print "1\n";
# #### MOV1: mov offsets
# $INPUT_MEM_FILE = $inst . "_MEM";
# $inst = "MOV_1";
# @PE_I_MEM_START = $OS_MEM_START_BASE;
# @PE_O_MEM_START = $offset_addr;
# @argument = ($inst, $INPUT_MEM_FILE, 0b0001, 0, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $output_mem_size); 
# MOV_sub(@argument);
# print "2\n";
#### MAC
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MAC_small";
my $output_filename = "dnn_0.outputs";
my $nli_filename = "dnn.nli";
$PE_on = 0b0001;
$MAC_OFFSET = 0b0001; 
my $MAC_NLI = 0b0000; 
my $O_SIGN = 0b0000; 
# $TO_MEM_START_BASE = $tempout_addr;
# $I_MEM_START_BASE = $input_addr;
# @O_MEM_START_BASE = ($output_addr, 0, 0, 0);
# $W_MEM_START_BASE = $weight_addr;
# $OS_MEM_START_BASE = $offset_addr;
# @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             # $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             # $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             # $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             # $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             # $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE); 
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_sub(@argument);
print "done\n";
###############

