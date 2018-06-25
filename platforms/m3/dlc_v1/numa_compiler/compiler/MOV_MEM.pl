#!/usr/bin/env perl

use warnings;
use POSIX;
use MOV_sub;

### three instructions in three stages:
### PE2->PE0
### PE2->PE1
### PE2->PE3
### Initial memory footprint: MOV_INIT_MEM_*.data
### Final result footprint: MOV_2_MEM_*.data
### Intermediate memory footprint: MOV_MEM_0_*.data, MOV_MEM_1_*.data
### data size, memory addresses can be changed in the "configurations" section

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
##########
######## configurations
my $input_size = 5760; ## number of word lines to copy; careful if larger than 32

#my $MOV0_PE0_source_addr = $MEM_H1B1_START;
#my $MOV0_PE0_dest_addr = $MEM_H2B1_START; ## also MOV1_PE0_source addr
#my $MOV1_PE0_dest_addr = $MEM_H3B1_START; ## also MOV2_PE0_source addr
#my $MOV2_PE0_dest_addr = $MEM_H4B1_START;
#
#my $MOV0_PE1_source_addr = $MEM_H1B1_START;
#my $MOV0_PE1_dest_addr = $MEM_H2B1_START; ## also MOV1_PE2_source addr
#my $MOV1_PE2_dest_addr = $MEM_H3B1_START; ## also MOV2_PE3_source addr
#my $MOV2_PE3_dest_addr = $MEM_H4B1_START;
#
 my $MOV0_PE0_source_addr = $MEM_H4B1_START;
 my $MOV0_PE0_dest_addr = $MEM_H4B2_START; ## also MOV1_PE0_source addr
 my $MOV1_PE0_dest_addr = $MEM_H4B3_START; ## also MOV2_PE0_source addr
 my $MOV2_PE0_dest_addr = $MEM_H4B4_START;

 my $MOV0_PE1_source_addr = $MEM_H4B1_START;
 my $MOV0_PE1_dest_addr = $MEM_H4B1_START; ## also MOV1_PE2_source addr
 my $MOV1_PE2_dest_addr = $MEM_H4B1_START; ## also MOV2_PE3_source addr
 my $MOV2_PE3_dest_addr = $MEM_H4B1_START;
#########
######## init 
my @PE0_MEM_LINES 	= ();
my @PE1_MEM_LINES 	= ();
my @PE2_MEM_LINES 	= ();
my @PE3_MEM_LINES 	= ();
my $PE0_I_MEM_START;
my $PE0_O_MEM_START;
my $PE1_I_MEM_START;
my $PE1_O_MEM_START;
my $PE2_I_MEM_START;
my $PE2_O_MEM_START;
my $PE3_I_MEM_START;
my $PE3_O_MEM_START;

my $inst = "MOV_INIT";
$PE0_I_MEM_START = $MOV0_PE0_source_addr;
$PE1_I_MEM_START = $MOV0_PE1_source_addr;

# for($i=0; $i<$MEM_OUT_BOUND; $i++){
	# $PE0_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
	# $PE1_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
	# $PE2_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
	# $PE3_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
# }

for($i=0; $i<$MEM_OUT_BOUND; $i++){
	$PE0_MEM_LINES[$i] = "000000000000000000000000\n";
	$PE1_MEM_LINES[$i] = "000000000000000000000000\n";
	$PE2_MEM_LINES[$i] = "000000000000000000000000\n";
	$PE3_MEM_LINES[$i] = "000000000000000000000000\n";
}

