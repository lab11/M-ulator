#!/usr/bin/perl
#use strict;
use warnings;
use POSIX ;

#### CONSTANT
my $PREC_06B 		= 0;
my $PREC_08B 		= 1;
my $PREC_12B 		= 2;
my $PREC_16B 		= 3;
my $PREC_24B 		= 4;
my $PREC_32B 		= 5;
######

## check with per script (NLI_PAR.pl, etc) for this part
my $NLI_SEGMENT = 10;
my $PE_I_PREC = $PREC_12B;
my $PE_I_EXP = 12;
my $PE_INPUT_SIZE = 128 * (96 / $PE_I_EXP);
##

my $out_nli_filename = "dnn.nli"; 
open (OUT_NLI, ">$out_nli_filename")
	or die "cannot open < ", $out_nli_filename, ": $!";

my @PE_DATA_A = ();
my @PE_DATA_B = ();
my @PE_DATA_X = ();
my $PE_X_STEP;
my $PE_EXP;
my $PE_N_DATA_WORD;

my $PE_I_SIGN = 1;
if ($PE_I_SIGN == 1 && $PE_I_PREC == $PREC_06B){ $PE_X_STEP = floor((2**6-1)/10);	$PE_EXP = 5;  	$PE_N_DATA_WORD = 16; }
if ($PE_I_SIGN == 1 && $PE_I_PREC == $PREC_08B){ $PE_X_STEP = floor((2**8-1)/10);	$PE_EXP = 7; 	$PE_N_DATA_WORD = 12; }
if ($PE_I_SIGN == 1 && $PE_I_PREC == $PREC_12B){ $PE_X_STEP = floor((2**12-1)/10);	$PE_EXP = 11; 	$PE_N_DATA_WORD = 8; }
if ($PE_I_SIGN == 1 && $PE_I_PREC == $PREC_16B){ $PE_X_STEP = floor((2**16-1)/10);	$PE_EXP = 15; 	$PE_N_DATA_WORD = 6; }

if ($PE_I_SIGN == 1){ @PE_DATA_A = (-1024, -512, -256, -128, -64, -32, -16, -8, -4, -2);}
for($i = 0; $i < $NLI_SEGMENT; $i++){
	if($i == 0){
		if ($PE_I_SIGN == 1) {$PE_DATA_X[0] = -int(2**$PE_EXP);}	
		$PE_DATA_X[1] = $PE_DATA_X[0] + $PE_X_STEP;	
		$PE_DATA_B[0] = 1;
	}else{
		if($i == $NLI_SEGMENT - 1){	$PE_DATA_X[$i+1] = 2**$PE_EXP - 1;}
		else{				$PE_DATA_X[$i+1] = $PE_DATA_X[$i] + $PE_X_STEP;}
						$PE_DATA_B[$i] = $PE_DATA_X[$i] * $PE_DATA_A[$i-1] + $PE_DATA_B[$i-1] - $PE_DATA_X[$i] * $PE_DATA_A[$i];
	}
}

#### debug
#for($i = 0; $i < $NLI_SEGMENT; $i++){
#  $PE_DATA_A[$i] = 1024;    ## shift = 10
#  $PE_DATA_B[$i] = 0;
#}
#### debug

my $i;
for ($i = 0; $i < $NLI_SEGMENT; $i++) {
  printf OUT_NLI sprintf("%d,", $PE_DATA_X[$i]);
}
printf OUT_NLI sprintf("%d\n", $PE_DATA_X[$NLI_SEGMENT]);

for ($i = 0; $i < $NLI_SEGMENT - 1; $i++) {
  printf OUT_NLI sprintf("%d,", $PE_DATA_A[$i]);
}
printf OUT_NLI sprintf("%d\n", $PE_DATA_A[$NLI_SEGMENT - 1]);

for ($i = 0; $i < $NLI_SEGMENT - 1; $i++) {
  printf OUT_NLI sprintf("%d,", $PE_DATA_B[$i]);
}
printf OUT_NLI sprintf("%d\n", $PE_DATA_B[$NLI_SEGMENT - 1]);

close (OUT_NLI);

################
my $out_input_filename = "nli.inputs"; 
open (OUT_INPUT, ">$out_input_filename")
	or die "cannot open < ", $out_input_filename, ": $!";

my @PE_INPUT = ();
for($i = 0; $i < $PE_INPUT_SIZE; $i++){
  $PE_INPUT[$i] = int((2**($PE_I_EXP) - 1) * $i / $PE_INPUT_SIZE - 2**($PE_I_EXP-1));
}

for ($i = 0; $i < $PE_INPUT_SIZE; $i++) {
  printf OUT_INPUT sprintf("%d,", $PE_INPUT[$i]);
}
printf OUT_INPUT sprintf("%d\n", $PE_INPUT[$PE_INPUT_SIZE - 1]);

close (OUT_INPUT);
