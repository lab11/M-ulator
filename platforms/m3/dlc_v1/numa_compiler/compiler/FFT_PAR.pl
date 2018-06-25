#!/usr/bin/perl
#use strict;
use warnings;
use POSIX ;

my $inst = "FFT";
if (defined $ARGV[0]) {
  $inst = $ARGV[0];
}
my $INPUT_MEM_FILE; 
if (defined $ARGV[1]) {
  $INPUT_MEM_FILE = $ARGV[1];    ## memory footprint before FFT (for instance, containing MAC parameters)
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

my $OF_DBG = join "", $inst, "_MEM.debug";
open (OUT_DBG, ">$OF_DBG")
	or die "cannot open < ", $OF_DBG, ": $!";

my $INST = join "", $inst, ".inst";
open (OUT_INST, ">$INST")
	or die "cannot open < ", $INST, ": $!";

my $out_dnn_inst_filename = "dnn.inst.c";
open (OUT_DNN_INST, ">$out_dnn_inst_filename")
	or die "cannot open < ", $out_dnn_inst_filename, ": $!";

my $i;
my $j;
my $LINE;

#
my $PE0_DBG	= 1;
my $PE1_DBG	= 1;
my $PE2_DBG	= 1;
my $PE3_DBG	= 1;
#
#
# SPECIFY PE UTILIZATION
my $PE0_I 	= 0;
my $PE0_O 	= 0;
my $PE0_W 	= 0;
my $PE0_OS	= 0;

my $PE1_I 	= 1;
my $PE1_O 	= 1;
my $PE1_W 	= 1;
my $PE1_OS	= 0;

my $PE2_I 	= 2;
my $PE2_O 	= 2;
my $PE2_W 	= 2;
my $PE2_OS	= 0;

my $PE3_I 	= 3;
my $PE3_O 	= 3;
my $PE3_W 	= 3;
my $PE3_OS	= 0;

my $OP_FFT		= 0;
my $OP_MAC		= 1;
my $OP_NLI		= 2;
my $OP_MOV		= 3;

my $NLI_SEGMENT		= 10;

my $MEM_H4_START 	= 0;
my $MEM_H3_START 	= 4096;
my $MEM_H2_START 	= 5120;
my $MEM_H1_START 	= 5632;
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

my $PREC_06B 		= 0;
my $PREC_08B 		= 1;
my $PREC_12B 		= 2;
my $PREC_16B 		= 3;
my $PREC_24B 		= 4;
my $PREC_32B 		= 5;

##############
###caogao
my $weight_filename = "fft_0.weights";
my $input_filename = "fft_0.inputs";
if (defined $ARGV[2]) { $weight_filename = $ARGV[2]; }
if (defined $ARGV[3]) { $input_filename = $ARGV[3];  }

open($weight_file, "<", $weight_filename) 
	or die "cannot open < ", $weight_filename, ": $!";
open($input_file, "<", $input_filename) 
	or die "cannot open < ", $input_filename, ": $!";

my $PE0_INST	 = 1;
my $PE1_INST	 = 0;
my $PE2_INST	 = 1;
my $PE3_INST	 = 0;

my $PE0_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE0_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE0_W_MEM_START_BASE = $MEM_H3B1_START;
#
my $PE1_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE1_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE1_W_MEM_START_BASE = $MEM_H3B1_START;
#
my $PE2_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE2_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE2_W_MEM_START_BASE = $MEM_H3B1_START;
#
my $PE3_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE3_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE3_W_MEM_START_BASE = $MEM_H3B1_START;

# IMPORTANT NOTE
# $I_PREC SHOULD BE >= $W_PREC UNLESS I_PREC and W_PREC are (08B, 12B)
# WORKING SETS ARE FOLLING
# ($I_PREC, $W_PREC) = (16B, 16B) (16B, 12B) (16B, 08B) (16B, 06B)
# ($I_PREC, $W_PREC) = (12B, 12B) (12B, 08B) (12B, 06B)
# ($I_PREC, $W_PREC) = (08B, 12B) (08B, 08B) (08B, 06B)
# ($I_PREC, $W_PREC) = (06B, 06B)
# $O_PREC HAS SAME RULE
my $COMMON_I_PREC	= $PREC_12B;
my $COMMON_O_PREC	= $PREC_12B;	
my $COMMON_W_PREC	= $PREC_08B;

# FFT_NUM MUST BE FACTOR OF I_SIZE
my $COMMON_FFT_NUM	= 4;

$LINE = <$input_file>;		# size 
my @line = split / /,$LINE;
my $I_ELEMENTS = $line[0];

$LINE = <$weight_file>;		# size 
@line = split / /,$LINE;
my $W_ELEMENTS = $line[0];
$W_ELEMENTS == $I_ELEMENTS / 2 / $COMMON_FFT_NUM 
  or die "twiddle factor does not match: check input file size and COMMON_FFT_NUM";

my $COMMON_I_SIZE; 
if ($COMMON_I_PREC == $PREC_06B) 	{$COMMON_I_SIZE = $I_ELEMENTS / 8};
if ($COMMON_I_PREC == $PREC_08B) 	{$COMMON_I_SIZE = $I_ELEMENTS / 4};
if ($COMMON_I_PREC == $PREC_12B) 	{$COMMON_I_SIZE = $I_ELEMENTS / 4};
if ($COMMON_I_PREC == $PREC_16B) 	{$COMMON_I_SIZE = $I_ELEMENTS / 2};
##############
	
# GENERATE DATA FOR TESTING FFT
# PE0
#
my $PE0_I_DATApWORD;
my $PE0_O_DATApWORD;
my $PE0_W_DATApWORD;
#
my $PE0_I_PREC		= $COMMON_I_PREC;
my $PE0_I_EXP;
if ($PE0_I_PREC == $PREC_06B) 	{$PE0_I_DATApWORD = 8;	$PE0_I_EXP = 6;}
if ($PE0_I_PREC == $PREC_08B) 	{$PE0_I_DATApWORD = 4;	$PE0_I_EXP = 8;}
if ($PE0_I_PREC == $PREC_12B) 	{$PE0_I_DATApWORD = 4;	$PE0_I_EXP = 12;}
if ($PE0_I_PREC == $PREC_16B) 	{$PE0_I_DATApWORD = 2;	$PE0_I_EXP = 16;}
#
my $PE0_W_PREC		= $COMMON_W_PREC;
my $PE0_W_EXP;
if ($PE0_W_PREC == $PREC_06B) 	{$PE0_W_DATApWORD = 8;	$PE0_W_EXP = 6;}
if ($PE0_W_PREC == $PREC_08B) 	{$PE0_W_DATApWORD = 4;	$PE0_W_EXP = 8;}
if ($PE0_W_PREC == $PREC_12B) 	{$PE0_W_DATApWORD = 4;	$PE0_W_EXP = 12;}
if ($PE0_W_PREC == $PREC_16B) 	{$PE0_W_DATApWORD = 2;	$PE0_W_EXP = 16;}
#
my $PE0_O_PREC		= $COMMON_O_PREC;
my $PE0_O_EXP;
if ($PE0_O_PREC == $PREC_06B) 	{$PE0_O_DATApWORD = 8;	$PE0_O_EXP = 6;}
if ($PE0_O_PREC == $PREC_08B) 	{$PE0_O_DATApWORD = 4;	$PE0_O_EXP = 8;}
if ($PE0_O_PREC == $PREC_12B) 	{$PE0_O_DATApWORD = 4;	$PE0_O_EXP = 12;}
if ($PE0_O_PREC == $PREC_16B) 	{$PE0_O_DATApWORD = 2;	$PE0_O_EXP = 16;}
#
my $PE0_FFT_NUM	 	= $COMMON_FFT_NUM;	
my $PE0_OP		= $OP_FFT;
my $PE0_TO_MEM_START	= 0;
#my $PE0_TO_MEM_START 	= $PE0_TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE0_TO_MEM_SIZE	= 0;
my $PE0_TO_PREC		= 0;
#my $PE0_I_MEM_START_BASE = $MEM_H1B1_START;
#my $PE0_I_MEM_START_BASE = $MEM_H2B4_START;
my $PE0_I_MEM_START  	= $PE0_I_MEM_START_BASE  + $PE0_I*(2**13);
my $PE0_I_MEM_SIZE 	= $COMMON_I_SIZE;
#my $PE0_I_MEM_SIZE 	= 128;
#my $PE0_I_PREC		= $PREC_12B;
#my $PE0_O_MEM_START_BASE = $MEM_H2B1_START;
#my $PE0_O_MEM_START_BASE = $PE0_I_MEM_START_BASE;
my $PE0_O_MEM_START  	= $PE0_O_MEM_START_BASE  + $PE0_O*(2**13);
my $PE0_O_MEM_SIZE 	= int($PE0_I_MEM_SIZE*$PE0_I_DATApWORD/$PE0_O_DATApWORD);
#my $PE0_O_PREC		= $I_PREC;
my $PE0_O_SIGN		= 1;
my $PE0_OS_MEM_START_BASE= 0;
my $PE0_OS_MEM_START 	= $PE0_OS_MEM_START_BASE + $PE0_OS*(2**13);
##my $PE0_W_MEM_START_BASE= $MEM_H4B1_START;
#my $PE0_W_MEM_START_BASE= $MEM_H2B1_START;
my $PE0_W_MEM_START  	= $PE0_W_MEM_START_BASE  + $PE0_W*(2**13);
my $PE0_W_MEM_SIZE	= int($PE0_I_MEM_SIZE*$PE0_I_DATApWORD/($PE0_W_DATApWORD*2*$PE0_FFT_NUM));
#my $W_PREC		= 0;
my $PE0_MAC_OFFSET	= 0;
my $PE0_MAC_NLI		= 0;
my $PE0_MAC_ROW_NUM	= 0;
my $PE0_FFT_I_UNIT_SIZE = int($PE0_I_MEM_SIZE/$PE0_FFT_NUM);
my $PE0_FFT_O_UNIT_SIZE	= int($PE0_O_MEM_SIZE/$PE0_FFT_NUM);
my $PE0_SHIFT		= int($PE0_I_EXP + $PE0_W_EXP - $PE0_O_EXP) - 1;
my $PE0_SHIFT_OS 	= 0;
my $PE0_SHIFT_NL	= 0;
#
print 	"\n";
print	sprintf("PE0_TO_MEM_START = %d\n", $PE0_TO_MEM_START + 0*(2**13));
print	sprintf("PE0_TO_MEM_END = %d\n", $PE0_TO_MEM_START + 0*(2**13) + $PE0_TO_MEM_SIZE - 1);
print	sprintf("PE0_I_MEM_START  = %d\n", $PE0_I_MEM_START);
print	sprintf("PE0_I_MEM_END    = %d\n", $PE0_I_MEM_START + $PE0_I_MEM_SIZE - 1);
print	sprintf("PE0_O_MEM_START  = %d\n", $PE0_O_MEM_START);
print	sprintf("PE0_O_MEM_END    = %d\n", $PE0_O_MEM_START + $PE0_O_MEM_SIZE - 1);
print	sprintf("PE0_W_MEM_START  = %d\n", $PE0_W_MEM_START);
print	sprintf("PE0_W_MEM_END    = %d\n", $PE0_W_MEM_START + $PE0_W_MEM_SIZE - 1);
print	sprintf("PE0_OS_MEM_START = %d\n", $PE0_OS_MEM_START);
print	sprintf("PE0_OS_MEM_END   = %d\n", $PE0_OS_MEM_START + $PE0_O_MEM_SIZE - 1);
#
#
my $PE0_INST_0 = "";
my $PE0_INST_1 = "";
my $PE0_INST_2 = "";
my $PE0_INST_3 = "";
my $PE0_INST_4 = "";
my $PE0_INST_5 = "";
#
my $PE0_MAC_EN = 0;
#
# CREATE INSTRUCTION
$PE0_INST_0 = "";
$PE0_INST_0 = join "", sprintf("%2.2b",$PE0_OP), $PE0_INST_0;
if ($PE0_MAC_EN) {
	$PE0_INST_0 = join "", sprintf("%13.13b",$PE0_TO_MEM_START), $PE0_INST_0;
	$PE0_INST_0 = join "", sprintf("%13.13b",$PE0_TO_MEM_SIZE), $PE0_INST_0;
}else {
	$PE0_INST_0 = join "", sprintf("%13.13b",$PE0_FFT_NUM), $PE0_INST_0;
	$PE0_INST_0 = join "", sprintf("%13.13b",$PE0_FFT_I_UNIT_SIZE), $PE0_INST_0;
}
$PE0_INST_0 = join "", sprintf("%3.3b",$PE0_TO_PREC), $PE0_INST_0;
$PE0_INST_0 = join "", sprintf("%1.1b",$PE0_I_MEM_START & 0x1), $PE0_INST_0;
#
$PE0_INST_1 = "";
$PE0_INST_1 = join "", sprintf("%14.14b",int($PE0_I_MEM_START/2) & (2**14-1)), $PE0_INST_1;
$PE0_INST_1 = join "", sprintf("%15.15b",$PE0_I_MEM_SIZE), $PE0_INST_1;
$PE0_INST_1 = join "", sprintf("%3.3b",$PE0_I_PREC), $PE0_INST_1;
#
$PE0_INST_2 = "";
$PE0_INST_2 = join "", sprintf("%15.15b",$PE0_O_MEM_START), $PE0_INST_2;
$PE0_INST_2 = join "", sprintf("%15.15b",$PE0_O_MEM_SIZE), $PE0_INST_2;
$PE0_INST_2 = join "", sprintf("%2.2b",$PE0_O_PREC & 0x3), $PE0_INST_2;
#
$PE0_INST_3 = "";
$PE0_INST_3 = join "", sprintf("%1.1b",int($PE0_O_PREC/4) & 0x1), $PE0_INST_3;
$PE0_INST_3 = join "", sprintf("%1.1b",$PE0_O_SIGN), $PE0_INST_3;
if ($MAC_EN){
	$PE0_INST_3 = join "", sprintf("%15.15b",$PE0_OS_MEM_START), $PE0_INST_3;
} else {
	$PE0_INST_3 = join "", sprintf("%13.13b",$PE0_FFT_O_UNIT_SIZE), $PE0_INST_3;
	$PE0_INST_3 = join "", sprintf("%2.2b",0), $PE0_INST_3;
}
$PE0_INST_3 = join "", sprintf("%15.15b",$PE0_W_MEM_START), $PE0_INST_3;
#
$PE0_INST_4 = "";
$PE0_INST_4 = join "", sprintf("%15.15b",$PE0_W_MEM_SIZE), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%3.3b",$PE0_W_PREC), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%1.1b",$PE0_MAC_OFFSET), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%1.1b",$PE0_MAC_NLI), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%12.12b",$PE0_MAC_ROW_NUM & (2**12-1)), $PE0_INST_4;
#
$PE0_INST_5 = "";
$PE0_INST_5 = join "", sprintf("%3.3b",int($PE0_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT_OS), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT_NL), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%14.14b",0), $PE0_INST_5;
#			
# GENERATE DATA FOR TESTING FFT
# PE1
#
my $PE1_I_DATApWORD;
my $PE1_O_DATApWORD;
my $PE1_W_DATApWORD;
# IMPORTANT NOTE
# $I_PREC SHOULD BE >= $W_PREC UNLESS I_PREC and W_PREC are (08B, 12B)
# WORKING SETS ARE FOLLING
# ($I_PREC, $W_PREC) = (16B, 16B) (16B, 12B) (16B, 08B) (16B, 06B)
# ($I_PREC, $W_PREC) = (12B, 12B) (12B, 08B) (12B, 06B)
# ($I_PREC, $W_PREC) = (08B, 12B) (08B, 08B) (08B, 06B)
# ($I_PREC, $W_PREC) = (06B, 06B)
# $O_PREC HAS SAME RULE
#
my $PE1_I_PREC		= $COMMON_I_PREC;
my $PE1_I_EXP;
if ($PE1_I_PREC == $PREC_06B) 	{$PE1_I_DATApWORD = 8;	$PE1_I_EXP = 6;}
if ($PE1_I_PREC == $PREC_08B) 	{$PE1_I_DATApWORD = 4;	$PE1_I_EXP = 8;}
if ($PE1_I_PREC == $PREC_12B) 	{$PE1_I_DATApWORD = 4;	$PE1_I_EXP = 12;}
if ($PE1_I_PREC == $PREC_16B) 	{$PE1_I_DATApWORD = 2;	$PE1_I_EXP = 16;}
#
my $PE1_W_PREC		= $COMMON_W_PREC;
my $PE1_W_EXP;
if ($PE1_W_PREC == $PREC_06B) 	{$PE1_W_DATApWORD = 8;	$PE1_W_EXP = 6;}
if ($PE1_W_PREC == $PREC_08B) 	{$PE1_W_DATApWORD = 4;	$PE1_W_EXP = 8;}
if ($PE1_W_PREC == $PREC_12B) 	{$PE1_W_DATApWORD = 4;	$PE1_W_EXP = 12;}
if ($PE1_W_PREC == $PREC_16B) 	{$PE1_W_DATApWORD = 2;	$PE1_W_EXP = 16;}
#
my $PE1_O_PREC		= $COMMON_O_PREC;
my $PE1_O_EXP;
if ($PE1_O_PREC == $PREC_06B) 	{$PE1_O_DATApWORD = 8;	$PE1_O_EXP = 6;}
if ($PE1_O_PREC == $PREC_08B) 	{$PE1_O_DATApWORD = 4;	$PE1_O_EXP = 8;}
if ($PE1_O_PREC == $PREC_12B) 	{$PE1_O_DATApWORD = 4;	$PE1_O_EXP = 12;}
if ($PE1_O_PREC == $PREC_16B) 	{$PE1_O_DATApWORD = 2;	$PE1_O_EXP = 16;}
#
my $PE1_FFT_NUM	 	= $COMMON_FFT_NUM;	
my $PE1_OP		= $OP_FFT;
my $PE1_TO_MEM_START	= 0;
#my $PE1_TO_MEM_START 	= $PE1_TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE1_TO_MEM_SIZE	= 0;
my $PE1_TO_PREC		= 0;
#my $PE1_I_MEM_START_BASE = $MEM_H1B2_START;
#my $PE1_I_MEM_START_BASE = $MEM_H2B4_START;
my $PE1_I_MEM_START  	= $PE1_I_MEM_START_BASE  + $PE1_I*(2**13);
my $PE1_I_MEM_SIZE 	= $COMMON_I_SIZE;
#my $PE1_I_MEM_SIZE 	= 128;
#my $PE1_I_PREC		= $PREC_12B;
#my $PE1_O_MEM_START_BASE = $MEM_H2B2_START;
#my $PE1_O_MEM_START_BASE = $MEM_H2B1_START;
#my $PE1_O_MEM_START_BASE = $PE1_I_MEM_START_BASE;
my $PE1_O_MEM_START  	= $PE1_O_MEM_START_BASE  + $PE1_O*(2**13);
my $PE1_O_MEM_SIZE 	= int($PE1_I_MEM_SIZE*$PE1_I_DATApWORD/$PE1_O_DATApWORD);
#my $PE1_O_PREC		= $I_PREC;
my $PE1_O_SIGN		= 1;
my $PE1_OS_MEM_START_BASE= 0;
my $PE1_OS_MEM_START 	= $PE1_OS_MEM_START_BASE + $PE1_OS*(2**13);
#my $PE1_W_MEM_START_BASE= $MEM_H4B1_START;
#my $PE1_W_MEM_START_BASE= $MEM_H2B1_START;
my $PE1_W_MEM_START  	= $PE1_W_MEM_START_BASE  + $PE1_W*(2**13);
my $PE1_W_MEM_SIZE	= int($PE1_I_MEM_SIZE*$PE1_I_DATApWORD/($PE1_W_DATApWORD*2*$PE1_FFT_NUM));
#my $W_PREC		= 0;
my $PE1_MAC_OFFSET	= 0;
my $PE1_MAC_NLI		= 0;
my $PE1_MAC_ROW_NUM	= 0;
my $PE1_FFT_I_UNIT_SIZE = int($PE1_I_MEM_SIZE/$PE1_FFT_NUM);
my $PE1_FFT_O_UNIT_SIZE	= int($PE1_O_MEM_SIZE/$PE1_FFT_NUM);
my $PE1_SHIFT		= int($PE1_I_EXP + $PE1_W_EXP - $PE1_O_EXP) - 1;
my $PE1_SHIFT_OS 	= 0;
my $PE1_SHIFT_NL	= 0;
#
print 	"\n";
print	sprintf("PE1_TO_MEM_START = %d\n", $PE1_TO_MEM_START + 1*(2**13));
print	sprintf("PE1_TO_MEM_END = %d\n", $PE1_TO_MEM_START + 1*(2**13) + $PE1_TO_MEM_SIZE - 1);
print	sprintf("PE1_I_MEM_START  = %d\n", $PE1_I_MEM_START);
print	sprintf("PE1_I_MEM_END    = %d\n", $PE1_I_MEM_START + $PE1_I_MEM_SIZE - 1);
print	sprintf("PE1_O_MEM_START  = %d\n", $PE1_O_MEM_START);
print	sprintf("PE1_O_MEM_END    = %d\n", $PE1_O_MEM_START + $PE1_O_MEM_SIZE - 1);
print	sprintf("PE1_W_MEM_START  = %d\n", $PE1_W_MEM_START);
print	sprintf("PE1_W_MEM_END    = %d\n", $PE1_W_MEM_START + $PE1_W_MEM_SIZE - 1);
print	sprintf("PE1_OS_MEM_START = %d\n", $PE1_OS_MEM_START);
print	sprintf("PE1_OS_MEM_END   = %d\n", $PE1_OS_MEM_START + $PE1_O_MEM_SIZE - 1);
#
#
my $PE1_INST_0 = "";
my $PE1_INST_1 = "";
my $PE1_INST_2 = "";
my $PE1_INST_3 = "";
my $PE1_INST_4 = "";
my $PE1_INST_5 = "";
#
my $PE1_MAC_EN = 0;
#
# CREATE INSTRUCTION
$PE1_INST_0 = "";
$PE1_INST_0 = join "", sprintf("%2.2b",$PE1_OP), $PE1_INST_0;
if ($PE1_MAC_EN) {
	$PE1_INST_0 = join "", sprintf("%13.13b",$PE1_TO_MEM_START), $PE1_INST_0;
	$PE1_INST_0 = join "", sprintf("%13.13b",$PE1_TO_MEM_SIZE), $PE1_INST_0;
}else {
	$PE1_INST_0 = join "", sprintf("%13.13b",$PE1_FFT_NUM), $PE1_INST_0;
	$PE1_INST_0 = join "", sprintf("%13.13b",$PE1_FFT_I_UNIT_SIZE), $PE1_INST_0;
}
$PE1_INST_0 = join "", sprintf("%3.3b",$PE1_TO_PREC), $PE1_INST_0;
$PE1_INST_0 = join "", sprintf("%1.1b",$PE1_I_MEM_START & 0x1), $PE1_INST_0;
#
$PE1_INST_1 = "";
$PE1_INST_1 = join "", sprintf("%14.14b",int($PE1_I_MEM_START/2) & (2**14-1)), $PE1_INST_1;
$PE1_INST_1 = join "", sprintf("%15.15b",$PE1_I_MEM_SIZE), $PE1_INST_1;
$PE1_INST_1 = join "", sprintf("%3.3b",$PE1_I_PREC), $PE1_INST_1;
#
$PE1_INST_2 = "";
$PE1_INST_2 = join "", sprintf("%15.15b",$PE1_O_MEM_START), $PE1_INST_2;
$PE1_INST_2 = join "", sprintf("%15.15b",$PE1_O_MEM_SIZE), $PE1_INST_2;
$PE1_INST_2 = join "", sprintf("%2.2b",$PE1_O_PREC & 0x3), $PE1_INST_2;
#
$PE1_INST_3 = "";
$PE1_INST_3 = join "", sprintf("%1.1b",int($PE1_O_PREC/4) & 0x1), $PE1_INST_3;
$PE1_INST_3 = join "", sprintf("%1.1b",$PE1_O_SIGN), $PE1_INST_3;
if ($MAC_EN){
	$PE1_INST_3 = join "", sprintf("%15.15b",$PE1_OS_MEM_START), $PE1_INST_3;
} else {
	$PE1_INST_3 = join "", sprintf("%13.13b",$PE1_FFT_O_UNIT_SIZE), $PE1_INST_3;
	$PE1_INST_3 = join "", sprintf("%2.2b",0), $PE1_INST_3;
}
$PE1_INST_3 = join "", sprintf("%15.15b",$PE1_W_MEM_START), $PE1_INST_3;
#
$PE1_INST_4 = "";
$PE1_INST_4 = join "", sprintf("%15.15b",$PE1_W_MEM_SIZE), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%3.3b",$PE1_W_PREC), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%1.1b",$PE1_MAC_OFFSET), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%1.1b",$PE1_MAC_NLI), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%12.12b",$PE1_MAC_ROW_NUM & (2**12-1)), $PE1_INST_4;
#
$PE1_INST_5 = "";
$PE1_INST_5 = join "", sprintf("%3.3b",int($PE1_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT_OS), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT_NL), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%14.14b",0), $PE1_INST_5;
#
# GENERATE DATA FOR TESTING FFT
# PE2
#
my $PE2_I_DATApWORD;
my $PE2_O_DATApWORD;
my $PE2_W_DATApWORD;
# IMPORTANT NOTE
# $I_PREC SHOULD BE >= $W_PREC UNLESS I_PREC and W_PREC are (08B, 12B)
# WORKING SETS ARE FOLLING
# ($I_PREC, $W_PREC) = (16B, 16B) (16B, 12B) (16B, 08B) (16B, 06B)
# ($I_PREC, $W_PREC) = (12B, 12B) (12B, 08B) (12B, 06B)
# ($I_PREC, $W_PREC) = (08B, 12B) (08B, 08B) (08B, 06B)
# ($I_PREC, $W_PREC) = (06B, 06B)
# $O_PREC HAS SAME RULE
#
my $PE2_I_PREC		= $COMMON_I_PREC;
my $PE2_I_EXP;
if ($PE2_I_PREC == $PREC_06B) 	{$PE2_I_DATApWORD = 8;	$PE2_I_EXP = 6;}
if ($PE2_I_PREC == $PREC_08B) 	{$PE2_I_DATApWORD = 4;	$PE2_I_EXP = 8;}
if ($PE2_I_PREC == $PREC_12B) 	{$PE2_I_DATApWORD = 4;	$PE2_I_EXP = 12;}
if ($PE2_I_PREC == $PREC_16B) 	{$PE2_I_DATApWORD = 2;	$PE2_I_EXP = 16;}
#
my $PE2_W_PREC		= $COMMON_W_PREC;
my $PE2_W_EXP;
if ($PE2_W_PREC == $PREC_06B) 	{$PE2_W_DATApWORD = 8;	$PE2_W_EXP = 6;}
if ($PE2_W_PREC == $PREC_08B) 	{$PE2_W_DATApWORD = 4;	$PE2_W_EXP = 8;}
if ($PE2_W_PREC == $PREC_12B) 	{$PE2_W_DATApWORD = 4;	$PE2_W_EXP = 12;}
if ($PE2_W_PREC == $PREC_16B) 	{$PE2_W_DATApWORD = 2;	$PE2_W_EXP = 16;}
#
my $PE2_O_PREC		= $COMMON_O_PREC;
my $PE2_O_EXP;
if ($PE2_O_PREC == $PREC_06B) 	{$PE2_O_DATApWORD = 8;	$PE2_O_EXP = 6;}
if ($PE2_O_PREC == $PREC_08B) 	{$PE2_O_DATApWORD = 4;	$PE2_O_EXP = 8;}
if ($PE2_O_PREC == $PREC_12B) 	{$PE2_O_DATApWORD = 4;	$PE2_O_EXP = 12;}
if ($PE2_O_PREC == $PREC_16B) 	{$PE2_O_DATApWORD = 2;	$PE2_O_EXP = 16;}
#
my $PE2_FFT_NUM	 	= $COMMON_FFT_NUM;	
my $PE2_OP		= $OP_FFT;
my $PE2_TO_MEM_START	= 0;
#my $PE2_TO_MEM_START 	= $PE2_TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE2_TO_MEM_SIZE	= 0;
my $PE2_TO_PREC		= 0;
# my $PE2_I_MEM_START_BASE = $MEM_H1B3_START;
#my $PE2_I_MEM_START_BASE = $MEM_H2B4_START;
my $PE2_I_MEM_START  	= $PE2_I_MEM_START_BASE  + $PE2_I*(2**13);
my $PE2_I_MEM_SIZE 	= $COMMON_I_SIZE;
#my $PE2_I_MEM_SIZE 	= 128;
#my $PE2_I_PREC		= $PREC_12B;
#my $PE2_O_MEM_START_BASE = $MEM_H2B3_START;
#my $PE2_O_MEM_START_BASE = $MEM_H2B1_START;
#my $PE2_O_MEM_START_BASE = $PE2_I_MEM_START_BASE;
my $PE2_O_MEM_START  	= $PE2_O_MEM_START_BASE  + $PE2_O*(2**13);
my $PE2_O_MEM_SIZE 	= int($PE2_I_MEM_SIZE*$PE2_I_DATApWORD/$PE2_O_DATApWORD);
#my $PE2_O_PREC		= $I_PREC;
my $PE2_O_SIGN		= 1;
my $PE2_OS_MEM_START_BASE= 0;
my $PE2_OS_MEM_START 	= $PE2_OS_MEM_START_BASE + $PE2_OS*(2**13);
#my $PE2_W_MEM_START_BASE= $MEM_H4B1_START;
#my $PE2_W_MEM_START_BASE= $MEM_H2B1_START;
my $PE2_W_MEM_START  	= $PE2_W_MEM_START_BASE  + $PE2_W*(2**13);
my $PE2_W_MEM_SIZE	= int($PE2_I_MEM_SIZE*$PE2_I_DATApWORD/($PE2_W_DATApWORD*2*$PE2_FFT_NUM));
#my $W_PREC		= 0;
my $PE2_MAC_OFFSET	= 0;
my $PE2_MAC_NLI		= 0;
my $PE2_MAC_ROW_NUM	= 0;
my $PE2_FFT_I_UNIT_SIZE = int($PE2_I_MEM_SIZE/$PE2_FFT_NUM);
my $PE2_FFT_O_UNIT_SIZE	= int($PE2_O_MEM_SIZE/$PE2_FFT_NUM);
my $PE2_SHIFT		= int($PE2_I_EXP + $PE2_W_EXP - $PE2_O_EXP) - 1;
my $PE2_SHIFT_OS 	= 0;
my $PE2_SHIFT_NL	= 0;
#
print 	"\n";
print	sprintf("PE2_TO_MEM_START = %d\n", $PE2_TO_MEM_START + 2*(2**13));
print	sprintf("PE2_TO_MEM_END = %d\n", $PE2_TO_MEM_START + 2*(2**13) + $PE2_TO_MEM_SIZE - 1);
print	sprintf("PE2_I_MEM_START  = %d\n", $PE2_I_MEM_START);
print	sprintf("PE2_I_MEM_END    = %d\n", $PE2_I_MEM_START + $PE2_I_MEM_SIZE - 1);
print	sprintf("PE2_O_MEM_START  = %d\n", $PE2_O_MEM_START);
print	sprintf("PE2_O_MEM_END    = %d\n", $PE2_O_MEM_START + $PE2_O_MEM_SIZE - 1);
print	sprintf("PE2_W_MEM_START  = %d\n", $PE2_W_MEM_START);
print	sprintf("PE2_W_MEM_END    = %d\n", $PE2_W_MEM_START + $PE2_W_MEM_SIZE - 1);
print	sprintf("PE2_OS_MEM_START = %d\n", $PE2_OS_MEM_START);
print	sprintf("PE2_OS_MEM_END   = %d\n", $PE2_OS_MEM_START + $PE2_O_MEM_SIZE - 1);
#
my $PE2_INST_0 = "";
my $PE2_INST_1 = "";
my $PE2_INST_2 = "";
my $PE2_INST_3 = "";
my $PE2_INST_4 = "";
my $PE2_INST_5 = "";
#
my $PE2_MAC_EN = 0;
#
# CREATE INSTRUCTION
$PE2_INST_0 = "";
$PE2_INST_0 = join "", sprintf("%2.2b",$PE2_OP), $PE2_INST_0;
if ($PE2_MAC_EN) {
	$PE2_INST_0 = join "", sprintf("%13.13b",$PE2_TO_MEM_START), $PE2_INST_0;
	$PE2_INST_0 = join "", sprintf("%13.13b",$PE2_TO_MEM_SIZE), $PE2_INST_0;
}else {
	$PE2_INST_0 = join "", sprintf("%13.13b",$PE2_FFT_NUM), $PE2_INST_0;
	$PE2_INST_0 = join "", sprintf("%13.13b",$PE2_FFT_I_UNIT_SIZE), $PE2_INST_0;
}
$PE2_INST_0 = join "", sprintf("%3.3b",$PE2_TO_PREC), $PE2_INST_0;
$PE2_INST_0 = join "", sprintf("%1.1b",$PE2_I_MEM_START & 0x1), $PE2_INST_0;
#
$PE2_INST_1 = "";
$PE2_INST_1 = join "", sprintf("%14.14b",int($PE2_I_MEM_START/2) & (2**14-1)), $PE2_INST_1;
$PE2_INST_1 = join "", sprintf("%15.15b",$PE2_I_MEM_SIZE), $PE2_INST_1;
$PE2_INST_1 = join "", sprintf("%3.3b",$PE2_I_PREC), $PE2_INST_1;
#
$PE2_INST_2 = "";
$PE2_INST_2 = join "", sprintf("%15.15b",$PE2_O_MEM_START), $PE2_INST_2;
$PE2_INST_2 = join "", sprintf("%15.15b",$PE2_O_MEM_SIZE), $PE2_INST_2;
$PE2_INST_2 = join "", sprintf("%2.2b",$PE2_O_PREC & 0x3), $PE2_INST_2;
#
$PE2_INST_3 = "";
$PE2_INST_3 = join "", sprintf("%1.1b",int($PE2_O_PREC/4) & 0x1), $PE2_INST_3;
$PE2_INST_3 = join "", sprintf("%1.1b",$PE2_O_SIGN), $PE2_INST_3;
if ($MAC_EN){
	$PE2_INST_3 = join "", sprintf("%15.15b",$PE2_OS_MEM_START), $PE2_INST_3;
} else {
	$PE2_INST_3 = join "", sprintf("%13.13b",$PE2_FFT_O_UNIT_SIZE), $PE2_INST_3;
	$PE2_INST_3 = join "", sprintf("%2.2b",0), $PE2_INST_3;
}
$PE2_INST_3 = join "", sprintf("%15.15b",$PE2_W_MEM_START), $PE2_INST_3;
#
$PE2_INST_4 = "";
$PE2_INST_4 = join "", sprintf("%15.15b",$PE2_W_MEM_SIZE), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%3.3b",$PE2_W_PREC), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%1.1b",$PE2_MAC_OFFSET), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%1.1b",$PE2_MAC_NLI), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%12.12b",$PE2_MAC_ROW_NUM & (2**12-1)), $PE2_INST_4;
#
$PE2_INST_5 = "";
$PE2_INST_5 = join "", sprintf("%3.3b",int($PE2_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT_OS), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT_NL), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%14.14b",0), $PE2_INST_5;
#
# GENERATE DATA FOR TESTING FFT
# PE3
#
my $PE3_I_DATApWORD;
my $PE3_O_DATApWORD;
my $PE3_W_DATApWORD;
# IMPORTANT NOTE
# $I_PREC SHOULD BE >= $W_PREC UNLESS I_PREC and W_PREC are (08B, 12B)
# WORKING SETS ARE FOLLING
# ($I_PREC, $W_PREC) = (16B, 16B) (16B, 12B) (16B, 08B) (16B, 06B)
# ($I_PREC, $W_PREC) = (12B, 12B) (12B, 08B) (12B, 06B)
# ($I_PREC, $W_PREC) = (08B, 12B) (08B, 08B) (08B, 06B)
# ($I_PREC, $W_PREC) = (06B, 06B)
# $O_PREC HAS SAME RULE
#
my $PE3_I_PREC		= $COMMON_I_PREC;
my $PE3_I_EXP;
if ($PE3_I_PREC == $PREC_06B) 	{$PE3_I_DATApWORD = 8;	$PE3_I_EXP = 6;}
if ($PE3_I_PREC == $PREC_08B) 	{$PE3_I_DATApWORD = 4;	$PE3_I_EXP = 8;}
if ($PE3_I_PREC == $PREC_12B) 	{$PE3_I_DATApWORD = 4;	$PE3_I_EXP = 12;}
if ($PE3_I_PREC == $PREC_16B) 	{$PE3_I_DATApWORD = 2;	$PE3_I_EXP = 16;}
#
my $PE3_W_PREC		= $COMMON_W_PREC;
my $PE3_W_EXP;
if ($PE3_W_PREC == $PREC_06B) 	{$PE3_W_DATApWORD = 8;	$PE3_W_EXP = 6;}
if ($PE3_W_PREC == $PREC_08B) 	{$PE3_W_DATApWORD = 4;	$PE3_W_EXP = 8;}
if ($PE3_W_PREC == $PREC_12B) 	{$PE3_W_DATApWORD = 4;	$PE3_W_EXP = 12;}
if ($PE3_W_PREC == $PREC_16B) 	{$PE3_W_DATApWORD = 2;	$PE3_W_EXP = 16;}
#
my $PE3_O_PREC		= $COMMON_O_PREC;
my $PE3_O_EXP;
if ($PE3_O_PREC == $PREC_06B) 	{$PE3_O_DATApWORD = 8;	$PE3_O_EXP = 6;}
if ($PE3_O_PREC == $PREC_08B) 	{$PE3_O_DATApWORD = 4;	$PE3_O_EXP = 8;}
if ($PE3_O_PREC == $PREC_12B) 	{$PE3_O_DATApWORD = 4;	$PE3_O_EXP = 12;}
if ($PE3_O_PREC == $PREC_16B) 	{$PE3_O_DATApWORD = 2;	$PE3_O_EXP = 16;}
#
my $PE3_FFT_NUM	 	= $COMMON_FFT_NUM;	
my $PE3_OP		= $OP_FFT;
my $PE3_TO_MEM_START	= 0;
#my $PE3_TO_MEM_START 	= $PE3_TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE3_TO_MEM_SIZE		= 0;
my $PE3_TO_PREC		= 0;
#my $PE3_I_MEM_START_BASE = $MEM_H1B4_START;
#my $PE3_I_MEM_START_BASE = $MEM_H2B4_START;
my $PE3_I_MEM_START  	= $PE3_I_MEM_START_BASE  + $PE3_I*(2**13);
my $PE3_I_MEM_SIZE 	= $COMMON_I_SIZE;
#my $PE3_I_MEM_SIZE 	= 128;
#my $PE3_I_PREC		= $PREC_12B;
#my $PE3_O_MEM_START_BASE = $MEM_H2B4_START;
#my $PE3_O_MEM_START_BASE = $MEM_H2B1_START;
#my $PE3_O_MEM_START_BASE = $PE3_I_MEM_START_BASE;
my $PE3_O_MEM_START  	= $PE3_O_MEM_START_BASE  + $PE3_O*(2**13);
my $PE3_O_MEM_SIZE 	= int($PE3_I_MEM_SIZE*$PE3_I_DATApWORD/$PE3_O_DATApWORD);
#my $PE3_O_PREC		= $I_PREC;
my $PE3_O_SIGN		= 1;
my $PE3_OS_MEM_START_BASE= 0;
my $PE3_OS_MEM_START 	= $PE3_OS_MEM_START_BASE + $PE3_OS*(2**13);
#my $PE3_W_MEM_START_BASE= $MEM_H4B1_START;
#my $PE3_W_MEM_START_BASE= $MEM_H2B1_START;
my $PE3_W_MEM_START  	= $PE3_W_MEM_START_BASE  + $PE3_W*(2**13);
my $PE3_W_MEM_SIZE	= int($PE3_I_MEM_SIZE*$PE3_I_DATApWORD/($PE3_W_DATApWORD*2*$PE3_FFT_NUM));
#my $W_PREC		= 0;
my $PE3_MAC_OFFSET	= 0;
my $PE3_MAC_NLI		= 0;
my $PE3_MAC_ROW_NUM	= 0;
my $PE3_FFT_I_UNIT_SIZE = int($PE3_I_MEM_SIZE/$PE3_FFT_NUM);
my $PE3_FFT_O_UNIT_SIZE	= int($PE3_O_MEM_SIZE/$PE3_FFT_NUM);
my $PE3_SHIFT		= int($PE3_I_EXP + $PE3_W_EXP - $PE3_O_EXP) - 1;
my $PE3_SHIFT_OS 	= 0;
my $PE3_SHIFT_NL	= 0;
#
print 	"\n";
print	sprintf("PE3_TO_MEM_START = %d\n", $PE3_TO_MEM_START + 3*(2**13));
print	sprintf("PE3_TO_MEM_END = %d\n", $PE3_TO_MEM_START + 3*(2**13) + $PE3_TO_MEM_SIZE - 1);
print	sprintf("PE3_I_MEM_START  = %d\n", $PE3_I_MEM_START);
print	sprintf("PE3_I_MEM_END    = %d\n", $PE3_I_MEM_START + $PE3_I_MEM_SIZE - 1);
print	sprintf("PE3_O_MEM_START  = %d\n", $PE3_O_MEM_START);
print	sprintf("PE3_O_MEM_END    = %d\n", $PE3_O_MEM_START + $PE3_O_MEM_SIZE - 1);
print	sprintf("PE3_W_MEM_START  = %d\n", $PE3_W_MEM_START);
print	sprintf("PE3_W_MEM_END    = %d\n", $PE3_W_MEM_START + $PE3_W_MEM_SIZE - 1);
print	sprintf("PE3_OS_MEM_START = %d\n", $PE3_OS_MEM_START);
print	sprintf("PE3_OS_MEM_END   = %d\n", $PE3_OS_MEM_START + $PE3_O_MEM_SIZE - 1);
#
my $PE3_MAC_EN = 0;
#
# CREATE INSTRUCTION
$PE3_INST_0 = "";
$PE3_INST_0 = join "", sprintf("%2.2b",$PE3_OP), $PE3_INST_0;
if ($PE3_MAC_EN) {
	$PE3_INST_0 = join "", sprintf("%13.13b",$PE3_TO_MEM_START), $PE3_INST_0;
	$PE3_INST_0 = join "", sprintf("%13.13b",$PE3_TO_MEM_SIZE), $PE3_INST_0;
}else {
	$PE3_INST_0 = join "", sprintf("%13.13b",$PE3_FFT_NUM), $PE3_INST_0;
	$PE3_INST_0 = join "", sprintf("%13.13b",$PE3_FFT_I_UNIT_SIZE), $PE3_INST_0;
}
$PE3_INST_0 = join "", sprintf("%3.3b",$PE3_TO_PREC), $PE3_INST_0;
$PE3_INST_0 = join "", sprintf("%1.1b",$PE3_I_MEM_START & 0x1), $PE3_INST_0;
#
$PE3_INST_1 = "";
$PE3_INST_1 = join "", sprintf("%14.14b",int($PE3_I_MEM_START/2) & (2**14-1)), $PE3_INST_1;
$PE3_INST_1 = join "", sprintf("%15.15b",$PE3_I_MEM_SIZE), $PE3_INST_1;
$PE3_INST_1 = join "", sprintf("%3.3b",$PE3_I_PREC), $PE3_INST_1;
#
$PE3_INST_2 = "";
$PE3_INST_2 = join "", sprintf("%15.15b",$PE3_O_MEM_START), $PE3_INST_2;
$PE3_INST_2 = join "", sprintf("%15.15b",$PE3_O_MEM_SIZE), $PE3_INST_2;
$PE3_INST_2 = join "", sprintf("%2.2b",$PE3_O_PREC & 0x3), $PE3_INST_2;
#
$PE3_INST_3 = "";
$PE3_INST_3 = join "", sprintf("%1.1b",int($PE3_O_PREC/4) & 0x1), $PE3_INST_3;
$PE3_INST_3 = join "", sprintf("%1.1b",$PE3_O_SIGN), $PE3_INST_3;
if ($MAC_EN){
	$PE3_INST_3 = join "", sprintf("%15.15b",$PE3_OS_MEM_START), $PE3_INST_3;
} else {
	$PE3_INST_3 = join "", sprintf("%13.13b",$PE3_FFT_O_UNIT_SIZE), $PE3_INST_3;
	$PE3_INST_3 = join "", sprintf("%2.2b",0), $PE3_INST_3;
}
$PE3_INST_3 = join "", sprintf("%15.15b",$PE3_W_MEM_START), $PE3_INST_3;
#
$PE3_INST_4 = "";
$PE3_INST_4 = join "", sprintf("%15.15b",$PE3_W_MEM_SIZE), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%3.3b",$PE3_W_PREC), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%1.1b",$PE3_MAC_OFFSET), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%1.1b",$PE3_MAC_NLI), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%12.12b",$PE3_MAC_ROW_NUM & (2**12-1)), $PE3_INST_4;
#
$PE3_INST_5 = "";
$PE3_INST_5 = join "", sprintf("%3.3b",int($PE3_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT_OS), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT_NL), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%14.14b",0), $PE3_INST_5;
#

