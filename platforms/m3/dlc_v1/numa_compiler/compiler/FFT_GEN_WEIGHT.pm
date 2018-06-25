#!/usr/bin/perl
## TODO: change PE1, PE2, PE3 memory gen same as PE0
package FFT_GEN_WEIGHT;
#use strict;
use warnings;
use POSIX ;
use Exporter;

our @ISA=qw(Exporter);
our @EXPORT=qw(FFT_GEN_WEIGHT);

######################################
## input: $inst, $INPUT_MEM_FILE, input, ADDR, (precision), $OUTPUT_MEM_FILE, output, config [input size, fft_num]
## output: none (output_mem_file & output as file)

sub FFT_GEN_WEIGHT {

my (@argv) = @_;

my $inst = "FFT";
if (defined $argv[0]) {
  $inst = $argv[0];
}
my $INPUT_MEM_FILE; 
my $OF0;
my $OF1;
my $OF2;
my $OF3;
if (defined $argv[1] and ($argv[1] ne "NULL")) {
  $INPUT_MEM_FILE = $argv[1];    ## memory footprint before this FFT (should at least include inputs and twiddle factors) 
}


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
my $weight_filename = "fft.weights";
my $input_filename = "fft.inputs";
my $import_input_file = 0;
if (defined $argv[2]) { $weight_filename = $argv[2]; }
if (defined $argv[3]) { $input_filename = $argv[3]; }
if (defined $argv[4] and $argv[4]) { $import_input_file = 1; }

open($weight_file, "<", $weight_filename) 
	or die "cannot open < ", $weight_filename, ": $!";
open($input_file, "<", $input_filename) 
	or die "cannot open < ", $input_filename, ": $!";

my $PE0_INST	 = 1;
my $PE1_INST	 = 0;
my $PE2_INST	 = 0;
my $PE3_INST	 = 0;
if (defined $argv[5]) {
  $PE0_INST	 = $argv[5] & 0b0001;
  $PE1_INST	 = $argv[5] & 0b0010;
  $PE2_INST	 = $argv[5] & 0b0100;
  $PE3_INST	 = $argv[5] & 0b1000;
}

# FFT_NUM MUST BE FACTOR OF I_SIZE
my $COMMON_FFT_NUM	= 4;
if (defined $argv[6]) { $COMMON_FFT_NUM = $argv[6];}

# IMPORTANT NOTE
# $I_PREC SHOULD BE >= $W_PREC UNLESS I_PREC and W_PREC are (08B, 12B)
# WORKING SETS ARE FOLLING
# ($I_PREC, $W_PREC) = (16B, 16B) (16B, 12B) (16B, 08B) (16B, 06B)
# ($I_PREC, $W_PREC) = (12B, 12B) (12B, 08B) (12B, 06B)
# ($I_PREC, $W_PREC) = (08B, 12B) (08B, 08B) (08B, 06B)
# ($I_PREC, $W_PREC) = (06B, 06B)
# $O_PREC HAS SAME RULE
my $COMMON_I_PREC	= $PREC_16B;
if (defined $argv[7]) { $COMMON_I_PREC = $argv[7];}
my $COMMON_O_PREC	= $PREC_16B;	
if (defined $argv[8]) { $COMMON_O_PREC = $argv[8];}
my $COMMON_W_PREC	= $PREC_16B;
if (defined $argv[9]) { $COMMON_W_PREC = $argv[9];}

my $PE0_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE0_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE0_W_MEM_START_BASE = $MEM_H3B1_START;
if (defined $argv[10] and defined $argv[11] and defined $argv[12]) {
  $PE0_I_MEM_START_BASE = $argv[10]; 
  $PE0_O_MEM_START_BASE = $argv[11]; 
  $PE0_W_MEM_START_BASE = $argv[12]; 
}
#
my $PE1_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE1_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE1_W_MEM_START_BASE = $MEM_H3B1_START;
if (defined $argv[13] and defined $argv[14] and defined $argv[15]) {
  $PE0_I_MEM_START_BASE = $argv[13]; 
  $PE0_O_MEM_START_BASE = $argv[14]; 
  $PE0_W_MEM_START_BASE = $argv[15]; 
}
#
my $PE2_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE2_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE2_W_MEM_START_BASE = $MEM_H3B1_START;
if (defined $argv[16] and defined $argv[17] and defined $argv[18]) {
  $PE0_I_MEM_START_BASE = $argv[16]; 
  $PE0_O_MEM_START_BASE = $argv[17]; 
  $PE0_W_MEM_START_BASE = $argv[18]; 
}
#
my $PE3_I_MEM_START_BASE = $MEM_H2B2_START;
my $PE3_O_MEM_START_BASE = $MEM_H2B4_START;
my $PE3_W_MEM_START_BASE = $MEM_H3B1_START;
if (defined $argv[19] and defined $argv[20] and defined $argv[21]) {
  $PE0_I_MEM_START_BASE = $argv[19]; 
  $PE0_O_MEM_START_BASE = $argv[20]; 
  $PE0_W_MEM_START_BASE = $argv[21]; 
}

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
my $PE0_SHIFT		= int($PE0_I_EXP + $PE0_W_EXP - $PE0_O_EXP) - 2;
my $PE0_SHIFT_OS 	= 0;
my $PE0_SHIFT_NL	= 0;
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
my $PE1_SHIFT		= int($PE1_I_EXP + $PE1_W_EXP - $PE1_O_EXP) - 2;
my $PE1_SHIFT_OS 	= 0;
my $PE1_SHIFT_NL	= 0;
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
my $PE2_SHIFT		= int($PE2_I_EXP + $PE2_W_EXP - $PE2_O_EXP) - 2;
my $PE2_SHIFT_OS 	= 0;
my $PE2_SHIFT_NL	= 0;
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
my $PE3_SHIFT		= int($PE3_I_EXP + $PE3_W_EXP - $PE3_O_EXP) - 2;
my $PE3_SHIFT_OS 	= 0;
my $PE3_SHIFT_NL	= 0;
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
#
# FFT DATA RANDOM GENERATION & CALCULATION
#
my $PE0_FFT_PT = $PE0_W_MEM_SIZE*$PE0_W_DATApWORD;
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
##
	$PE0_DATA_W_RE[$i] = $weight_line[2 * $i]; 
	$PE0_DATA_W_IM[$i] = $weight_line[2 * $i + 1];
##
}
##
@input_line = split / /,$LINE;		## TODO: check number of variables
if ($import_input_file) {
  $LINE = <$input_file>;			# size 
  @input_line = split / /,$LINE;		## TODO: check number of variables
}
##
for($i = 0; $i < $PE0_I_MEM_SIZE*$PE0_I_DATApWORD/2; $i++){
	$PE0_DATA_I0_RE[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I0_IM[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I1_RE[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
	$PE0_DATA_I1_IM[$i] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
##
  if ($import_input_file) {
  	$PE0_DATA_I0_RE[$i] = $input_line[4 * $i + 0];
  	$PE0_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
  	$PE0_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
  	$PE0_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
  }
##
}
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
if ($import_input_file) {
	$PE1_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE1_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE1_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE1_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
}
##
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
if ($import_input_file) {
	$PE2_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE2_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE2_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE2_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
}
##
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
if ($import_input_file) {
	$PE3_DATA_I0_RE[$i] = $input_line[4 * $i];
	$PE3_DATA_I0_IM[$i] = $input_line[4 * $i + 1];
	$PE3_DATA_I1_RE[$i] = $input_line[4 * $i + 2];
	$PE3_DATA_I1_IM[$i] = $input_line[4 * $i + 3];
}
##
}
#########################
# caogao
#
# MEM_* INITIALIZATION
#
my $FILE;
if (not (defined $argv[1] and $argv[1] ne "NULL")) {
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
 	$PE0_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
 	$PE1_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
 	$PE2_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
 	$PE3_MEM_LINES[$i] = "xxxxxxxxxxxxxxxxxxxxxxxx\n";
  	# $PE0_MEM_LINES[$i] = "000000000000000000000000\n";
  	# $PE1_MEM_LINES[$i] = "000000000000000000000000\n";
  	# $PE2_MEM_LINES[$i] = "000000000000000000000000\n";
  	# $PE3_MEM_LINES[$i] = "000000000000000000000000\n";
  }
} else {
  $FILE = $INPUT_MEM_FILE."_0.data";
  open (MEM_DATA, $FILE)
    or die "cannot open <", $FILE, ": $!";
  @LINES_DATA = <MEM_DATA>;
  for($i=0; $i<$MEM_OUT_BOUND; $i++){
  	$PE0_MEM_LINES[$i] = $LINES_DATA[$i];
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
## debug 521
#print "test\n";
#print $PE0_I_MEM_SIZE, "\n";
#print $PE0_W_MEM_SIZE, "\n";
#print $PE0_W_DATApWORD, "\n";
#print $PE0_I_DATApWORD, "\n";
## debug 521
if ($import_input_file) {
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
} ## end if (import_input_file)
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
if ($import_input_file) {
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
} # end if ($import_input_file)
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
if ($import_input_file) {
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
} # end if ($import_input_file)
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
if ($import_input_file) {
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
} # end if ($import_input_file)
#
# WRITE PE_MEM w/ INPUT DATA
# FFT I/W DATA WRITING INTO MEMORY
#
$OF0 = join "", $inst, "_MEM_0.data";
open (OUT0, ">$OF0")
	or die "cannot open < ", $OF0, ": $!";
$OF1 = join "", $inst, "_MEM_1.data";
open (OUT1, ">$OF1")
	or die "cannot open < ", $OF1, ": $!";
$OF2 = join "", $inst, "_MEM_2.data";
open (OUT2, ">$OF2")
	or die "cannot open < ", $OF2, ": $!";
$OF3 = join "", $inst, "_MEM_3.data";
open (OUT3, ">$OF3")
	or die "cannot open < ", $OF3, ": $!";

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

system ($^X,"V2_MEM.pl", $OF0);
system ($^X,"V2_MEM.pl", $OF1);
system ($^X,"V2_MEM.pl", $OF2);
system ($^X,"V2_MEM.pl", $OF3);

system ($^X,"gen_m0_data_file.pl", $OF0);
system ($^X,"gen_m0_data_file.pl", $OF1);
system ($^X,"gen_m0_data_file.pl", $OF2);
system ($^X,"gen_m0_data_file.pl", $OF3);
##########
} ## end FFT_GEN_WEIGHT
