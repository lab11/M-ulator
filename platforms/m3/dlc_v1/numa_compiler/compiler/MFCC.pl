#!/usr/bin/env perl

use warnings;
use POSIX;
use MAC_sub;
use MAC_gen_weight;
use MOV_sub;

########################
#### layer 0
my $inst = "MFCC_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "mfcc.weights";
my $input_filename = "mfcc.inputs";
my $offset_filename = "mfcc.offsets";
my $import_input_file = 0b0010;
my $PE_on = 0b0110;        
my $MAC_OFFSET = 0b0000; 
my $I_MEM_START_BASE = 5632;    
my $W_MEM_START_BASE = 0;       ## TODO 
my $OS_MEM_START_BASE = 0; 
my @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight(@argument);
#system("cp MFCC_INIT_MEM_0.data temp0_MFCC_INIT_MEM_0.data");
print "0\n";
########################
my $ii;
my $original;
my $copy;
system("rm mfcc.inst.c");
#### MOV0
$inst = "MFCC_MOV_0";
@PE_I_MEM_START = 5632;
@PE_O_MEM_START = 5632;
my $input_size = 51;                    ## TODO
@argument = ($inst, $INPUT_MEM_FILE, 0b0010, 0, 2, 0, 0, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size); 
MOV_sub(@argument);
#### MAC
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MFCC_MAC";
$weight_filename = "mfcc.weights";
$input_filename = "mfcc.inputs";
my $output_filename = "mfcc.outputs";
$offset_filename = "mfcc.offsets";
my $nli_filename = "dnn.nli";
$PE_on = 0b0110;       
$MAC_OFFSET = 0b0000; 
my $MAC_NLI = 0b0000; 
my $O_SIGN = 0b0110; 
$TO_MEM_START_BASE = 5696;
$I_MEM_START_BASE = 5632;
@O_MEM_START_BASE = (0, 5516, 5528, 0);   ## TODO
$W_MEM_START_BASE = 0;                    ## TODO
$OS_MEM_START_BASE = 5376;                ## TODO
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_sub(@argument);