# PE_INST_LD
my $PE_INST_LD_0	= 0;
my $PE_INST_ADDR_0	= 0;
my $PE_INST_DATA_0	= "00000000000000000000000000000000";
my $PE_INST_SEL_0	= 0;
my $PE_INST_LD_1	= 0;
my $PE_INST_ADDR_1	= 0;
my $PE_INST_DATA_1	= "00000000000000000000000000000000";
my $PE_INST_SEL_1	= 0;
my $PE_INST_LD_2	= 0;
my $PE_INST_ADDR_2	= 0;
my $PE_INST_DATA_2	= "00000000000000000000000000000000";
my $PE_INST_SEL_2	= 0;
my $PE_INST_LD_3	= 0;
my $PE_INST_ADDR_3	= 0;
my $PE_INST_DATA_3	= "00000000000000000000000000000000";
my $PE_INST_SEL_3	= 0;

# PE_CONTROL
my $PE_INIT_0		= 0;
my $PE_INST_REPEAT_0	= 0;
my $PE_INST_START_0	= 0;
my $PE_INST_DONE_0	= 0;
my $PE_INST_STALL_0	= 0;
my $PE_INIT_1		= 0;
my $PE_INST_REPEAT_1	= 0;
my $PE_INST_START_1	= 0;
my $PE_INST_DONE_1	= 0;
my $PE_INST_STALL_1	= 0;
my $PE_INIT_2		= 0;
my $PE_INST_REPEAT_2	= 0;
my $PE_INST_START_2	= 0;
my $PE_INST_DONE_2	= 0;
my $PE_INST_STALL_2	= 0;
my $PE_INIT_3		= 0;
my $PE_INST_REPEAT_3	= 0;
my $PE_INST_START_3	= 0;
my $PE_INST_DONE_3	= 0;
my $PE_INST_STALL_3	= 0;