## generate random number in memory
my $temp_num;
for ($i = $PE0_I_MEM_START; $i < $PE0_I_MEM_START + $input_size; $i++){
	$temp_num = (($i  / $input_size) * (2 ** 32)) & 0xFFFFFFFF;
#  print sprintf ("temp: %8.8x\n", $temp_num);
	$PE0_MEM_LINES[$i] = sprintf ("%8.8x%8.8x%8.8x\n", $temp_num, $temp_num, $temp_num);
}
for ($i = $PE1_I_MEM_START; $i < $PE1_I_MEM_START + $input_size; $i++){
	$temp_num = ((-$i  / $input_size) * (2 ** 32)) & 0xFFFFFFFF;
#  print sprintf ("temp: %8.8x\n", $temp_num);
	$PE1_MEM_LINES[$i] = sprintf ("%8.8x%8.8x%8.8x\n", $temp_num, $temp_num, $temp_num);
}
## 

## generate ininital memory footprint data file 
my $OF0 = join "", $inst, "_MEM_0.data";
open (OUT0, ">$OF0")
	or die "cannot open < ", $OF0, ": $!";
my $OF1 = join "", $inst, "_MEM_1.data";
open (OUT1, ">$OF1")
	or die "cannot open < ", $OF1, ": $!";
my $OF2 = join "", $inst, "_MEM_2.data";
open (OUT2, ">$OF2")
	or die "cannot open < ", $OF2, ": $!";
my $OF3 = join "", $inst, "_MEM_3.data";
open (OUT3, ">$OF3")
	or die "cannot open < ", $OF3, ": $!";

for($i=0; $i<$MEM_OUT_BOUND; $i++){
	printf OUT0 $PE0_MEM_LINES[$i];
	printf OUT1 $PE1_MEM_LINES[$i];
	printf OUT2 $PE2_MEM_LINES[$i];
	printf OUT3 $PE3_MEM_LINES[$i];
}
close(OUT0);
close(OUT1);
close(OUT2);
close(OUT3);
$OF0 = join "", $inst, "_MEM_0.data";
system ($^X,"V2_MEM.pl", $OF0);
$OF1 = join "", $inst, "_MEM_1.data";
system ($^X,"V2_MEM.pl", $OF1);
$OF2 = join "", $inst, "_MEM_2.data";
system ($^X,"V2_MEM.pl", $OF2);
$OF3 = join "", $inst, "_MEM_3.data";
system ($^X,"V2_MEM.pl", $OF3);
## 
#############################
##### MOV0
my $copy;
system("rm dnn.inst.c");
$PE2_I_MEM_START = 0;
$PE2_O_MEM_START = 0;
my $INPUT_MEM_FILE = "MOV_INIT";
@argument = ("MOV_0", $INPUT_MEM_FILE . "_MEM", 0b0100, 0, 0, 0, 0,  0,0,0, 0,0,0, $PE2_I_MEM_START, $PE2_O_MEM_START, $input_size, 0, 0, 0); 
MOV_sub(@argument);

##### MOV1
$INPUT_MEM_FILE = "MOV_0";
@argument = ("MOV_0", $INPUT_MEM_FILE . "_MEM", 0b0100, 0, 0, 1, 0,  0,0,0, 0,0,0, $PE2_I_MEM_START, $PE2_O_MEM_START, $input_size, 0, 0, 0); 
#for ($ii = 0; $ii < 4; $ii++) { 
#  $original = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".goldendata";
#  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
#  system ("cp", $original, $copy); 
#}
MOV_sub(@argument);
#for ($ii = 0; $ii < 4; $ii++) { 
#  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
#  system ("rm", $copy); 
#}

##### MOV2
$INPUT_MEM_FILE = "MOV_1";
@argument = ("MOV_0", $INPUT_MEM_FILE . "_MEM", 0b0100, 0, 0, 3, 0,  0,0,0, 0,0,0, $PE2_I_MEM_START, $PE2_O_MEM_START, $input_size, 0, 0, 0); 
#for ($ii = 0; $ii < 4; $ii++) { 
#  $original = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".goldendata";
#  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
#  system ("cp", $original, $copy); 
#}
MOV_sub(@argument);
#for ($ii = 0; $ii < 4; $ii++) { 
#  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
#  system ("rm", $copy); 
#}
#exit();
