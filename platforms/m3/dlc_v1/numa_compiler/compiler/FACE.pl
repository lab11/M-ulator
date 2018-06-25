#!/usr/bin/env perl

use warnings;
use POSIX;
use MAC_sub_face;
use MAC_sub_face_M1;
use MAC_sub_face_M2;
use MAC_gen_weight_face;
use MAC_gen_weight_face_M1;
use MAC_gen_weight_face_M2;
use MOV_sub;

##### Initial: FACE_INIT_MEM_*.data
##### Final: layer_0_MEM_*.data 

########################
#### layer 0
my $inst = "FACE_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "face_0.weights";
my $input_filename = "face.inputs";
my $offset_filename = "face_0.offsets";
my $import_input_file = 0b0001;
my $PE_on = 0b0001;        
my $MAC_OFFSET = 0b0001; 
my $I_MEM_START_BASE = 5120;
my $W_MEM_START_BASE = 0;
my $OS_MEM_START_BASE = 2064;


########################
#### layer 0
my $inst = "FACE_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "face_0.weights";
my $input_filename = "face.inputs";
my $offset_filename = "face_0.offsets";
my $import_input_file = 0b0001;
my $PE_on = 0b0001;        
my $MAC_OFFSET = 0b0001; 
my $I_MEM_START_BASE = 5120;
my $W_MEM_START_BASE = 0;
my $OS_MEM_START_BASE = 2064;
my @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight_face_M1(@argument);
#system("cp DNN_INIT_MEM_0.data temp0_DNN_INIT_MEM_0.data");
print "0\n";

#### layer 1
#$import_input_file = 0b0000;
$INPUT_MEM_FILE = "FACE_INIT_MEM";
$weight_filename = join "", "face_1.weights";
#$input_filename = "face_0.outputs";
$input_filename = "face.inputs";
$offset_filename = join "", "face_1.offsets";
$import_input_file = 0b0001;
$PE_on = 0b0001;        
$MAC_OFFSET = 0b0001; 
$I_MEM_START_BASE = 5634;
$W_MEM_START_BASE = 5728;
$OS_MEM_START_BASE = 5668;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight_face_M2(@argument);
#system("cp DNN_INIT_MEM_0.data temp1_DNN_INIT_MEM_0.data");
print "1\n";


########################
########################
my $ii;
my $original;
my $copy;
system("rm dnn.inst.c");

#### layer 0
$INPUT_MEM_FILE = $inst . "_MEM";
#$INPUT_MEM_FILE = "NULL";
$inst = "layer_0";
$weight_filename = "face_0.weights";
$input_filename = "face.inputs";
my $output_filename = "face_0.outputs";
$offset_filename = "face_0.offsets";
my $nli_filename = "dnn.nli";
$PE_on = 0b0001;       
$MAC_OFFSET = 0b0001; 
my $MAC_NLI = 0b0000; 
my $O_SIGN = 0b0001; 
$TO_MEM_START_BASE = 5696;
$I_MEM_START_BASE = 5120;
@O_MEM_START_BASE = (5634, 5516, 5528, 5540);
$W_MEM_START_BASE = 0;
$OS_MEM_START_BASE = 2064;
my $PE_SHIFT = 3;
my $PE_SHIFT_OS = 2;
my $PE_SHIFT_NL = 0;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE,
			 $PE_SHIFT, $PE_SHIFT_OS,$PE_SHIFT_NL); 
MAC_sub_face_M1(@argument);
###############


#### layer 1
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "layer_1";
$weight_filename = "face_1.weights";
$input_filename = "face_0.outputs";
$output_filename = "face_1.outputs";
$offset_filename = "face_1.offsets";
$TO_MEM_START_BASE = 5706;
$I_MEM_START_BASE = 5634;
@O_MEM_START_BASE = (5632, 5644, 5656, 5668);
$W_MEM_START_BASE = 5728;
$OS_MEM_START_BASE = 5668;
$PE_SHIFT = 6;
$PE_SHIFT_OS = 0;
$PE_SHIFT_NL = 0;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE,
			 $PE_SHIFT, $PE_SHIFT_OS,$PE_SHIFT_NL); 
MAC_sub_face_M2(@argument);
###############