# NLI_LD
my $ADDR;
my $NLI_LD		= 0;
my $NLI_LD_ADDR		= 0;
my $NLI_LD_DATA_A	= 0;
my $NLI_LD_DATA_B	= 0;
my $NLI_LD_DATA_X	= 0;
my $NLI_LD_0		= 0;
my $NLI_LD_ADDR_0 	= 0;
my $NLI_LD_DATA_A_0	= 0;
my $NLI_LD_DATA_B_0	= 0;
my $NLI_LD_DATA_X_0 	= 0;
my $NLI_LD_1		= 0;
my $NLI_LD_ADDR_1	= 0;
my $NLI_LD_DATA_A_1	= 0;
my $NLI_LD_DATA_B_1	= 0;
my $NLI_LD_DATA_X_1 	= 0;
my $NLI_LD_2		= 0;
my $NLI_LD_ADDR_2	= 0;
my $NLI_LD_DATA_A_2	= 0;
my $NLI_LD_DATA_B_2	= 0;
my $NLI_LD_DATA_X_2	= 0;
my $NLI_LD_3		= 0;
my $NLI_LD_ADDR_3	= 0;
my $NLI_LD_DATA_A_3	= 0;
my $NLI_LD_DATA_B_3	= 0;
my $NLI_LD_DATA_X_3	= 0;

# CPU2ARB
my $CPU2ARB_DATA_0	= 0;
my $CPU2ARB_RD_0	= 0;
my $CPU2ARB_WR_0	= 0;
my $CPU2ARB_ADDR_0	= 0;
my $CPU2ARB_DATA_1	= 0;
my $CPU2ARB_RD_1	= 0;
my $CPU2ARB_WR_1	= 0;
my $CPU2ARB_ADDR_1	= 0;
my $CPU2ARB_DATA_2	= 0;
my $CPU2ARB_RD_2	= 0;
my $CPU2ARB_WR_2	= 0;
my $CPU2ARB_ADDR_2	= 0;
my $CPU2ARB_DATA_3	= 0;
my $CPU2ARB_RD_3	= 0;
my $CPU2ARB_WR_3	= 0;
my $CPU2ARB_ADDR_3	= 0;

my $INPUT_LINE;
my $INPUT_PART00;
my $INPUT_PART01;
my $INPUT_PART02;
my $INPUT_PART03;
my $INPUT_PART04;
my $INPUT_PART10;
my $INPUT_PART11;
my $INPUT_PART12;
my $INPUT_PART13;
my $INPUT_PART14;
my $INPUT_PART20;
my $INPUT_PART21;
my $INPUT_PART22;
my $INPUT_PART23;
my $INPUT_PART24;
my $INPUT_PART30;
my $INPUT_PART31;
my $INPUT_PART32;
my $INPUT_PART33;
my $INPUT_PART34;

# WRITE OUT_INST
#
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_0\t";}
if ($PE1_INST == 1){		print "$PE1_INST_0\t";}
if ($PE2_INST == 1){		print "$PE2_INST_0\t";}
if ($PE3_INST == 1){		print "$PE3_INST_0\t";}
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_1\t";}
if ($PE1_INST == 1){		print "$PE1_INST_1\t";}
if ($PE2_INST == 1){		print "$PE2_INST_1\t";}
if ($PE3_INST == 1){		print "$PE3_INST_1\t";}
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_2\t";}
if ($PE1_INST == 1){		print "$PE1_INST_2\t";}
if ($PE2_INST == 1){		print "$PE2_INST_2\t";}
if ($PE3_INST == 1){		print "$PE3_INST_2\t";}
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_3\t";}
if ($PE1_INST == 1){		print "$PE1_INST_3\t";}
if ($PE2_INST == 1){		print "$PE2_INST_3\t";}
if ($PE3_INST == 1){		print "$PE3_INST_3\t";}
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_4\t";}
if ($PE1_INST == 1){		print "$PE1_INST_4\t";}
if ($PE2_INST == 1){		print "$PE2_INST_4\t";}
if ($PE3_INST == 1){		print "$PE3_INST_4\t";}
print "\n";
if ($PE0_INST == 1){		print "$PE0_INST_5\t";}
if ($PE1_INST == 1){		print "$PE1_INST_5\t";}
if ($PE2_INST == 1){		print "$PE2_INST_5\t";}
if ($PE3_INST == 1){		print "$PE3_INST_5\t";}
print "\n";
print "\n";

