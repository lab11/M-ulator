#!/usr/bin/env perl

use warnings;
use POSIX;
use MAC_sub;
use MAC_gen_weight;
use MOV_sub;

##### Initial: DNN_INIT_MEM_*.data
##### Final: layer_0_MEM_*.data 

########################
#### layer 0
my $inst = "DNN_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "dnn_0.weights";
my $input_filename = "dnn.inputs";
my $offset_filename = "dnn_0.offsets";
my $import_input_file = 0b0001;
my $PE_on = 0b1111;        
my $MAC_OFFSET = 0b1111; 
my $I_MEM_START_BASE = 2450;
my $W_MEM_START_BASE = 0;
my $OS_MEM_START_BASE = 3100;
my @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight(@argument);
#system("cp DNN_INIT_MEM_0.data temp0_DNN_INIT_MEM_0.data");
print "0\n";

#### layer 1
$import_input_file = 0b0000;
$INPUT_MEM_FILE = "DNN_INIT_MEM";
$weight_filename = join "", "dnn_1.weights";
$offset_filename = join "", "dnn_1.offsets";
#  $I_MEM_START_BASE = 5632;  ## doesn't matter
$W_MEM_START_BASE = 2496;
$OS_MEM_START_BASE = 5388;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE, 
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_gen_weight(@argument);
#system("cp DNN_INIT_MEM_0.data temp1_DNN_INIT_MEM_0.data");
print "1\n";

#### layer 2
$import_input_file = 0b0000;
$PE_on = 0b0001;
$INPUT_MEM_FILE = "DNN_INIT_MEM";
$weight_filename = join "", "dnn_2.weights";
$offset_filename = join "", "dnn_2.offsets";
#  $I_MEM_START_BASE = 5632;  ## doesn't matter
$W_MEM_START_BASE = 4800;
$OS_MEM_START_BASE = 5400;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $offset_filename, 
                $import_input_file, $PE_on, $MAC_OFFSET, "",  
                $I_MEM_START_BASE, $W_MEM_START_BASE, $OS_MEM_START_BASE);
MAC_gen_weight(@argument);
#exit(0);

########################
my $ii;
my $original;
my $copy;
system("rm dnn.inst.c");
#### MOV0_0
$inst = "MOV_0_0";
@PE_I_MEM_START = 2450;
@PE_O_MEM_START = 2450;
my $input_size = 51;
@argument = ($inst, $INPUT_MEM_FILE, 0b0001, 1, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size); 
MOV_sub(@argument);
#### MOV0_1
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_0_1";
@argument = ($inst, $INPUT_MEM_FILE, 0b0010, 0, 2, 0, 0, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size); 
MOV_sub(@argument);
#### MOV0_2
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_0_2";
@argument = ($inst, $INPUT_MEM_FILE, 0b0100, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size); 
MOV_sub(@argument);
#### layer 0
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "layer_0";
$weight_filename = "dnn_0.weights";
$input_filename = "dnn.inputs";
my $output_filename = "dnn_0.outputs";
$offset_filename = "dnn_0.offsets";
my $nli_filename = "dnn.nli";
$PE_on = 0b1111;       
$MAC_OFFSET = 0b1111; 
my $MAC_NLI = 0b0000; 
my $O_SIGN = 0b0000; 
$TO_MEM_START_BASE = 2550;
$I_MEM_START_BASE = 2450;
@O_MEM_START_BASE = (3072, 3072, 3072, 3072);
$W_MEM_START_BASE = 0;
$OS_MEM_START_BASE = 3100;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_sub(@argument);
###############

#exit(0);
$input_size = 12;
#### MOV1_0
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_1_0";
@PE_I_MEM_START = (5504, 5516, 5528, 5540);
@PE_O_MEM_START = (5504, 5516, 5528, 5540);
@argument = ($inst, $INPUT_MEM_FILE, 0b1111, 1, 2, 3, 0, 
             $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size,
             $PE_I_MEM_START[1], $PE_O_MEM_START[1], $input_size,
             $PE_I_MEM_START[2], $PE_O_MEM_START[2], $input_size,
             $PE_I_MEM_START[3], $PE_O_MEM_START[3], $input_size); 
