#!/usr/bin/env perl

use warnings;
use POSIX;
use MOV_sub;

my $input_size = 10; ## word lines; carefule if larger than 32
##########
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
########
### init 
my $inst = "MEM_TEST_1_INIT";
$PE0_I_MEM_START = $MEM_H1B1_START;
#$PE1_I_MEM_START = $MEM_H1B1_START;
$PE2_I_MEM_START = $MEM_H1B1_START;
$PE3_I_MEM_START = $MEM_H1B1_START;

my $temp_num;
for($i=0; $i<$MEM_OUT_BOUND; $i++){
	$temp_num = (($i / $MEM_OUT_BOUND) * (2 ** 32)) & 0xFFFFFFFF;
	$PE0_MEM_LINES[$i] = sprintf ("%8.8x%8.8x%8.8x\n", $temp_num, $temp_num, $temp_num);
	$temp_num = ((-$i / $MEM_OUT_BOUND) * (2 ** 32)) & 0xFFFFFFFF;
	$PE2_MEM_LINES[$i] = sprintf ("%8.8x%8.8x%8.8x\n", $temp_num, $temp_num, $temp_num);
	$temp_num = (($i * 0.5 / $MEM_OUT_BOUND) * (2 ** 32)) & 0xFFFFFFFF;
	$PE3_MEM_LINES[$i] = sprintf ("%8.8x%8.8x%8.8x\n", $temp_num, $temp_num, $temp_num);
	$PE1_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
}

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
#############################
##### MOV0
my $copy;
system("rm dnn.inst.c");
$PE0_O_MEM_START = $MEM_H1B1_START;
my $INPUT_MEM_FILE = "MEM_TEST_1_INIT";
@argument = ("MEM_TEST_1_0", $INPUT_MEM_FILE . "_MEM", 0b0001, 1, 0, 0, 0,  $PE0_I_MEM_START, $PE0_O_MEM_START, $input_size, 0, 0, 0, 0, 0, 0, 0, 0, 0); 
MOV_sub(@argument);

##### MOV1
$PE1_I_MEM_START = $MEM_H1B1_START;
$PE1_O_MEM_START = $MEM_H2B1_START;
$INPUT_MEM_FILE = "MEM_TEST_1_0";
@argument = ("MEM_TEST_1_1", $INPUT_MEM_FILE . "_MEM", 0b0010, 0, 1, 0, 0,  0, 0, 0, $PE1_I_MEM_START, $PE1_O_MEM_START, $input_size, 0, 0, 0,  0, 0, 0); 
for ($ii = 0; $ii < 4; $ii++) { 
  $original = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".goldendata";
  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
  system ("cp", $original, $copy); 
}
MOV_sub(@argument);
for ($ii = 0; $ii < 4; $ii++) { 
  $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
  system ("rm", $copy); 
}