#
#
my $PE_INST_ADDR = 0;
# PE_INST_LD = 0
$PE_INST_ADDR = 0;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_0;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_0;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_0;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_0;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);
#
# PE_INST_LD = 1
$PE_INST_ADDR = 1;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_1;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_1;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_1;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_1;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);

#
# PE_INST_LD = 2
$PE_INST_ADDR = 2;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_2;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_2;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_2;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_2;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);
#
# PE_INST_LD = 3
$PE_INST_ADDR = 3;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_3;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_3;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_3;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_3;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);
#
# PE_INST_LD = 4
$PE_INST_ADDR = 4;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_4;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_4;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_4;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_4;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);
#
# PE_INST_LD = 5
$PE_INST_ADDR = 5;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 1; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = $PE0_INST_5;	print "$PE_INST_DATA_0\t";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 1; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = $PE1_INST_5;	print "$PE_INST_DATA_1\t";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 1; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = $PE2_INST_5;	print "$PE_INST_DATA_2\t";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 1; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = $PE3_INST_5;	print "$PE_INST_DATA_3\t";}
print "\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);
#
# PE_INST_LD = DISABLE
$PE_INST_ADDR = 0;
if ($PE0_INST == 1) {	$PE_INST_LD_0 	= 0; 	$PE_INST_ADDR_0 = $PE_INST_ADDR;	$PE_INST_DATA_0 = "00000000000000000000000000000000";}
if ($PE1_INST == 1) {	$PE_INST_LD_1 	= 0; 	$PE_INST_ADDR_1 = $PE_INST_ADDR;	$PE_INST_DATA_1 = "00000000000000000000000000000000";}
if ($PE2_INST == 1) {	$PE_INST_LD_2 	= 0; 	$PE_INST_ADDR_2 = $PE_INST_ADDR;	$PE_INST_DATA_2 = "00000000000000000000000000000000";}
if ($PE3_INST == 1) {	$PE_INST_LD_3 	= 0; 	$PE_INST_ADDR_3 = $PE_INST_ADDR;	$PE_INST_DATA_3 = "00000000000000000000000000000000";}
print 	"\n";
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);

#
#
# PE_INST_START
if ($PE0_INST == 1) {	$PE_INST_START_0 = 1;	$PE_INST_STALL_0 = 0;}
if ($PE1_INST == 1) {	$PE_INST_START_1 = 1;	$PE_INST_STALL_1 = 0;}
if ($PE2_INST == 1) {	$PE_INST_START_2 = 1;	$PE_INST_STALL_2 = 0;}
if ($PE3_INST == 1) {	$PE_INST_START_3 = 1;	$PE_INST_STALL_3 = 0;}
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);

#
# PE_INST_START
if ($PE0_INST == 1) {	$PE_INST_START_0 = 0;	$PE_INST_STALL_0 = 0;}
if ($PE1_INST == 1) {	$PE_INST_START_1 = 0;	$PE_INST_STALL_1 = 0;}
if ($PE2_INST == 1) {	$PE_INST_START_2 = 0;	$PE_INST_STALL_2 = 0;}
if ($PE3_INST == 1) {	$PE_INST_START_3 = 0;	$PE_INST_STALL_3 = 0;}
printf OUT_INST sprintf("PE_INST_LD_0 = %.1b PE_INST_ADDR_0 = %4.4b PE_INST_DATA_0 = %32.32s PE_INST_SEL_0 = %.1b \n", $PE_INST_LD_0, $PE_INST_ADDR_0, $PE_INST_DATA_0, $PE_INST_SEL_0);
printf OUT_INST sprintf("PE_INIT_0 = %.1b PE_INST_REPEAT_0 = %.1b \n", $PE_INIT_0, $PE_INST_REPEAT_0);
printf OUT_INST sprintf("PE_INST_START_0 = %.1b PE_INST_STALL_0 = %.1b \n", $PE_INST_START_0, $PE_INST_STALL_0);
printf OUT_INST sprintf("NLI_LD_0 = %.1b NLI_LD_ADDR_0 = %4.4b NLI_LD_DATA_A_0 = %4.4x NLI_LD_DATA_B_0 = %8.8x NLI_LD_DATA_X_0 = %4.4x \n", $NLI_LD_0, $NLI_LD_ADDR_0, $NLI_LD_DATA_A_0 & 0xFFFF, $NLI_LD_DATA_B_0 & 0xFFFFFFFF, $NLI_LD_DATA_X_0 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_0 = %24.24x CPU2ARB_RD_0 = %.1b CPU2ARB_WR_0 = %.1b CPU2ARB_ADDR_0 = %13.13b \n", $CPU2ARB_DATA_0, $CPU2ARB_RD_0, $CPU2ARB_WR_0, $CPU2ARB_ADDR_0);
#
printf OUT_INST sprintf("PE_INST_LD_1 = %.1b PE_INST_ADDR_1 = %4.4b PE_INST_DATA_1 = %32.32s PE_INST_SEL_1 = %.1b \n", $PE_INST_LD_1, $PE_INST_ADDR_1, $PE_INST_DATA_1, $PE_INST_SEL_1);
printf OUT_INST sprintf("PE_INIT_1 = %.1b PE_INST_REPEAT_1 = %.1b \n", $PE_INIT_1, $PE_INST_REPEAT_1);
printf OUT_INST sprintf("PE_INST_START_1 = %.1b PE_INST_STALL_1 = %.1b \n", $PE_INST_START_1, $PE_INST_STALL_1);
printf OUT_INST sprintf("NLI_LD_1 = %.1b NLI_LD_ADDR_1 = %4.4b NLI_LD_DATA_A_1 = %4.4x NLI_LD_DATA_B_1 = %8.8x NLI_LD_DATA_X_1 = %4.4x \n", $NLI_LD_1, $NLI_LD_ADDR_1, $NLI_LD_DATA_A_1 & 0xFFFF, $NLI_LD_DATA_B_1 & 0xFFFFFFFF, $NLI_LD_DATA_X_1 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_1 = %24.24x CPU2ARB_RD_1 = %.1b CPU2ARB_WR_1 = %.1b CPU2ARB_ADDR_1 = %13.13b \n", $CPU2ARB_DATA_1, $CPU2ARB_RD_1, $CPU2ARB_WR_1, $CPU2ARB_ADDR_1);
#
printf OUT_INST sprintf("PE_INST_LD_2 = %.1b PE_INST_ADDR_2 = %4.4b PE_INST_DATA_2 = %32.32s PE_INST_SEL_2 = %.1b \n", $PE_INST_LD_2, $PE_INST_ADDR_2, $PE_INST_DATA_2, $PE_INST_SEL_2);
printf OUT_INST sprintf("PE_INIT_2 = %.1b PE_INST_REPEAT_2 = %.1b \n", $PE_INIT_2, $PE_INST_REPEAT_2);
printf OUT_INST sprintf("PE_INST_START_2 = %.1b PE_INST_STALL_2 = %.1b \n", $PE_INST_START_2, $PE_INST_STALL_2);
printf OUT_INST sprintf("NLI_LD_2 = %.1b NLI_LD_ADDR_2 = %4.4b NLI_LD_DATA_A_2 = %4.4x NLI_LD_DATA_B_2 = %8.8x NLI_LD_DATA_X_2 = %4.4x \n", $NLI_LD_2, $NLI_LD_ADDR_2, $NLI_LD_DATA_A_2 & 0xFFFF, $NLI_LD_DATA_B_2 & 0xFFFFFFFF, $NLI_LD_DATA_X_2 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_2 = %24.24x CPU2ARB_RD_2 = %.1b CPU2ARB_WR_2 = %.1b CPU2ARB_ADDR_2 = %13.13b \n", $CPU2ARB_DATA_2, $CPU2ARB_RD_2, $CPU2ARB_WR_2, $CPU2ARB_ADDR_2);
#
printf OUT_INST sprintf("PE_INST_LD_3 = %.1b PE_INST_ADDR_3 = %4.4b PE_INST_DATA_3 = %32.32s PE_INST_SEL_3 = %.1b \n", $PE_INST_LD_3, $PE_INST_ADDR_3, $PE_INST_DATA_3, $PE_INST_SEL_3);
printf OUT_INST sprintf("PE_INIT_3 = %.1b PE_INST_REPEAT_3 = %.1b \n", $PE_INIT_3, $PE_INST_REPEAT_3);
printf OUT_INST sprintf("PE_INST_START_3 = %.1b PE_INST_STALL_3 = %.1b \n", $PE_INST_START_3, $PE_INST_STALL_3);
printf OUT_INST sprintf("NLI_LD_3 = %.1b NLI_LD_ADDR_3 = %4.4b NLI_LD_DATA_A_3 = %4.4x NLI_LD_DATA_B_3 = %8.8x NLI_LD_DATA_X_3 = %4.4x \n", $NLI_LD_3, $NLI_LD_ADDR_3, $NLI_LD_DATA_A_3 & 0xFFFF, $NLI_LD_DATA_B_3 & 0xFFFFFFFF, $NLI_LD_DATA_X_3 & 0xFFFF);
printf OUT_INST sprintf("CPU2ARB_DATA_3 = %24.24x CPU2ARB_RD_3 = %.1b CPU2ARB_WR_3 = %.1b CPU2ARB_ADDR_3 = %13.13b \n", $CPU2ARB_DATA_3, $CPU2ARB_RD_3, $CPU2ARB_WR_3, $CPU2ARB_ADDR_3);

close (OUT_INST);

#########################
# caogao
if ($PE0_INST == 1){
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][0] = 0b%32.32s;\n", $PE0_INST_0);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][1] = 0b%32.32s;\n", $PE0_INST_1);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][2] = 0b%32.32s;\n", $PE0_INST_2);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][3] = 0b%32.32s;\n", $PE0_INST_3);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][4] = 0b%32.32s;\n", $PE0_INST_4);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][0][5] = 0b%32.32s;\n", $PE0_INST_5);
}
if ($PE1_INST == 1){
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][0] = 0b%32.32s;\n", $PE1_INST_0);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][1] = 0b%32.32s;\n", $PE1_INST_1);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][2] = 0b%32.32s;\n", $PE1_INST_2);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][3] = 0b%32.32s;\n", $PE1_INST_3);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][4] = 0b%32.32s;\n", $PE1_INST_4);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][1][5] = 0b%32.32s;\n", $PE1_INST_5);
}
if ($PE2_INST == 1){
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][0] = 0b%32.32s;\n", $PE2_INST_0);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][1] = 0b%32.32s;\n", $PE2_INST_1);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][2] = 0b%32.32s;\n", $PE2_INST_2);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][3] = 0b%32.32s;\n", $PE2_INST_3);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][4] = 0b%32.32s;\n", $PE2_INST_4);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][2][5] = 0b%32.32s;\n", $PE2_INST_5);
}
if ($PE3_INST == 1){
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][0] = 0b%32.32s;\n", $PE3_INST_0);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][1] = 0b%32.32s;\n", $PE3_INST_1);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][2] = 0b%32.32s;\n", $PE3_INST_2);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][3] = 0b%32.32s;\n", $PE3_INST_3);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][4] = 0b%32.32s;\n", $PE3_INST_4);
  printf OUT_DNN_INST sprintf("PE_INSTS[i][3][5] = 0b%32.32s;\n", $PE3_INST_5);
}
#########################


# DATA GENERATION & MEMORY PRINT
#
my $p=0;
my $q=0;
my $k=0;
my $PE0_I_SELECTED;
my $PE1_I_SELECTED;
my $PE2_I_SELECTED;
my $PE3_I_SELECTED;

my $LINE_MEM;
my $LINES_MEM;
my @PE0_MEM_LINES 	= ();
my @PE1_MEM_LINES 	= ();
my @PE2_MEM_LINES 	= ();
my @PE3_MEM_LINES 	= ();

my @PE0_LINE_O 		= ();
my @PE1_LINE_O 		= ();
my @PE2_LINE_O 		= ();
my @PE3_LINE_O 		= ();