MOV_sub(@argument);
#### MOV1_1
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_1_1";
@argument = ($inst, $INPUT_MEM_FILE, 0b1111, 2, 3, 0, 1,
             $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size,
             $PE_I_MEM_START[1], $PE_O_MEM_START[1], $input_size,
             $PE_I_MEM_START[2], $PE_O_MEM_START[2], $input_size,
             $PE_I_MEM_START[3], $PE_O_MEM_START[3], $input_size); 
MOV_sub(@argument);
#### MOV1_2
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_1_2";
@argument = ($inst, $INPUT_MEM_FILE, 0b1111, 3, 0, 1, 2, 
             $PE_I_MEM_START[0], $PE_O_MEM_START[0], $input_size,
             $PE_I_MEM_START[1], $PE_O_MEM_START[1], $input_size,
             $PE_I_MEM_START[2], $PE_O_MEM_START[2], $input_size,
             $PE_I_MEM_START[3], $PE_O_MEM_START[3], $input_size); 
MOV_sub(@argument);
#### layer 1
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "layer_1";
$weight_filename = "dnn_1.weights";
$input_filename = "dnn_0.outputs";
$output_filename = "dnn_1.outputs";
$offset_filename = "dnn_1.offsets";
$TO_MEM_START_BASE = 5728;
$I_MEM_START_BASE = 5504;
@O_MEM_START_BASE = (5632, 5644, 5656, 5668);
$W_MEM_START_BASE = 2496;
$OS_MEM_START_BASE = 5388;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[1], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[2], $W_MEM_START_BASE, $OS_MEM_START_BASE, 
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[3], $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_sub(@argument);
###############

$input_size = 12;
#### MOV2_0
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_2_0";
@PE_I_MEM_START = (5632, 5644, 5656, 5668);
@PE_O_MEM_START = (5632, 5644, 5656, 5668);
@argument = ($inst, $INPUT_MEM_FILE, 0b0010, 0, 0, 0, 0, 
             0, 0, 0,
             $PE_I_MEM_START[1], $PE_O_MEM_START[1], $input_size,
             0, 0, 0, 
             0, 0, 0); 
MOV_sub(@argument);
#### MOV2_1
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_2_1";
@argument = ($inst, $INPUT_MEM_FILE, 0b0100, 0, 0, 0, 0,
             0, 0, 0, 
             0, 0, 0, 
             $PE_I_MEM_START[2], $PE_O_MEM_START[2], $input_size,
             0, 0, 0); 
MOV_sub(@argument);
#### MOV2_2
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "MOV_2_2";
@argument = ($inst, $INPUT_MEM_FILE, 0b1000, 0, 0, 0, 0, 
             0, 0, 0, 
             0, 0, 0, 
             0, 0, 0, 
             $PE_I_MEM_START[3], $PE_O_MEM_START[3], $input_size); 
MOV_sub(@argument);
#### layer 2
$INPUT_MEM_FILE = $inst . "_MEM";
$inst = "layer_2";
$weight_filename = "dnn_2.weights";
$input_filename = "dnn_1.outputs";
$output_filename = "dnn_2.outputs";
$offset_filename = "dnn_2.offsets";
$PE_on = 0b0001;       
$O_SIGN = 0b1111; 
$TO_MEM_START_BASE = 5696;
$I_MEM_START_BASE = 5632;
@O_MEM_START_BASE = (5504);
$W_MEM_START_BASE = 4800;
$OS_MEM_START_BASE = 5400;
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $offset_filename, $nli_filename, 
             $PE_on, $MAC_OFFSET, $MAC_NLI, $O_SIGN,  
             $TO_MEM_START_BASE, $I_MEM_START_BASE, $O_MEM_START_BASE[0], $W_MEM_START_BASE, $OS_MEM_START_BASE); 
MAC_sub(@argument);