#
#
# GENERATE RANDOM DATA FOR X0, X1, W OF FFT
# X0, X1, W ARE COMPLX NUMBERS WITH REAL AND IMAG PARTS
# PE0
#
my @PE0_DATA_I0_RE 	= ();
my @PE0_DATA_I0_IM 	= ();
my @PE0_DATA_I1_RE 	= ();
my @PE0_DATA_I1_IM 	= ();
my @PE0_DATA_O0_RE	= ();
my @PE0_DATA_O0_IM	= ();
my @PE0_DATA_O1_RE	= ();
my @PE0_DATA_O1_IM	= ();
my @PE0_DATA_W_RE 	= ();
my @PE0_DATA_W_IM 	= ();
#i
# FFT DATA RANDOM GENERATION & CALCULATION
#
my $PE0_FFT_PT = $PE0_W_MEM_SIZE*$PE0_W_DATApWORD;
print "PE0 ====================\n";
print sprintf("FFT PT = %d\n", $PE0_FFT_PT);
print sprintf("FFT NUM = %d\n", $PE0_FFT_NUM);
print "\n";
if ($PE0_W_PREC == $PREC_06B) 	{print "PE0_W_PREC = 06B\t";	print "PE0_DATApWORD = 8\n";}
if ($PE0_W_PREC == $PREC_08B) 	{print "PE0_W_PREC = 08B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_W_PREC == $PREC_12B) 	{print "PE0_W_PREC = 12B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_W_PREC == $PREC_16B) 	{print "PE0_W_PREC = 16B\t";	print "PE0_DATApWORD = 2\n";}
if ($PE0_I_PREC == $PREC_06B) 	{print "PE0_I_PREC = 06B\t";	print "PE0_DATApWORD = 8\n";}
if ($PE0_I_PREC == $PREC_08B) 	{print "PE0_I_PREC = 08B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_I_PREC == $PREC_12B) 	{print "PE0_I_PREC = 12B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_I_PREC == $PREC_16B) 	{print "PE0_I_PREC = 16B\t";	print "PE0_DATApWORD = 2\n";}
if ($PE0_O_PREC == $PREC_06B) 	{print "PE0_O_PREC = 06B\t";	print "PE0_DATApWORD = 8\n";}
if ($PE0_O_PREC == $PREC_08B) 	{print "PE0_O_PREC = 08B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_O_PREC == $PREC_12B) 	{print "PE0_O_PREC = 12B\t";	print "PE0_DATApWORD = 4\n";}
if ($PE0_O_PREC == $PREC_16B) 	{print "PE0_O_PREC = 16B\t";	print "PE0_DATApWORD = 2\n";}
print "\n";
print sprintf("TOTAL W = %d\n", $PE0_W_MEM_SIZE*$PE0_W_DATApWORD);
print sprintf("TOTAL I0&I1 PAIR = %d\n", $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2);
print sprintf("TOTAL O0&O1 PAIR = %d\n", $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2);
print "\n";
print sprintf("TOTAL WORD SIZE OF W = %d\n", $PE0_W_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I = %d\n", $PE0_I_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF O = %d\n", $PE0_O_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I_UNIT = %d\n", $PE0_FFT_I_UNIT_SIZE);
print sprintf("TOTAL WORD SIZE OF O_UNIT = %d\n", $PE0_FFT_O_UNIT_SIZE);
print "\n";
##################
## caogao
##
$LINE = <$weight_file>;			# size 
my @weight_line = split / /,$LINE;		## TODO: check number of variables
##
for($i = 0; $i < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD; $i++){
	$PE0_DATA_W_RE[$i] = int(rand(2**($PE0_W_EXP)-1) - 2**($PE0_W_EXP-1));
	$PE0_DATA_W_IM[$i] = int(rand(2**($PE0_W_EXP)-1) - 2**($PE0_W_EXP-1));
	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE0_DATA_W_RE[$i]&0xFF, $PE0_DATA_W_IM[$i]&0xFF);
}
##
$LINE = <$input_file>;			# size 
my @input_line = split / /,$LINE;		## TODO: check number of variables
##
for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){
	$PE0_DATA_I0_RE[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I0_IM[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I1_RE[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I1_IM[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
##
	$PE0_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE0_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE0_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE0_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
##
}
#
my $PE0_W_RE;
my $PE0_W_IM;
$j = 0;
for($i = 0; $i < $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2; $i++){
	$PE0_W_RE = $PE0_DATA_W_RE[$j];
	$PE0_W_IM = $PE0_DATA_W_IM[$j];
	#
	if ($PE0_DBG){
		print sprintf("At j=%4.4d: W_RE = %.d\tW_IM = %.d\n",$j,$PE0_W_RE,$PE0_W_IM);
	}
	$j++;
	#
  ## debug 58: changed floor to ceil
	$PE0_DATA_O0_RE[$i] = ceil(($PE0_DATA_I0_RE[$i] + ($PE0_W_RE*$PE0_DATA_I1_RE[$i] - $PE0_W_IM*$PE0_DATA_I1_IM[$i]))/(2**$PE0_SHIFT));
	$PE0_DATA_O1_RE[$i] = ceil(($PE0_DATA_I0_RE[$i] - ($PE0_W_RE*$PE0_DATA_I1_RE[$i] - $PE0_W_IM*$PE0_DATA_I1_IM[$i]))/(2**$PE0_SHIFT));
	$PE0_DATA_O0_IM[$i] = ceil(($PE0_DATA_I0_IM[$i] + ($PE0_W_RE*$PE0_DATA_I1_IM[$i] + $PE0_W_IM*$PE0_DATA_I1_RE[$i]))/(2**$PE0_SHIFT));
	$PE0_DATA_O1_IM[$i] = ceil(($PE0_DATA_I0_IM[$i] - ($PE0_W_RE*$PE0_DATA_I1_IM[$i] + $PE0_W_IM*$PE0_DATA_I1_RE[$i]))/(2**$PE0_SHIFT));
  ## debug 58
	#$PE0_DATA_O0_RE[$i] = ($PE0_DATA_I0_RE[$i] + ($PE0_W_RE*$PE0_DATA_I1_RE[$i] - $PE0_W_IM*$PE0_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE0_DATA_O1_RE[$i] = ($PE0_DATA_I0_RE[$i] - ($PE0_W_RE*$PE0_DATA_I1_RE[$i] - $PE0_W_IM*$PE0_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE0_DATA_O0_IM[$i] = ($PE0_DATA_I0_IM[$i] + ($PE0_W_RE*$PE0_DATA_I1_IM[$i] + $PE0_W_IM*$PE0_DATA_I1_RE[$i]))/(2**$SHIFT);
	#$PE0_DATA_O1_IM[$i] = ($PE0_DATA_I0_IM[$i] - ($PE0_W_RE*$PE0_DATA_I1_IM[$i] + $PE0_W_IM*$PE0_DATA_I1_RE[$i]))/(2**$SHIFT);
	#
	if($PE0_DATA_O0_RE[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O0_RE[$i] = 2**($PE0_O_EXP-1)-1;}
	if($PE0_DATA_O0_RE[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O0_RE[$i] = -2**($PE0_O_EXP-1);}
	if($PE0_DATA_O0_IM[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O0_IM[$i] = 2**($PE0_O_EXP-1)-1;}
	if($PE0_DATA_O0_IM[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O0_IM[$i] = -2**($PE0_O_EXP-1);}
	if($PE0_DATA_O1_RE[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O1_RE[$i] = 2**($PE0_O_EXP-1)-1;}
	if($PE0_DATA_O1_RE[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O1_RE[$i] = -2**($PE0_O_EXP-1);}
	if($PE0_DATA_O1_IM[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O1_IM[$i] = 2**($PE0_O_EXP-1)-1;}
	if($PE0_DATA_O1_IM[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O1_IM[$i] = -2**($PE0_O_EXP-1);}
	#
	if ($PE0_DBG){	
		print sprintf("At i=%4.4d: I0_RE = %.d\tI0_IM = %.d\n",$i,$PE0_DATA_I0_RE[$i],$PE0_DATA_I0_IM[$i]);
		print sprintf("At i=%4.4d: I1_RE = %.d\tI1_IM = %.d\n",$i,$PE0_DATA_I1_RE[$i],$PE0_DATA_I1_IM[$i]);
		print sprintf("At i=%4.4d: O0_RE = %.d\tO0_IM = %.d\n",$i,$PE0_DATA_O0_RE[$i],$PE0_DATA_O0_IM[$i]);
		print sprintf("At i=%4.4d: O1_RE = %.d\tO1_IM = %.d\n",$i,$PE0_DATA_O1_RE[$i],$PE0_DATA_O1_IM[$i]);
		#if($j == $FFT_NUM){	$j = 0;}
		if($j == $PE0_W_MEM_SIZE*$PE0_W_DATApWORD){	print "\n";}
		#print "\n";
	}
	if($j == $PE0_W_MEM_SIZE*$PE0_W_DATApWORD){	$j = 0;}
}
#
if ($PE0_DBG){
	print "PE0_DATA_W\n";
	for($i = 0; $i < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD; $i++){	
		print sprintf("PE0_DATA_W_IM[%i] = %4.4x\t",$i,$PE0_DATA_W_IM[$i]&0xFFFF);
		print sprintf("PE0_DATA_W_RE[%i] = %4.4x\t",$i,$PE0_DATA_W_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_I0\n";
	for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_I0_IM[%i] = %4.4x\t",$i,$PE0_DATA_I0_IM[$i]&0xFFFF);
		print sprintf("PE0_DATA_I0_RE[%i] = %4.4x\t",$i,$PE0_DATA_I0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_I1\n";
	for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_I1_IM[%i] = %4.4x\t",$i,$PE0_DATA_I1_IM[$i]&0xFFFF);
		print sprintf("PE0_DATA_I1_RE[%i] = %4.4x\t",$i,$PE0_DATA_I1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_O0\n";
	for($i = 0; $i < $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_O0_IM[%i] = %4.4x\t",$i,$PE0_DATA_O0_IM[$i]&0xFFFF);
		print sprintf("PE0_DATA_O0_RE[%i] = %4.4x\t",$i,$PE0_DATA_O0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_O1\n";
	for($i = 0; $i < $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_O1_IM[%i] = %4.4x\t",$i,$PE0_DATA_O1_IM[$i]&0xFFFF);
		print sprintf("PE0_DATA_O1_RE[%i] = %4.4x\t",$i,$PE0_DATA_O1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
}
#
if ($PE0_DBG){
	print "\n";
	print "PE0_DATA_W\n";
	for($i = 0; $i < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD; $i++){	
		print sprintf("PE0_DATA_W_IM[%i] = %.d\t",$i,$PE0_DATA_W_IM[$i]);
		print sprintf("PE0_DATA_W_RE[%i] = %.d\t",$i,$PE0_DATA_W_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_I0\n";
	for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_I0_IM[%i] = %.d\t",$i,$PE0_DATA_I0_IM[$i]);
		print sprintf("PE0_DATA_I0_RE[%i] = %.d\t",$i,$PE0_DATA_I0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_I1\n";
	for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_I1_IM[%i] = %.d\t",$i,$PE0_DATA_I1_IM[$i]);
		print sprintf("PE0_DATA_I1_RE[%i] = %.d\t",$i,$PE0_DATA_I1_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_O0\n";
	for($i = 0; $i < $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_O0_IM[%i] = %.d\t",$i,$PE0_DATA_O0_IM[$i]);
		print sprintf("PE0_DATA_O0_RE[%i] = %.d\t",$i,$PE0_DATA_O0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE0_DATA_O1\n";
	for($i = 0; $i < $PE0_O_MEM_SIZE*$PE0_O_DATApWORD/2; $i++){	
		print sprintf("PE0_DATA_O1_IM[%i] = %.d\t",$i,$PE0_DATA_O1_IM[$i]);
		print sprintf("PE0_DATA_O1_RE[%i] = %.d\t",$i,$PE0_DATA_O1_RE[$i]);
	}
	print "\n";
	print "\n";
}
#
#
# GENERATE RANDOM DATA FOR X0, X1, W OF FFT
# X0, X1, W ARE COMPLX NUMBERS WITH REAL AND IMAG PARTS
# PE1
#
my @PE1_DATA_I0_RE 	= ();
my @PE1_DATA_I0_IM 	= ();
my @PE1_DATA_I1_RE 	= ();
my @PE1_DATA_I1_IM 	= ();
my @PE1_DATA_O0_RE	= ();
my @PE1_DATA_O0_IM	= ();
my @PE1_DATA_O1_RE	= ();
my @PE1_DATA_O1_IM	= ();
my @PE1_DATA_W_RE 	= ();
my @PE1_DATA_W_IM 	= ();
#
# FFT DATA RANDOM GENERATION & CALCULATION
#
my $PE1_FFT_PT = $PE1_W_MEM_SIZE*$PE1_W_DATApWORD;
print "PE1 ====================\n";
print sprintf("FFT PT = %d\n", $PE1_FFT_PT);
print sprintf("FFT NUM = %d\n", $PE1_FFT_NUM);
print "\n";
if ($PE1_W_PREC == $PREC_06B) 	{print "PE1_W_PREC = 06B\t";	print "PE1_DATApWORD = 8\n";}
if ($PE1_W_PREC == $PREC_08B) 	{print "PE1_W_PREC = 08B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_W_PREC == $PREC_12B) 	{print "PE1_W_PREC = 12B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_W_PREC == $PREC_16B) 	{print "PE1_W_PREC = 16B\t";	print "PE1_DATApWORD = 2\n";}
if ($PE1_I_PREC == $PREC_06B) 	{print "PE1_I_PREC = 06B\t";	print "PE1_DATApWORD = 8\n";}
if ($PE1_I_PREC == $PREC_08B) 	{print "PE1_I_PREC = 08B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_I_PREC == $PREC_12B) 	{print "PE1_I_PREC = 12B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_I_PREC == $PREC_16B) 	{print "PE1_I_PREC = 16B\t";	print "PE1_DATApWORD = 2\n";}
if ($PE1_O_PREC == $PREC_06B) 	{print "PE1_O_PREC = 06B\t";	print "PE1_DATApWORD = 8\n";}
if ($PE1_O_PREC == $PREC_08B) 	{print "PE1_O_PREC = 08B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_O_PREC == $PREC_12B) 	{print "PE1_O_PREC = 12B\t";	print "PE1_DATApWORD = 4\n";}
if ($PE1_O_PREC == $PREC_16B) 	{print "PE1_O_PREC = 16B\t";	print "PE1_DATApWORD = 2\n";}
print "\n";
print sprintf("TOTAL W = %d\n", $PE1_W_MEM_SIZE*$PE1_W_DATApWORD);
print sprintf("TOTAL I0&I1 PAIR = %d\n", $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2);
print sprintf("TOTAL O0&O1 PAIR = %d\n", $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2);
print "\n";
print sprintf("TOTAL WORD SIZE OF W = %d\n", $PE1_W_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I = %d\n", $PE1_I_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF O = %d\n", $PE1_O_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I_UNIT = %d\n", $PE1_FFT_I_UNIT_SIZE);
print sprintf("TOTAL WORD SIZE OF O_UNIT = %d\n", $PE1_FFT_O_UNIT_SIZE);
print "\n";
#
for($i = 0; $i < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD; $i++){
	$PE1_DATA_W_RE[$i] = int(rand(2**($PE1_W_EXP)-1) - 2**($PE1_W_EXP-1));
	$PE1_DATA_W_IM[$i] = int(rand(2**($PE1_W_EXP)-1) - 2**($PE1_W_EXP-1));
##
	$PE1_DATA_W_RE[$i] = $weight_line[2 * $i]; 
	$PE1_DATA_W_IM[$i] = $weight_line[2 * $i + 1]; 
##
	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE1_DATA_W_RE[$i]&0xFF, $PE1_DATA_W_IM[$i]&0xFF);
}
for($i = 0; $i < $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2; $i++){
	$PE1_DATA_I0_RE[$i] = int(rand(2**($PE1_I_EXP)-1) - 2**($PE1_I_EXP-1));
	$PE1_DATA_I0_IM[$i] = int(rand(2**($PE1_I_EXP)-1) - 2**($PE1_I_EXP-1));
	$PE1_DATA_I1_RE[$i] = int(rand(2**($PE1_I_EXP)-1) - 2**($PE1_I_EXP-1));
	$PE1_DATA_I1_IM[$i] = int(rand(2**($PE1_I_EXP)-1) - 2**($PE1_I_EXP-1));
##
	$PE1_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE1_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE1_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE1_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
##
}
#
my $PE1_W_RE;
my $PE1_W_IM;
$j = 0;
for($i = 0; $i < $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2; $i++){
	$PE1_W_RE = $PE1_DATA_W_RE[$j];
	$PE1_W_IM = $PE1_DATA_W_IM[$j];
	#
	if ($PE1_DBG){
		print sprintf("At j=%4.4d: W_RE = %.d\tW_IM = %.d\n",$j,$PE1_W_RE,$PE1_W_IM);
	}
	$j++;
	#
	$PE1_DATA_O0_RE[$i] = ceil(($PE1_DATA_I0_RE[$i] + ($PE1_W_RE*$PE1_DATA_I1_RE[$i] - $PE1_W_IM*$PE1_DATA_I1_IM[$i]))/(2**$PE1_SHIFT));
	$PE1_DATA_O1_RE[$i] = ceil(($PE1_DATA_I0_RE[$i] - ($PE1_W_RE*$PE1_DATA_I1_RE[$i] - $PE1_W_IM*$PE1_DATA_I1_IM[$i]))/(2**$PE1_SHIFT));
	$PE1_DATA_O0_IM[$i] = ceil(($PE1_DATA_I0_IM[$i] + ($PE1_W_RE*$PE1_DATA_I1_IM[$i] + $PE1_W_IM*$PE1_DATA_I1_RE[$i]))/(2**$PE1_SHIFT));
	$PE1_DATA_O1_IM[$i] = ceil(($PE1_DATA_I0_IM[$i] - ($PE1_W_RE*$PE1_DATA_I1_IM[$i] + $PE1_W_IM*$PE1_DATA_I1_RE[$i]))/(2**$PE1_SHIFT));
	#$PE1_DATA_O0_RE[$i] = ($PE1_DATA_I0_RE[$i] + ($PE1_W_RE*$PE1_DATA_I1_RE[$i] - $PE1_W_IM*$PE1_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE1_DATA_O1_RE[$i] = ($PE1_DATA_I0_RE[$i] - ($PE1_W_RE*$PE1_DATA_I1_RE[$i] - $PE1_W_IM*$PE1_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE1_DATA_O0_IM[$i] = ($PE1_DATA_I0_IM[$i] + ($PE1_W_RE*$PE1_DATA_I1_IM[$i] + $PE1_W_IM*$PE1_DATA_I1_RE[$i]))/(2**$SHIFT);
	#$PE1_DATA_O1_IM[$i] = ($PE1_DATA_I0_IM[$i] - ($PE1_W_RE*$PE1_DATA_I1_IM[$i] + $PE1_W_IM*$PE1_DATA_I1_RE[$i]))/(2**$SHIFT);
	#
	if($PE1_DATA_O0_RE[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O0_RE[$i] = 2**($PE1_O_EXP-1)-1;}
	if($PE1_DATA_O0_RE[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O0_RE[$i] = -2**($PE1_O_EXP-1);}
	if($PE1_DATA_O0_IM[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O0_IM[$i] = 2**($PE1_O_EXP-1)-1;}
	if($PE1_DATA_O0_IM[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O0_IM[$i] = -2**($PE1_O_EXP-1);}
	if($PE1_DATA_O1_RE[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O1_RE[$i] = 2**($PE1_O_EXP-1)-1;}
	if($PE1_DATA_O1_RE[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O1_RE[$i] = -2**($PE1_O_EXP-1);}
	if($PE1_DATA_O1_IM[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O1_IM[$i] = 2**($PE1_O_EXP-1)-1;}
	if($PE1_DATA_O1_IM[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O1_IM[$i] = -2**($PE1_O_EXP-1);}
	#
	if ($PE1_DBG){	
		print sprintf("At i=%4.4d: I0_RE = %.d\tI0_IM = %.d\n",$i,$PE1_DATA_I0_RE[$i],$PE1_DATA_I0_IM[$i]);
		print sprintf("At i=%4.4d: I1_RE = %.d\tI1_IM = %.d\n",$i,$PE1_DATA_I1_RE[$i],$PE1_DATA_I1_IM[$i]);
		print sprintf("At i=%4.4d: O0_RE = %.d\tO0_IM = %.d\n",$i,$PE1_DATA_O0_RE[$i],$PE1_DATA_O0_IM[$i]);
		print sprintf("At i=%4.4d: O1_RE = %.d\tO1_IM = %.d\n",$i,$PE1_DATA_O1_RE[$i],$PE1_DATA_O1_IM[$i]);
		#if($j == $FFT_NUM){	$j = 0;}
		if($j == $PE1_W_MEM_SIZE*$PE1_W_DATApWORD){	print "\n";}
		#print "\n";
	}
	if($j == $PE1_W_MEM_SIZE*$PE1_W_DATApWORD){	$j = 0;}
}
#
if ($PE1_DBG){
	print "PE1_DATA_W\n";
	for($i = 0; $i < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD; $i++){	
		print sprintf("PE1_DATA_W_IM[%i] = %4.4x\t",$i,$PE1_DATA_W_IM[$i]&0xFFFF);
		print sprintf("PE1_DATA_W_RE[%i] = %4.4x\t",$i,$PE1_DATA_W_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_I0\n";
	for($i = 0; $i < $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_I0_IM[%i] = %4.4x\t",$i,$PE1_DATA_I0_IM[$i]&0xFFFF);
		print sprintf("PE1_DATA_I0_RE[%i] = %4.4x\t",$i,$PE1_DATA_I0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_I1\n";
	for($i = 0; $i < $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_I1_IM[%i] = %4.4x\t",$i,$PE1_DATA_I1_IM[$i]&0xFFFF);
		print sprintf("PE1_DATA_I1_RE[%i] = %4.4x\t",$i,$PE1_DATA_I1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_O0\n";
	for($i = 0; $i < $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_O0_IM[%i] = %4.4x\t",$i,$PE1_DATA_O0_IM[$i]&0xFFFF);
		print sprintf("PE1_DATA_O0_RE[%i] = %4.4x\t",$i,$PE1_DATA_O0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_O1\n";
	for($i = 0; $i < $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_O1_IM[%i] = %4.4x\t",$i,$PE1_DATA_O1_IM[$i]&0xFFFF);
		print sprintf("PE1_DATA_O1_RE[%i] = %4.4x\t",$i,$PE1_DATA_O1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
}
#
if ($PE1_DBG){
	print "\n";
	print "PE1_DATA_W\n";
	for($i = 0; $i < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD; $i++){	
		print sprintf("PE1_DATA_W_IM[%i] = %.d\t",$i,$PE1_DATA_W_IM[$i]);
		print sprintf("PE1_DATA_W_RE[%i] = %.d\t",$i,$PE1_DATA_W_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_I0\n";
	for($i = 0; $i < $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_I0_IM[%i] = %.d\t",$i,$PE1_DATA_I0_IM[$i]);
		print sprintf("PE1_DATA_I0_RE[%i] = %.d\t",$i,$PE1_DATA_I0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_I1\n";
	for($i = 0; $i < $PE1_I_MEM_SIZE*$PE1_I_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_I1_IM[%i] = %.d\t",$i,$PE1_DATA_I1_IM[$i]);
		print sprintf("PE1_DATA_I1_RE[%i] = %.d\t",$i,$PE1_DATA_I1_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_O0\n";
	for($i = 0; $i < $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_O0_IM[%i] = %.d\t",$i,$PE1_DATA_O0_IM[$i]);
		print sprintf("PE1_DATA_O0_RE[%i] = %.d\t",$i,$PE1_DATA_O0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE1_DATA_O1\n";
	for($i = 0; $i < $PE1_O_MEM_SIZE*$PE1_O_DATApWORD/2; $i++){	
		print sprintf("PE1_DATA_O1_IM[%i] = %.d\t",$i,$PE1_DATA_O1_IM[$i]);
		print sprintf("PE1_DATA_O1_RE[%i] = %.d\t",$i,$PE1_DATA_O1_RE[$i]);
	}
	print "\n";
	print "\n";
}
#
#
# GENERATE RANDOM DATA FOR X0, X1, W OF FFT
# X0, X1, W ARE COMPLX NUMBERS WITH REAL AND IMAG PARTS
# PE2
#
my @PE2_DATA_I0_RE 	= ();
my @PE2_DATA_I0_IM 	= ();
my @PE2_DATA_I1_RE 	= ();
my @PE2_DATA_I1_IM 	= ();
my @PE2_DATA_O0_RE	= ();
my @PE2_DATA_O0_IM	= ();
my @PE2_DATA_O1_RE	= ();
my @PE2_DATA_O1_IM	= ();
my @PE2_DATA_W_RE 	= ();
my @PE2_DATA_W_IM 	= ();
#
# FFT DATA RANDOM GENERATION & CALCULATION
#
my $PE2_FFT_PT = $PE2_W_MEM_SIZE*$PE2_W_DATApWORD;
print "PE2 ====================\n";
print sprintf("FFT PT = %d\n", $PE2_FFT_PT);
print sprintf("FFT NUM = %d\n", $PE2_FFT_NUM);
print "\n";
if ($PE2_W_PREC == $PREC_06B) 	{print "PE2_W_PREC = 06B\t";	print "PE2_DATApWORD = 8\n";}
if ($PE2_W_PREC == $PREC_08B) 	{print "PE2_W_PREC = 08B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_W_PREC == $PREC_12B) 	{print "PE2_W_PREC = 12B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_W_PREC == $PREC_16B) 	{print "PE2_W_PREC = 16B\t";	print "PE2_DATApWORD = 2\n";}
if ($PE2_I_PREC == $PREC_06B) 	{print "PE2_I_PREC = 06B\t";	print "PE2_DATApWORD = 8\n";}
if ($PE2_I_PREC == $PREC_08B) 	{print "PE2_I_PREC = 08B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_I_PREC == $PREC_12B) 	{print "PE2_I_PREC = 12B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_I_PREC == $PREC_16B) 	{print "PE2_I_PREC = 16B\t";	print "PE2_DATApWORD = 2\n";}
if ($PE2_O_PREC == $PREC_06B) 	{print "PE2_O_PREC = 06B\t";	print "PE2_DATApWORD = 8\n";}
if ($PE2_O_PREC == $PREC_08B) 	{print "PE2_O_PREC = 08B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_O_PREC == $PREC_12B) 	{print "PE2_O_PREC = 12B\t";	print "PE2_DATApWORD = 4\n";}
if ($PE2_O_PREC == $PREC_16B) 	{print "PE2_O_PREC = 16B\t";	print "PE2_DATApWORD = 2\n";}
print "\n";
print sprintf("TOTAL W = %d\n", $PE2_W_MEM_SIZE*$PE2_W_DATApWORD);
print sprintf("TOTAL I0&I1 PAIR = %d\n", $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2);
print sprintf("TOTAL O0&O1 PAIR = %d\n", $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2);
print "\n";
print sprintf("TOTAL WORD SIZE OF W = %d\n", $PE2_W_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I = %d\n", $PE2_I_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF O = %d\n", $PE2_O_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I_UNIT = %d\n", $PE2_FFT_I_UNIT_SIZE);
print sprintf("TOTAL WORD SIZE OF O_UNIT = %d\n", $PE2_FFT_O_UNIT_SIZE);
print "\n";
#
for($i = 0; $i < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD; $i++){
	$PE2_DATA_W_RE[$i] = int(rand(2**($PE2_W_EXP)-1) - 2**($PE2_W_EXP-1));
	$PE2_DATA_W_IM[$i] = int(rand(2**($PE2_W_EXP)-1) - 2**($PE2_W_EXP-1));
##
	$PE2_DATA_W_RE[$i] = $weight_line[2 * $i];
	$PE2_DATA_W_IM[$i] = $weight_line[2 * $i + 1];
##
	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE2_DATA_W_RE[$i]&0xFF, $PE2_DATA_W_IM[$i]&0xFF);
}
for($i = 0; $i < $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2; $i++){
	$PE2_DATA_I0_RE[$i] = int(rand(2**($PE2_I_EXP)-1) - 2**($PE2_I_EXP-1));
	$PE2_DATA_I0_IM[$i] = int(rand(2**($PE2_I_EXP)-1) - 2**($PE2_I_EXP-1));
	$PE2_DATA_I1_RE[$i] = int(rand(2**($PE2_I_EXP)-1) - 2**($PE2_I_EXP-1));
	$PE2_DATA_I1_IM[$i] = int(rand(2**($PE2_I_EXP)-1) - 2**($PE2_I_EXP-1));
##
	$PE2_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE2_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE2_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE2_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
##
}
#
my $PE2_W_RE;
my $PE2_W_IM;
$j = 0;
for($i = 0; $i < $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2; $i++){
	$PE2_W_RE = $PE2_DATA_W_RE[$j];
	$PE2_W_IM = $PE2_DATA_W_IM[$j];
	#
	if ($PE2_DBG){
		print sprintf("At j=%4.4d: W_RE = %.d\tW_IM = %.d\n",$j,$PE2_W_RE,$PE2_W_IM);
	}
	$j++;
	#
	$PE2_DATA_O0_RE[$i] = ceil(($PE2_DATA_I0_RE[$i] + ($PE2_W_RE*$PE2_DATA_I1_RE[$i] - $PE2_W_IM*$PE2_DATA_I1_IM[$i]))/(2**$PE2_SHIFT));
	$PE2_DATA_O1_RE[$i] = ceil(($PE2_DATA_I0_RE[$i] - ($PE2_W_RE*$PE2_DATA_I1_RE[$i] - $PE2_W_IM*$PE2_DATA_I1_IM[$i]))/(2**$PE2_SHIFT));
	$PE2_DATA_O0_IM[$i] = ceil(($PE2_DATA_I0_IM[$i] + ($PE2_W_RE*$PE2_DATA_I1_IM[$i] + $PE2_W_IM*$PE2_DATA_I1_RE[$i]))/(2**$PE2_SHIFT));
	$PE2_DATA_O1_IM[$i] = ceil(($PE2_DATA_I0_IM[$i] - ($PE2_W_RE*$PE2_DATA_I1_IM[$i] + $PE2_W_IM*$PE2_DATA_I1_RE[$i]))/(2**$PE2_SHIFT));
	#$PE2_DATA_O0_RE[$i] = ($PE2_DATA_I0_RE[$i] + ($PE2_W_RE*$PE2_DATA_I1_RE[$i] - $PE2_W_IM*$PE2_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE2_DATA_O1_RE[$i] = ($PE2_DATA_I0_RE[$i] - ($PE2_W_RE*$PE2_DATA_I1_RE[$i] - $PE2_W_IM*$PE2_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE2_DATA_O0_IM[$i] = ($PE2_DATA_I0_IM[$i] + ($PE2_W_RE*$PE2_DATA_I1_IM[$i] + $PE2_W_IM*$PE2_DATA_I1_RE[$i]))/(2**$SHIFT);
	#$PE2_DATA_O1_IM[$i] = ($PE2_DATA_I0_IM[$i] - ($PE2_W_RE*$PE2_DATA_I1_IM[$i] + $PE2_W_IM*$PE2_DATA_I1_RE[$i]))/(2**$SHIFT);
	#
	if($PE2_DATA_O0_RE[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O0_RE[$i] = 2**($PE2_O_EXP-1)-1;}
	if($PE2_DATA_O0_RE[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O0_RE[$i] = -2**($PE2_O_EXP-1);}
	if($PE2_DATA_O0_IM[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O0_IM[$i] = 2**($PE2_O_EXP-1)-1;}
	if($PE2_DATA_O0_IM[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O0_IM[$i] = -2**($PE2_O_EXP-1);}
	if($PE2_DATA_O1_RE[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O1_RE[$i] = 2**($PE2_O_EXP-1)-1;}
	if($PE2_DATA_O1_RE[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O1_RE[$i] = -2**($PE2_O_EXP-1);}
	if($PE2_DATA_O1_IM[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O1_IM[$i] = 2**($PE2_O_EXP-1)-1;}
	if($PE2_DATA_O1_IM[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O1_IM[$i] = -2**($PE2_O_EXP-1);}
	#
	if ($PE2_DBG){	
		print sprintf("At i=%4.4d: I0_RE = %.d\tI0_IM = %.d\n",$i,$PE2_DATA_I0_RE[$i],$PE2_DATA_I0_IM[$i]);
		print sprintf("At i=%4.4d: I1_RE = %.d\tI1_IM = %.d\n",$i,$PE2_DATA_I1_RE[$i],$PE2_DATA_I1_IM[$i]);
		print sprintf("At i=%4.4d: O0_RE = %.d\tO0_IM = %.d\n",$i,$PE2_DATA_O0_RE[$i],$PE2_DATA_O0_IM[$i]);
		print sprintf("At i=%4.4d: O1_RE = %.d\tO1_IM = %.d\n",$i,$PE2_DATA_O1_RE[$i],$PE2_DATA_O1_IM[$i]);
		#if($j == $FFT_NUM){	$j = 0;}
		if($j == $PE2_W_MEM_SIZE*$PE2_W_DATApWORD){	print "\n";}
		#print "\n";
	}
	if($j == $PE2_W_MEM_SIZE*$PE2_W_DATApWORD){	$j = 0;}
}
#
if ($PE2_DBG){
	print "PE2_DATA_W\n";
	for($i = 0; $i < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD; $i++){	
		print sprintf("PE2_DATA_W_IM[%i] = %4.4x\t",$i,$PE2_DATA_W_IM[$i]&0xFFFF);
		print sprintf("PE2_DATA_W_RE[%i] = %4.4x\t",$i,$PE2_DATA_W_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_I0\n";
	for($i = 0; $i < $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_I0_IM[%i] = %4.4x\t",$i,$PE2_DATA_I0_IM[$i]&0xFFFF);
		print sprintf("PE2_DATA_I0_RE[%i] = %4.4x\t",$i,$PE2_DATA_I0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_I1\n";
	for($i = 0; $i < $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_I1_IM[%i] = %4.4x\t",$i,$PE2_DATA_I1_IM[$i]&0xFFFF);
		print sprintf("PE2_DATA_I1_RE[%i] = %4.4x\t",$i,$PE2_DATA_I1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_O0\n";
	for($i = 0; $i < $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_O0_IM[%i] = %4.4x\t",$i,$PE2_DATA_O0_IM[$i]&0xFFFF);
		print sprintf("PE2_DATA_O0_RE[%i] = %4.4x\t",$i,$PE2_DATA_O0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_O1\n";
	for($i = 0; $i < $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_O1_IM[%i] = %4.4x\t",$i,$PE2_DATA_O1_IM[$i]&0xFFFF);
		print sprintf("PE2_DATA_O1_RE[%i] = %4.4x\t",$i,$PE2_DATA_O1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
}
#
if ($PE2_DBG){
	print "\n";
	print "PE2_DATA_W\n";
	for($i = 0; $i < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD; $i++){	
		print sprintf("PE2_DATA_W_IM[%i] = %.d\t",$i,$PE2_DATA_W_IM[$i]);
		print sprintf("PE2_DATA_W_RE[%i] = %.d\t",$i,$PE2_DATA_W_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_I0\n";
	for($i = 0; $i < $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_I0_IM[%i] = %.d\t",$i,$PE2_DATA_I0_IM[$i]);
		print sprintf("PE2_DATA_I0_RE[%i] = %.d\t",$i,$PE2_DATA_I0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_I1\n";
	for($i = 0; $i < $PE2_I_MEM_SIZE*$PE2_I_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_I1_IM[%i] = %.d\t",$i,$PE2_DATA_I1_IM[$i]);
		print sprintf("PE2_DATA_I1_RE[%i] = %.d\t",$i,$PE2_DATA_I1_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_O0\n";
	for($i = 0; $i < $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_O0_IM[%i] = %.d\t",$i,$PE2_DATA_O0_IM[$i]);
		print sprintf("PE2_DATA_O0_RE[%i] = %.d\t",$i,$PE2_DATA_O0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE2_DATA_O1\n";
	for($i = 0; $i < $PE2_O_MEM_SIZE*$PE2_O_DATApWORD/2; $i++){	
		print sprintf("PE2_DATA_O1_IM[%i] = %.d\t",$i,$PE2_DATA_O1_IM[$i]);
		print sprintf("PE2_DATA_O1_RE[%i] = %.d\t",$i,$PE2_DATA_O1_RE[$i]);
	}
	print "\n";
	print "\n";
}
#
#
# GENERATE RANDOM DATA FOR X0, X1, W OF FFT
# X0, X1, W ARE COMPLX NUMBERS WITH REAL AND IMAG PARTS
# PE3
#
my @PE3_DATA_I0_RE 	= ();
my @PE3_DATA_I0_IM 	= ();
my @PE3_DATA_I1_RE 	= ();
my @PE3_DATA_I1_IM 	= ();
my @PE3_DATA_O0_RE	= ();
my @PE3_DATA_O0_IM	= ();
my @PE3_DATA_O1_RE	= ();
my @PE3_DATA_O1_IM	= ();
my @PE3_DATA_W_RE 	= ();
my @PE3_DATA_W_IM 	= ();
#
# FFT DATA RANDOM GENERATION & CALCULATION
#
my $PE3_FFT_PT = $PE3_W_MEM_SIZE*$PE3_W_DATApWORD;
print "PE3 ====================\n";
print sprintf("FFT PT = %d\n", $PE3_FFT_PT);
print sprintf("FFT NUM = %d\n", $PE3_FFT_NUM);
print "\n";
if ($PE3_W_PREC == $PREC_06B) 	{print "PE3_W_PREC = 06B\t";	print "PE3_DATApWORD = 8\n";}
if ($PE3_W_PREC == $PREC_08B) 	{print "PE3_W_PREC = 08B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_W_PREC == $PREC_12B) 	{print "PE3_W_PREC = 12B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_W_PREC == $PREC_16B) 	{print "PE3_W_PREC = 16B\t";	print "PE3_DATApWORD = 2\n";}
if ($PE3_I_PREC == $PREC_06B) 	{print "PE3_I_PREC = 06B\t";	print "PE3_DATApWORD = 8\n";}
if ($PE3_I_PREC == $PREC_08B) 	{print "PE3_I_PREC = 08B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_I_PREC == $PREC_12B) 	{print "PE3_I_PREC = 12B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_I_PREC == $PREC_16B) 	{print "PE3_I_PREC = 16B\t";	print "PE3_DATApWORD = 2\n";}
if ($PE3_O_PREC == $PREC_06B) 	{print "PE3_O_PREC = 06B\t";	print "PE3_DATApWORD = 8\n";}
if ($PE3_O_PREC == $PREC_08B) 	{print "PE3_O_PREC = 08B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_O_PREC == $PREC_12B) 	{print "PE3_O_PREC = 12B\t";	print "PE3_DATApWORD = 4\n";}
if ($PE3_O_PREC == $PREC_16B) 	{print "PE3_O_PREC = 16B\t";	print "PE3_DATApWORD = 2\n";}
print "\n";
print sprintf("TOTAL W = %d\n", $PE3_W_MEM_SIZE*$PE3_W_DATApWORD);
print sprintf("TOTAL I0&I1 PAIR = %d\n", $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2);
print sprintf("TOTAL O0&O1 PAIR = %d\n", $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2);
print "\n";
print sprintf("TOTAL WORD SIZE OF W = %d\n", $PE3_W_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I = %d\n", $PE3_I_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF O = %d\n", $PE3_O_MEM_SIZE);
print sprintf("TOTAL WORD SIZE OF I_UNIT = %d\n", $PE3_FFT_I_UNIT_SIZE);
print sprintf("TOTAL WORD SIZE OF O_UNIT = %d\n", $PE3_FFT_O_UNIT_SIZE);
print "\n";
#
for($i = 0; $i < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD; $i++){
	$PE3_DATA_W_RE[$i] = int(rand(2**($PE3_W_EXP)-1) - 2**($PE3_W_EXP-1));
	$PE3_DATA_W_IM[$i] = int(rand(2**($PE3_W_EXP)-1) - 2**($PE3_W_EXP-1));
##
	$PE3_DATA_W_RE[$i] = $weight_line[2 * $i];
	$PE3_DATA_W_IM[$i] = $weight_line[2 * $i + 1];
##
	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE3_DATA_W_RE[$i]&0xFF, $PE3_DATA_W_IM[$i]&0xFF);
}
for($i = 0; $i < $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2; $i++){
	$PE3_DATA_I0_RE[$i] = int(rand(2**($PE3_I_EXP)-1) - 2**($PE3_I_EXP-1));
	$PE3_DATA_I0_IM[$i] = int(rand(2**($PE3_I_EXP)-1) - 2**($PE3_I_EXP-1));
	$PE3_DATA_I1_RE[$i] = int(rand(2**($PE3_I_EXP)-1) - 2**($PE3_I_EXP-1));
	$PE3_DATA_I1_IM[$i] = int(rand(2**($PE3_I_EXP)-1) - 2**($PE3_I_EXP-1));
##
	$PE3_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE3_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE3_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE3_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
##
}
#
my $PE3_W_RE;
my $PE3_W_IM;
$j = 0;
for($i = 0; $i < $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2; $i++){
	$PE3_W_RE = $PE3_DATA_W_RE[$j];
	$PE3_W_IM = $PE3_DATA_W_IM[$j];
	#
	if ($PE3_DBG){
		print sprintf("At j=%4.4d: W_RE = %.d\tW_IM = %.d\n",$j,$PE3_W_RE,$PE3_W_IM);
	}
	$j++;
	#
	$PE3_DATA_O0_RE[$i] = ceil(($PE3_DATA_I0_RE[$i] + ($PE3_W_RE*$PE3_DATA_I1_RE[$i] - $PE3_W_IM*$PE3_DATA_I1_IM[$i]))/(2**$PE3_SHIFT));
	$PE3_DATA_O1_RE[$i] = ceil(($PE3_DATA_I0_RE[$i] - ($PE3_W_RE*$PE3_DATA_I1_RE[$i] - $PE3_W_IM*$PE3_DATA_I1_IM[$i]))/(2**$PE3_SHIFT));
	$PE3_DATA_O0_IM[$i] = ceil(($PE3_DATA_I0_IM[$i] + ($PE3_W_RE*$PE3_DATA_I1_IM[$i] + $PE3_W_IM*$PE3_DATA_I1_RE[$i]))/(2**$PE3_SHIFT));
	$PE3_DATA_O1_IM[$i] = ceil(($PE3_DATA_I0_IM[$i] - ($PE3_W_RE*$PE3_DATA_I1_IM[$i] + $PE3_W_IM*$PE3_DATA_I1_RE[$i]))/(2**$PE3_SHIFT));
	#$PE3_DATA_O0_RE[$i] = ($PE3_DATA_I0_RE[$i] + ($PE3_W_RE*$PE3_DATA_I1_RE[$i] - $PE3_W_IM*$PE3_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE3_DATA_O1_RE[$i] = ($PE3_DATA_I0_RE[$i] - ($PE3_W_RE*$PE3_DATA_I1_RE[$i] - $PE3_W_IM*$PE3_DATA_I1_IM[$i]))/(2**$SHIFT);
	#$PE3_DATA_O0_IM[$i] = ($PE3_DATA_I0_IM[$i] + ($PE3_W_RE*$PE3_DATA_I1_IM[$i] + $PE3_W_IM*$PE3_DATA_I1_RE[$i]))/(2**$SHIFT);
	#$PE3_DATA_O1_IM[$i] = ($PE3_DATA_I0_IM[$i] - ($PE3_W_RE*$PE3_DATA_I1_IM[$i] + $PE3_W_IM*$PE3_DATA_I1_RE[$i]))/(2**$SHIFT);
	#
	if($PE3_DATA_O0_RE[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O0_RE[$i] = 2**($PE3_O_EXP-1)-1;}
	if($PE3_DATA_O0_RE[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O0_RE[$i] = -2**($PE3_O_EXP-1);}
	if($PE3_DATA_O0_IM[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O0_IM[$i] = 2**($PE3_O_EXP-1)-1;}
	if($PE3_DATA_O0_IM[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O0_IM[$i] = -2**($PE3_O_EXP-1);}
	if($PE3_DATA_O1_RE[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O1_RE[$i] = 2**($PE3_O_EXP-1)-1;}
	if($PE3_DATA_O1_RE[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O1_RE[$i] = -2**($PE3_O_EXP-1);}
	if($PE3_DATA_O1_IM[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O1_IM[$i] = 2**($PE3_O_EXP-1)-1;}
	if($PE3_DATA_O1_IM[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O1_IM[$i] = -2**($PE3_O_EXP-1);}
	#
	if ($PE3_DBG){	
		print sprintf("At i=%4.4d: I0_RE = %.d\tI0_IM = %.d\n",$i,$PE3_DATA_I0_RE[$i],$PE3_DATA_I0_IM[$i]);
		print sprintf("At i=%4.4d: I1_RE = %.d\tI1_IM = %.d\n",$i,$PE3_DATA_I1_RE[$i],$PE3_DATA_I1_IM[$i]);
		print sprintf("At i=%4.4d: O0_RE = %.d\tO0_IM = %.d\n",$i,$PE3_DATA_O0_RE[$i],$PE3_DATA_O0_IM[$i]);
		print sprintf("At i=%4.4d: O1_RE = %.d\tO1_IM = %.d\n",$i,$PE3_DATA_O1_RE[$i],$PE3_DATA_O1_IM[$i]);
		#if($j == $FFT_NUM){	$j = 0;}
		if($j == $PE3_W_MEM_SIZE*$PE3_W_DATApWORD){	print "\n";}
		#print "\n";
	}
	if($j == $PE3_W_MEM_SIZE*$PE3_W_DATApWORD){	$j = 0;}
}
#
if ($PE3_DBG){
	print "PE3_DATA_W\n";
	for($i = 0; $i < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD; $i++){	
		print sprintf("PE3_DATA_W_IM[%i] = %4.4x\t",$i,$PE3_DATA_W_IM[$i]&0xFFFF);
		print sprintf("PE3_DATA_W_RE[%i] = %4.4x\t",$i,$PE3_DATA_W_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_I0\n";
	for($i = 0; $i < $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_I0_IM[%i] = %4.4x\t",$i,$PE3_DATA_I0_IM[$i]&0xFFFF);
		print sprintf("PE3_DATA_I0_RE[%i] = %4.4x\t",$i,$PE3_DATA_I0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_I1\n";
	for($i = 0; $i < $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_I1_IM[%i] = %4.4x\t",$i,$PE3_DATA_I1_IM[$i]&0xFFFF);
		print sprintf("PE3_DATA_I1_RE[%i] = %4.4x\t",$i,$PE3_DATA_I1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_O0\n";
	for($i = 0; $i < $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_O0_IM[%i] = %4.4x\t",$i,$PE3_DATA_O0_IM[$i]&0xFFFF);
		print sprintf("PE3_DATA_O0_RE[%i] = %4.4x\t",$i,$PE3_DATA_O0_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_O1\n";
	for($i = 0; $i < $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_O1_IM[%i] = %4.4x\t",$i,$PE3_DATA_O1_IM[$i]&0xFFFF);
		print sprintf("PE3_DATA_O1_RE[%i] = %4.4x\t",$i,$PE3_DATA_O1_RE[$i]&0xFFFF);
	}
	print "\n";
	print "\n";
}
#
if (0){
	print "\n";
	print "PE3_DATA_W\n";
	for($i = 0; $i < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD; $i++){	
		print sprintf("PE3_DATA_W_IM[%i] = %.d\t",$i,$PE3_DATA_W_IM[$i]);
		print sprintf("PE3_DATA_W_RE[%i] = %.d\t",$i,$PE3_DATA_W_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_I0\n";
	for($i = 0; $i < $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_I0_IM[%i] = %.d\t",$i,$PE3_DATA_I0_IM[$i]);
		print sprintf("PE3_DATA_I0_RE[%i] = %.d\t",$i,$PE3_DATA_I0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_I1\n";
	for($i = 0; $i < $PE3_I_MEM_SIZE*$PE3_I_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_I1_IM[%i] = %.d\t",$i,$PE3_DATA_I1_IM[$i]);
		print sprintf("PE3_DATA_I1_RE[%i] = %.d\t",$i,$PE3_DATA_I1_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_O0\n";
	for($i = 0; $i < $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_O0_IM[%i] = %.d\t",$i,$PE3_DATA_O0_IM[$i]);
		print sprintf("PE3_DATA_O0_RE[%i] = %.d\t",$i,$PE3_DATA_O0_RE[$i]);
	}
	print "\n";
	print "\n";
	print "PE3_DATA_O1\n";
	for($i = 0; $i < $PE3_O_MEM_SIZE*$PE3_O_DATApWORD/2; $i++){	
		print sprintf("PE3_DATA_O1_IM[%i] = %.d\t",$i,$PE3_DATA_O1_IM[$i]);
		print sprintf("PE3_DATA_O1_RE[%i] = %.d\t",$i,$PE3_DATA_O1_RE[$i]);
	}
	print "\n";
	print "\n";
}


#########################
# caogao
#
# MEM_* INITIALIZATION
#
my $FILE;
if (not defined $ARGV[1]) {
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE0_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
  	$PE1_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
  	$PE2_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
  	$PE3_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
  }
} else {
  $FILE = $INPUT_MEM_FILE."_0.data";
  open (MEM_DATA, $FILE)
    or die "cannot open <", $FILE, ": $!";
  @LINES_DATA = <MEM_DATA>;
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE0_MEM_LINES[$i] = $LINES_DATA[$i];
    print $LINES_DATA[$i];
  }
  close(MEM_DATA);
  
  $FILE = $INPUT_MEM_FILE."_1.data";
  open (MEM_DATA, $FILE)
    or die "cannot open <", $FILE, ": $!";
  @LINES_DATA = <MEM_DATA>;
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE1_MEM_LINES[$i] = $LINES_DATA[$i];
  }
  close(MEM_DATA);
  
  $FILE = $INPUT_MEM_FILE."_2.data";
  open (MEM_DATA, $FILE)
    or die "cannot open <", $FILE, ": $!";
  @LINES_DATA = <MEM_DATA>;
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE2_MEM_LINES[$i] = $LINES_DATA[$i];
  }
  close(MEM_DATA);
  
  $FILE = $INPUT_MEM_FILE."_3.data";
  open (MEM_DATA, $FILE)
    or die "cannot open <", $FILE, ": $!";
  @LINES_DATA = <MEM_DATA>;
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE3_MEM_LINES[$i] = $LINES_DATA[$i];
  }
  close(MEM_DATA);
}
###########################
#
# FFT DATA PACKING INTO 96B/WORD
# PE0
#
$k = 0;
$i = 0;
while ($k < $PE0_W_MEM_SIZE){
	$LINE_MEM = "";
	for($j = 0; $j < $PE0_W_DATApWORD; $j++){	
		#print sprintf("%6.6b\t%6.6b\n",$DATA_W_IM[$i]&0x3F,$DATA_W_RE[$i]&0x3F);
		if($PE0_W_PREC==$PREC_06B){		
			$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_W_IM[$i]*(2**6))&0xFC0 | $PE0_DATA_W_RE[$i]&0x3F), $LINE_MEM; 	
			#print "$LINE_MEM\n";
		}
		if($PE0_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_W_IM[$i]&0xFF  ), sprintf("%2.2x",$PE0_DATA_W_RE[$i]&0xFF  ), $LINE_MEM;}
		if($PE0_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_W_IM[$i]&0xFFF ), sprintf("%3.3x",$PE0_DATA_W_RE[$i]&0xFFF ), $LINE_MEM;}
		if($PE0_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_W_IM[$i]&0xFFFF), sprintf("%4.4x",$PE0_DATA_W_RE[$i]&0xFFFF), $LINE_MEM;}
		$i++;
	}
	if($PE0_W_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE0_W_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000",	$LINE_MEM;}
	if($PE0_W_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE0_W_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
	
	if ($PE0_INST && $PE0_W == 0){		$PE0_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_W == 1){		$PE1_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_W == 2){		$PE2_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_W == 3){		$PE3_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE0_I_MEM_SIZE){
	for($p = 0; $p < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD/$PE0_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE0_I_DATApWORD; $q++){	
			#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_I0_IM[$i]&0x3F,$PE0_DATA_I0_RE[$i]&0x3F);
			if($PE0_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_I0_IM[$i]*(2**6))&0xFC0 | $PE0_DATA_I0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE0_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_I0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE0_DATA_I0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE0_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_I0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE0_DATA_I0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE0_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_I0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE0_DATA_I0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE0_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE0_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE0_INST && $PE0_I == 0){		$PE0_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 1){		$PE1_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 2){		$PE2_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 3){		$PE3_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
	for($p = 0; $p < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD/$PE0_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE0_I_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_I1_IM[$j]&0x3F,$DATA_I1_RE[$j]&0x3F);
			if($PE0_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_I1_IM[$j]*(2**6))&0xFC0 | $PE0_DATA_I1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE0_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_I1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE0_DATA_I1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE0_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_I1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE0_DATA_I1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE0_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_I1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE0_DATA_I1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE0_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE0_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE0_INST && $PE0_I == 0){		$PE0_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 1){		$PE1_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 2){		$PE2_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_I == 3){		$PE3_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE0_O_MEM_SIZE){
	for($p = 0; $p < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD/$PE0_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE0_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_O0_IM[$i]&0x3F,$DATA_O0_RE[$i]&0x3F);
			if($PE0_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_O0_IM[$i]*(2**6))&0xFC0 | $PE0_DATA_O0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE0_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_O0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE0_DATA_O0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE0_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_O0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE0_DATA_O0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE0_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_O0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE0_DATA_O0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE0_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE0_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE0_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
	for($p = 0; $p < $PE0_W_MEM_SIZE*$PE0_W_DATApWORD/$PE0_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE0_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_O1_IM[$j]&0x3F,$PE0_DATA_O1_RE[$j]&0x3F);
			if($PE0_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_O1_IM[$j]*(2**6))&0xFC0 | $PE0_DATA_O1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE0_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_O1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE0_DATA_O1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE0_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_O1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE0_DATA_O1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE0_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_O1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE0_DATA_O1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE0_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE0_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE0_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE0_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
}
#
# FFT DATA PACKING INTO 96B/WORD
# PE1
#
$k = 0;
$i = 0;
while ($k < $PE1_W_MEM_SIZE){
	$LINE_MEM = "";
	for($j = 0; $j < $PE1_W_DATApWORD; $j++){	
		#print sprintf("%6.6b\t%6.6b\n",$DATA_W_IM[$i]&0x3F,$DATA_W_RE[$i]&0x3F);
		if($PE1_W_PREC==$PREC_06B){		
			$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_W_IM[$i]*(2**6))&0xFC0 | $PE1_DATA_W_RE[$i]&0x3F), $LINE_MEM; 	
			#print "$LINE_MEM\n";
		}
		if($PE1_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_W_IM[$i]&0xFF  ), sprintf("%2.2x",$PE1_DATA_W_RE[$i]&0xFF  ), $LINE_MEM;}
		if($PE1_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_W_IM[$i]&0xFFF ), sprintf("%3.3x",$PE1_DATA_W_RE[$i]&0xFFF ), $LINE_MEM;}
		if($PE1_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_W_IM[$i]&0xFFFF), sprintf("%4.4x",$PE1_DATA_W_RE[$i]&0xFFFF), $LINE_MEM;}
		$i++;
	}
	if($PE1_W_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE1_W_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000",	$LINE_MEM;}
	if($PE1_W_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE1_W_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
	
	if ($PE1_INST && $PE1_W == 0){		$PE0_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_W == 1){		$PE1_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_W == 2){		$PE2_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_W == 3){		$PE3_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE1_I_MEM_SIZE){
	for($p = 0; $p < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD/$PE1_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE1_I_DATApWORD; $q++){	
			#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_I0_IM[$i]&0x3F,$PE1_DATA_I0_RE[$i]&0x3F);
			if($PE1_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_I0_IM[$i]*(2**6))&0xFC0 | $PE1_DATA_I0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE1_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_I0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE1_DATA_I0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE1_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_I0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE1_DATA_I0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE1_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_I0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE1_DATA_I0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE1_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE1_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE1_INST && $PE1_I == 0){		$PE0_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 1){		$PE1_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 2){		$PE2_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 3){		$PE3_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
	for($p = 0; $p < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD/$PE1_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE1_I_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_I1_IM[$j]&0x3F,$DATA_I1_RE[$j]&0x3F);
			if($PE1_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_I1_IM[$j]*(2**6))&0xFC0 | $PE1_DATA_I1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE1_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_I1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE1_DATA_I1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE1_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_I1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE1_DATA_I1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE1_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_I1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE1_DATA_I1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE1_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE1_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE1_INST && $PE1_I == 0){		$PE0_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 1){		$PE1_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 2){		$PE2_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_I == 3){		$PE3_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE1_O_MEM_SIZE){
	for($p = 0; $p < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD/$PE1_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE1_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_O0_IM[$i]&0x3F,$DATA_O0_RE[$i]&0x3F);
			if($PE1_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_O0_IM[$i]*(2**6))&0xFC0 | $PE1_DATA_O0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE1_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_O0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE1_DATA_O0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE1_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_O0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE1_DATA_O0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE1_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_O0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE1_DATA_O0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE1_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE1_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE1_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
	for($p = 0; $p < $PE1_W_MEM_SIZE*$PE1_W_DATApWORD/$PE1_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE1_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_O1_IM[$j]&0x3F,$PE1_DATA_O1_RE[$j]&0x3F);
			if($PE1_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_O1_IM[$j]*(2**6))&0xFC0 | $PE1_DATA_O1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE1_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_O1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE1_DATA_O1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE1_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_O1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE1_DATA_O1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE1_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_O1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE1_DATA_O1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE1_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE1_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE1_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE1_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
}
#
# FFT DATA PACKING INTO 96B/WORD
# PE2
#
$k = 0;
$i = 0;
while ($k < $PE2_W_MEM_SIZE){
	$LINE_MEM = "";
	for($j = 0; $j < $PE2_W_DATApWORD; $j++){	
		#print sprintf("%6.6b\t%6.6b\n",$DATA_W_IM[$i]&0x3F,$DATA_W_RE[$i]&0x3F);
		if($PE2_W_PREC==$PREC_06B){		
			$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_W_IM[$i]*(2**6))&0xFC0 | $PE2_DATA_W_RE[$i]&0x3F), $LINE_MEM; 	
			#print "$LINE_MEM\n";
		}
		if($PE2_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_W_IM[$i]&0xFF  ), sprintf("%2.2x",$PE2_DATA_W_RE[$i]&0xFF  ), $LINE_MEM;}
		if($PE2_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_W_IM[$i]&0xFFF ), sprintf("%3.3x",$PE2_DATA_W_RE[$i]&0xFFF ), $LINE_MEM;}
		if($PE2_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_W_IM[$i]&0xFFFF), sprintf("%4.4x",$PE2_DATA_W_RE[$i]&0xFFFF), $LINE_MEM;}
		$i++;
	}
	if($PE2_W_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE2_W_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000",	$LINE_MEM;}
	if($PE2_W_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE2_W_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
	
	if ($PE2_INST && $PE2_W == 0){		$PE0_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_W == 1){		$PE1_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_W == 2){		$PE2_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_W == 3){		$PE3_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE2_I_MEM_SIZE){
	for($p = 0; $p < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD/$PE2_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE2_I_DATApWORD; $q++){	
			#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_I0_IM[$i]&0x3F,$PE2_DATA_I0_RE[$i]&0x3F);
			if($PE2_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_I0_IM[$i]*(2**6))&0xFC0 | $PE2_DATA_I0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE2_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_I0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE2_DATA_I0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE2_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_I0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE2_DATA_I0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE2_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_I0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE2_DATA_I0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE2_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE2_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE2_INST && $PE2_I == 0){		$PE0_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 1){		$PE1_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 2){		$PE2_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 3){		$PE3_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
	for($p = 0; $p < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD/$PE2_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE2_I_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_I1_IM[$j]&0x3F,$DATA_I1_RE[$j]&0x3F);
			if($PE2_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_I1_IM[$j]*(2**6))&0xFC0 | $PE2_DATA_I1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE2_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_I1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE2_DATA_I1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE2_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_I1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE2_DATA_I1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE2_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_I1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE2_DATA_I1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE2_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE2_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE2_INST && $PE2_I == 0){		$PE0_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 1){		$PE1_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 2){		$PE2_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_I == 3){		$PE3_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE2_O_MEM_SIZE){
	for($p = 0; $p < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD/$PE2_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE2_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_O0_IM[$i]&0x3F,$DATA_O0_RE[$i]&0x3F);
			if($PE2_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_O0_IM[$i]*(2**6))&0xFC0 | $PE2_DATA_O0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE2_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_O0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE2_DATA_O0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE2_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_O0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE2_DATA_O0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE2_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_O0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE2_DATA_O0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE2_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE2_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE2_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
	for($p = 0; $p < $PE2_W_MEM_SIZE*$PE2_W_DATApWORD/$PE2_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE2_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_O1_IM[$j]&0x3F,$PE2_DATA_O1_RE[$j]&0x3F);
			if($PE2_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_O1_IM[$j]*(2**6))&0xFC0 | $PE2_DATA_O1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE2_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_O1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE2_DATA_O1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE2_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_O1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE2_DATA_O1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE2_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_O1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE2_DATA_O1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE2_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE2_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE2_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE2_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
}
#
# FFT DATA PACKING INTO 96B/WORD
# PE3
#
$k = 0;
$i = 0;
while ($k < $PE3_W_MEM_SIZE){
	$LINE_MEM = "";
	for($j = 0; $j < $PE3_W_DATApWORD; $j++){	
		#print sprintf("%6.6b\t%6.6b\n",$DATA_W_IM[$i]&0x3F,$DATA_W_RE[$i]&0x3F);
		if($PE3_W_PREC==$PREC_06B){		
			$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_W_IM[$i]*(2**6))&0xFC0 | $PE3_DATA_W_RE[$i]&0x3F), $LINE_MEM; 	
			#print "$LINE_MEM\n";
		}
		if($PE3_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_W_IM[$i]&0xFF  ), sprintf("%2.2x",$PE3_DATA_W_RE[$i]&0xFF  ), $LINE_MEM;}
		if($PE3_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_W_IM[$i]&0xFFF ), sprintf("%3.3x",$PE3_DATA_W_RE[$i]&0xFFF ), $LINE_MEM;}
		if($PE3_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_W_IM[$i]&0xFFFF), sprintf("%4.4x",$PE3_DATA_W_RE[$i]&0xFFFF), $LINE_MEM;}
		$i++;
	}
	if($PE3_W_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE3_W_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000",	$LINE_MEM;}
	if($PE3_W_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
	if($PE3_W_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
	
	if ($PE3_INST && $PE3_W == 0){		$PE0_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_W == 1){		$PE1_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_W == 2){		$PE2_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_W == 3){		$PE3_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE3_I_MEM_SIZE){
	for($p = 0; $p < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD/$PE3_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE3_I_DATApWORD; $q++){	
			#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_I0_IM[$i]&0x3F,$PE3_DATA_I0_RE[$i]&0x3F);
			if($PE3_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_I0_IM[$i]*(2**6))&0xFC0 | $PE3_DATA_I0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE3_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_I0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE3_DATA_I0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE3_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_I0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE3_DATA_I0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE3_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_I0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE3_DATA_I0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE3_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE3_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE3_INST && $PE3_I == 0){		$PE0_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 1){		$PE1_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 2){		$PE2_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 3){		$PE3_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
	for($p = 0; $p < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD/$PE3_I_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE3_I_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_I1_IM[$j]&0x3F,$DATA_I1_RE[$j]&0x3F);
			if($PE3_I_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_I1_IM[$j]*(2**6))&0xFC0 | $PE3_DATA_I1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE3_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_I1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE3_DATA_I1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE3_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_I1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE3_DATA_I1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE3_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_I1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE3_DATA_I1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE3_I_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_I_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE3_I_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_I_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		if ($PE3_INST && $PE3_I == 0){		$PE0_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 1){		$PE1_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 2){		$PE2_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_I == 3){		$PE3_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
	}
}
$k = 0;
$i = 0;
$j = 0;
while($k < $PE3_O_MEM_SIZE){
	for($p = 0; $p < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD/$PE3_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE3_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$DATA_O0_IM[$i]&0x3F,$DATA_O0_RE[$i]&0x3F);
			if($PE3_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_O0_IM[$i]*(2**6))&0xFC0 | $PE3_DATA_O0_RE[$i]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE3_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_O0_IM[$i]&0xFF  ), sprintf("%2.2x",$PE3_DATA_O0_RE[$i]&0xFF  ), $LINE_MEM;}
			if($PE3_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_O0_IM[$i]&0xFFF ), sprintf("%3.3x",$PE3_DATA_O0_RE[$i]&0xFFF ), $LINE_MEM;}
			if($PE3_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_O0_IM[$i]&0xFFFF), sprintf("%4.4x",$PE3_DATA_O0_RE[$i]&0xFFFF), $LINE_MEM;}
			$i++;
		}
		if($PE3_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE3_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE3_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
	for($p = 0; $p < $PE3_W_MEM_SIZE*$PE3_W_DATApWORD/$PE3_O_DATApWORD; $p++){
		$LINE_MEM = "";
		for($q = 0; $q < $PE3_O_DATApWORD; $q++){
			#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_O1_IM[$j]&0x3F,$PE3_DATA_O1_RE[$j]&0x3F);
			if($PE3_O_PREC==$PREC_06B){		
				$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_O1_IM[$j]*(2**6))&0xFC0 | $PE3_DATA_O1_RE[$j]&0x3F), $LINE_MEM; 	
				#print "$LINE_MEM\n";
			}
			if($PE3_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_O1_IM[$j]&0xFF  ), sprintf("%2.2x",$PE3_DATA_O1_RE[$j]&0xFF  ), $LINE_MEM;}
			if($PE3_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_O1_IM[$j]&0xFFF ), sprintf("%3.3x",$PE3_DATA_O1_RE[$j]&0xFFF ), $LINE_MEM;}
			if($PE3_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_O1_IM[$j]&0xFFFF), sprintf("%4.4x",$PE3_DATA_O1_RE[$j]&0xFFFF), $LINE_MEM;}
			$j++;
		}
		if($PE3_O_PREC==$PREC_06B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_O_PREC==$PREC_08B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		if($PE3_O_PREC==$PREC_12B){		$LINE_MEM = join "", "", 		$LINE_MEM;}
		if($PE3_O_PREC==$PREC_16B){		$LINE_MEM = join "", "00000000", 	$LINE_MEM;}
		#print "$LINE_MEM\n";
		$PE3_LINE_O[$k] = "$LINE_MEM\n";
		$k++;
	}
}

#
# WRITE PE_MEM w/ INPUT DATA
# FFT I/W DATA WRITING INTO MEMORY
#
for($i=0; $i<$MEM_OUT_BOUND; $i++){
	printf OUT0 $PE0_MEM_LINES[$i];
	printf OUT1 $PE1_MEM_LINES[$i];
	printf OUT2 $PE2_MEM_LINES[$i];
	printf OUT3 $PE3_MEM_LINES[$i];
}
close (OUT0);
close (OUT1);
close (OUT2);
close (OUT3);
close (OUT_DBG);

#
# WRITE PE_MEM w/ OUTPUT DATA
# FFT I/W + O DATA WRITING INTO MEMORY
#
$OF0 = join "", $inst, "_MEM_0.goldendata";
$OF1 = join "", $inst, "_MEM_1.goldendata";
$OF2 = join "", $inst, "_MEM_2.goldendata";
$OF3 = join "", $inst, "_MEM_3.goldendata";
open (OUT0, ">$OF0")
	or die "cannot open < ", $OUT0, ": $!";
open (OUT1, ">$OF1")
	or die "cannot open < ", $OUT1, ": $!";
open (OUT2, ">$OF2")
	or die "cannot open < ", $OUT2, ": $!";
open (OUT3, ">$OF3")
	or die "cannot open < ", $OUT3, ": $!";
#
# PE0
#
$k = 0;
for($i=$PE0_O_MEM_START_BASE; $i<($PE0_O_MEM_START_BASE + $PE0_O_MEM_SIZE); $i++){
	$LINE_MEM = $PE0_LINE_O[$k];
	$k++;
	if ($PE0_INST == 1){
	if ($PE0_O == 0){	$PE0_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE0_O == 1){	$PE1_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE0_O == 2){	$PE2_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE0_O == 3){	$PE3_MEM_LINES[$i] =  $LINE_MEM; }
	}
} 
# PE1
#
$k = 0;
for($i=$PE1_O_MEM_START_BASE; $i<($PE1_O_MEM_START_BASE + $PE1_O_MEM_SIZE); $i++){
	$LINE_MEM = $PE1_LINE_O[$k];
	$k++;
	if ($PE1_INST == 1){
	if ($PE1_O == 0){	$PE0_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE1_O == 1){	$PE1_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE1_O == 2){	$PE2_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE1_O == 3){	$PE3_MEM_LINES[$i] =  $LINE_MEM; }
	}
} 
# PE2
#
$k = 0;
for($i=$PE2_O_MEM_START_BASE; $i<($PE2_O_MEM_START_BASE + $PE2_O_MEM_SIZE); $i++){
	$LINE_MEM = $PE2_LINE_O[$k];
	$k++;
	if ($PE2_INST == 1){
	if ($PE2_O == 0){	$PE0_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE2_O == 1){	$PE1_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE2_O == 2){	$PE2_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE2_O == 3){	$PE3_MEM_LINES[$i] =  $LINE_MEM; }
	}
} 
# PE3
#
$k = 0;
for($i=$PE3_O_MEM_START_BASE; $i<($PE3_O_MEM_START_BASE + $PE3_O_MEM_SIZE); $i++){
	$LINE_MEM = $PE3_LINE_O[$k];
	$k++;
	if ($PE3_INST == 1){
	if ($PE3_O == 0){	$PE0_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE3_O == 1){	$PE1_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE3_O == 2){	$PE2_MEM_LINES[$i] =  $LINE_MEM; } 
	if ($PE3_O == 3){	$PE3_MEM_LINES[$i] =  $LINE_MEM; }
	}
}
# 
# WRITE PE_MEM w/ OUTPUT DATA
#
for($i=0; $i<$MEM_OUT_BOUND; $i++){
	printf OUT0 $PE0_MEM_LINES[$i];
	printf OUT1 $PE1_MEM_LINES[$i];
	printf OUT2 $PE2_MEM_LINES[$i];
	printf OUT3 $PE3_MEM_LINES[$i];
}
close (OUT0);
close (OUT1);
close (OUT2);
close (OUT3);

#########
## caogao

close(OUT_DNN_INST);

print ("\n");
print ("EXECUTING   ./V2_MEM.pl\n");

$OF0 = join "", $inst, "_MEM_0.data";
system ($^X,"V2_MEM.pl", $OF0);
$OF1 = join "", $inst, "_MEM_1.data";
system ($^X,"V2_MEM.pl", $OF1);
$OF2 = join "", $inst, "_MEM_2.data";
system ($^X,"V2_MEM.pl", $OF2);
$OF3 = join "", $inst, "_MEM_3.data";
system ($^X,"V2_MEM.pl", $OF3);

$OF0 = join "", $inst, "_MEM_0.goldendata";
system ($^X,"V2_MEM.pl", $OF0);
$OF1 = join "", $inst, "_MEM_1.goldendata";
system ($^X,"V2_MEM.pl", $OF1);
$OF2 = join "", $inst, "_MEM_2.goldendata";
system ($^X,"V2_MEM.pl", $OF2);
$OF3 = join "", $inst, "_MEM_3.goldendata";
system ($^X,"V2_MEM.pl", $OF3);

print ("COMPLETED\n");
print ("\n");
##########
