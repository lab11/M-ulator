#!/usr/bin/perl
package MAC_sub_face_M1;
#use strict;
use warnings;
use POSIX ;
use Exporter;

our @ISA=qw(Exporter);
our @EXPORT=qw(MAC_sub_face_M1);

######################################
## input: $inst, $INPUT_MEM_FILE, input, ADDR, (precision), $OUTPUT_MEM_FILE, output, config [input size, fft_num]
## output: none (output_mem_file & output as file)

sub MAC_sub_face_M1 {

my (@argv) = @_;

foreach my $n (@argv) {
  print $n, "\t"; 
}
print "\n"; 

my $inst = "MAC";
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
} else {
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
}

my $INST = join "", $inst, ".inst";
open (OUT_INST, ">$INST")
	or die "cannot open < ", $INST, ": $!";

my $out_dnn_inst_filename = "dnn.inst.c";
open (OUT_DNN_INST, ">>$out_dnn_inst_filename")
	or die "cannot open < ", $out_dnn_inst_filename, ": $!";
my $out_dnn_nli_filename = "dnn.nli.c";
open (OUT_DNN_NLI, ">>$out_dnn_nli_filename")
	or die "cannot open < ", $out_dnn_nli_filename, ": $!";

my $OF_LOG = join "", $inst, "_GEN.log";
open (OUT_LOG, ">$OF_LOG")
	or die "cannot open < ", $OF_LOG, ": $!";
#####################
my $i;
my $j;
my $LINE;
my $IDX_DBG 	= 0;
my @LINES_DBG 	= ();

#
my $OP_FFT		= 0;
my $OP_MAC		= 1;
my $OP_NLI		= 2;
my $OP_MOV		= 3;

my $PREC_06B 		= 0;
my $PREC_08B 		= 1;
my $PREC_12B 		= 2;
my $PREC_16B 		= 3;
my $PREC_24B 		= 4;
my $PREC_32B 		= 5;

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

my $NLI_SEGMENT		= 10;

my $PE0_I 		= 0;
my $PE0_O 		= 0;
my $PE0_W 		= 0;
my $PE0_OS		= 0;

my $PE1_I 		= 1;
my $PE1_O 		= 1;
my $PE1_W 		= 1;
my $PE1_OS		= 1;

my $PE2_I 		= 2;
my $PE2_O 		= 2;
my $PE2_W 		= 2;
my $PE2_OS		= 2;

my $PE3_I 		= 3;
my $PE3_O 		= 3;
my $PE3_W 		= 3;
my $PE3_OS		= 3;

#####################
# SPECIFY PE UTILIZATION
my $weight_filename = "dnn.weights";
my $input_filename = "dnn.inputs";
my $offset_filename = "dnn.offsets";
my $nli_filename = "dnn.nli";
my $output_filename = "dnn.outputs";
if (defined $argv[2]) { $weight_filename = $argv[2]; }
if (defined $argv[3]) { $input_filename = $argv[3]; }
if (defined $argv[4]) { $output_filename = $argv[4]; }
if (defined $argv[5]) { $offset_filename = $argv[5]; }
if (defined $argv[6]) { $nli_filename = $argv[6]; }

open($weight_file, "<", $weight_filename) 
	or die "cannot open < ", $weight_filename, ": $!";
open($input_file, "<", $input_filename) 
	or die "cannot open < ", $input_filename, ": $!";
open($offset_file, "<", $offset_filename) 
	or die "cannot open < ", $offset_filename, ": $!";
open($nli_file, "<", $nli_filename) 
	or die "cannot open < ", $nli_filename, ": $!";
open($output_file, ">", $output_filename) 
	or die "cannot open < ", $output_filename, ": $!";

my $PE0_INST = 1;
my $PE1_INST = 1;
my $PE2_INST = 1;
my $PE3_INST = 1;
if (defined $argv[7]) {
  $PE0_INST	= (($argv[7] & 0b0001) != 0);
  $PE1_INST	= (($argv[7] & 0b0010) != 0);
  $PE2_INST	= (($argv[7] & 0b0100) != 0);
  $PE3_INST	= (($argv[7] & 0b1000) != 0);
}
#
my $PE0_MAC_OFFSET = 1;
my $PE1_MAC_OFFSET = 1;
my $PE2_MAC_OFFSET = 1;
my $PE3_MAC_OFFSET = 1;
if (defined $argv[8]) {
  $PE0_MAC_OFFSET	= (($argv[8] & 0b0001) != 0);
  $PE1_MAC_OFFSET	= (($argv[8] & 0b0010) != 0);
  $PE2_MAC_OFFSET	= (($argv[8] & 0b0100) != 0);
  $PE3_MAC_OFFSET	= (($argv[8] & 0b1000) != 0);
}
#
my $PE0_MAC_NLI	= 0;
my $PE1_MAC_NLI	= 0;
my $PE2_MAC_NLI	= 0;
my $PE3_MAC_NLI	= 0;
if (defined $argv[9]) {
  $PE0_MAC_NLI = (($argv[9] & 0b0001) != 0);
  $PE1_MAC_NLI = (($argv[9] & 0b0010) != 0);
  $PE2_MAC_NLI = (($argv[9] & 0b0100) != 0);
  $PE3_MAC_NLI = (($argv[9] & 0b1000) != 0);
}
#
my $PE0_O_SIGN = 0;
my $PE1_O_SIGN = 0;
my $PE2_O_SIGN = 0;
my $PE3_O_SIGN = 0;
if (defined $argv[10]) {
  $PE0_O_SIGN = (($argv[10] & 0b0001) != 0);
  $PE1_O_SIGN = (($argv[10] & 0b0010) != 0);
  $PE2_O_SIGN = (($argv[10] & 0b0100) != 0);
  $PE3_O_SIGN = (($argv[10] & 0b1000) != 0);
}

my $PE0_TO_MEM_START	    = $MEM_H1B3_START; 
my $PE0_I_MEM_START_BASE  = $MEM_H1B1_START; 
my $PE0_O_MEM_START_BASE  = $MEM_H2B4_START; 
my $PE0_W_MEM_START_BASE  = $MEM_H4B1_START; 
my $PE0_OS_MEM_START_BASE = $MEM_H2B3_START; 
if (defined $argv[11] and defined $argv[12] and defined $argv[13] and defined $argv[14] and defined $argv[15]) {
  print "defined\n";
  $PE0_TO_MEM_START = $argv[11]; 
  $PE0_I_MEM_START_BASE = $argv[12]; 
  $PE0_O_MEM_START_BASE = $argv[13]; 
  $PE0_W_MEM_START_BASE = $argv[14]; 
  $PE0_OS_MEM_START_BASE = $argv[15]; 
}
#
my $PE1_TO_MEM_START	    = $MEM_H1B3_START; 
my $PE1_I_MEM_START_BASE  = $MEM_H1B1_START; 
my $PE1_O_MEM_START_BASE  = $MEM_H2B4_START; 
my $PE1_W_MEM_START_BASE  = $MEM_H4B1_START; 
my $PE1_OS_MEM_START_BASE = $MEM_H2B3_START; 
if (defined $argv[16] and defined $argv[17] and defined $argv[18] and defined $argv[19] and defined $argv[20]) {
  $PE1_TO_MEM_START = $argv[16]; 
  $PE1_I_MEM_START_BASE = $argv[17]; 
  $PE1_O_MEM_START_BASE = $argv[18]; 
  $PE1_W_MEM_START_BASE = $argv[19]; 
  $PE1_OS_MEM_START_BASE = $argv[20]; 
}
#
my $PE2_TO_MEM_START	    = $MEM_H1B3_START;  
my $PE2_I_MEM_START_BASE  = $MEM_H1B1_START;  
my $PE2_O_MEM_START_BASE  = $MEM_H2B4_START;  
my $PE2_W_MEM_START_BASE  = $MEM_H4B1_START;  
my $PE2_OS_MEM_START_BASE = $MEM_H2B3_START;  
if (defined $argv[21] and defined $argv[22] and defined $argv[23] and defined $argv[24] and defined $argv[25]) {
  $PE2_TO_MEM_START = $argv[21]; 
  $PE2_I_MEM_START_BASE = $argv[22]; 
  $PE2_O_MEM_START_BASE = $argv[23]; 
  $PE2_W_MEM_START_BASE = $argv[24]; 
  $PE2_OS_MEM_START_BASE = $argv[25]; 
}
#
my $PE3_TO_MEM_START	    = $MEM_H1B3_START;  
my $PE3_I_MEM_START_BASE  = $MEM_H1B1_START;  
my $PE3_O_MEM_START_BASE  = $MEM_H2B4_START;  
my $PE3_W_MEM_START_BASE  = $MEM_H4B1_START;  
my $PE3_OS_MEM_START_BASE = $MEM_H2B3_START;  
if (defined $argv[26] and defined $argv[27] and defined $argv[28] and defined $argv[29] and defined $argv[30]) {
  $PE3_TO_MEM_START = $argv[26]; 
  $PE3_I_MEM_START_BASE = $argv[27]; 
  $PE3_O_MEM_START_BASE = $argv[28]; 
  $PE3_W_MEM_START_BASE = $argv[29]; 
  $PE3_OS_MEM_START_BASE = $argv[30]; 
}

my $PE_SHIFT = $argv[31];
my $PE_SHIFT_OS = $argv[32];
my $PE_SHIFT_NL = $argv[33];

##################
$LINE = <$weight_file>;		# size 
my @line = split / /,$LINE;
my $MAC_COL_NUM_ORG	= $line[0];
my $MAC_ROW_NUM_ORG	= $line[1];
my $temp_value = $line[2];
my $temp_prec;
if ($temp_value == 6) {$temp_prec = $PREC_06B;}
elsif ($temp_value == 8) {$temp_prec = $PREC_08B;}
elsif ($temp_value == 12) {$temp_prec = $PREC_12B;}
elsif ($temp_value == 16) {$temp_prec = $PREC_16B;}
else  {die "W_PREC SPECIFICATION IS OUT OF RANGE\n\n";}
my $COMMON_W_PREC = $temp_prec;

$LINE = <$input_file>;		# size 
@line = split / /,$LINE;
$MAC_COL_NUM_ORG == $line[0]
	or die "input file does not match weight file";
$temp_value = $line[1];
if ($temp_value == 6) {$temp_prec = $PREC_06B;}
elsif ($temp_value == 8) {$temp_prec = $PREC_08B;}
elsif ($temp_value == 12) {$temp_prec = $PREC_12B;}
elsif ($temp_value == 16) {$temp_prec = $PREC_16B;}
else  {die "I_PREC SPECIFICATION IS OUT OF RANGE\n\n";}
my $COMMON_I_PREC = $temp_prec;
#
$temp_value = $line[2];
if ($temp_value == 6) {$temp_prec = $PREC_06B;}
elsif ($temp_value == 8) {$temp_prec = $PREC_08B;}
elsif ($temp_value == 12) {$temp_prec = $PREC_12B;}
elsif ($temp_value == 16) {$temp_prec = $PREC_16B;}
else  {die "O_PREC SPECIFICATION IS OUT OF RANGE\n\n";}
my $COMMON_O_PREC = $temp_prec;
#
$temp_value = $line[3];
if ($temp_value == 16) {$temp_prec = $PREC_16B;}
elsif ($temp_value == 24) {$temp_prec = $PREC_24B;}
elsif ($temp_value == 32) {$temp_prec = $PREC_32B;}
else  {die "TO_PREC SPECIFICATION IS OUT OF RANGE\n\n";}
my $COMMON_TO_PREC = $temp_prec;

$LINE = <$offset_file>;		# size 
@line = split / /,$LINE;
$MAC_ROW_NUM_ORG == $line[0]
	or die "offset file does not match weight file";

print	sprintf("MAC_ROW_NUM_ORG = %d\n", $MAC_ROW_NUM_ORG);
my $num_PE_on = $PE0_INST + $PE1_INST + $PE2_INST + $PE3_INST;
print	sprintf("num_PE_on = %d\n", $num_PE_on);
$MAC_ROW_NUM_ORG = ceil($MAC_ROW_NUM_ORG / $num_PE_on);
#print	sprintf("MAC_ROW_NUM_ORG(output)= %d\n", $MAC_ROW_NUM_ORG);
#print	sprintf("MAC_COL_NUM_ORG(input)= %d\n", $MAC_COL_NUM_ORG);
#

# $W_WORD_UNIT = LCM($I_DATApWORD, $W_DATApWORD)/$W_DATApWORD
# $W_ROW_UNTI  = LCM($I_DATApWORD, $W_DATApWORD)/$I_DATApWORD
# *LCM = Least Common Multiple*
#
# UPDATE $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
# ADD CODE TO CALCUATE LCM LATER!
# $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
#my $ROW_NUM_MIN_FACTOR = 24;
my $ROW_NUM_MIN_FACTOR = 6;
#my $COL_NUM_MIN_FACTOR = 8;   ## same as PE_I_DATApWORD
my $COL_NUM_MIN_FACTOR = 12;   ## same as PE_I_DATApWORD
##########
my $PE0_MAC_COL_NUM_ORG	= $MAC_COL_NUM_ORG;
my $PE0_MAC_ROW_NUM_ORG	= $MAC_ROW_NUM_ORG;
my $PE1_MAC_COL_NUM_ORG	= $MAC_COL_NUM_ORG;
my $PE1_MAC_ROW_NUM_ORG	= $MAC_ROW_NUM_ORG;
my $PE2_MAC_COL_NUM_ORG	= $MAC_COL_NUM_ORG;
my $PE2_MAC_ROW_NUM_ORG	= $MAC_ROW_NUM_ORG;
my $PE3_MAC_COL_NUM_ORG	= $MAC_COL_NUM_ORG;
my $PE3_MAC_ROW_NUM_ORG	= $MAC_ROW_NUM_ORG;

my $PE0_I_DATApWORD;
my $PE0_O_DATApWORD;
my $PE0_W_DATApWORD;
#
# SET DATA PRECISION
#
my $PE0_I_PREC 		= $COMMON_I_PREC;
if ($PE0_I_PREC != $PREC_06B && $PE0_I_PREC != $PREC_08B && $PE0_I_PREC != $PREC_12B && $PE0_I_PREC != $PREC_16B){
	die "I_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE0_I_EXP;
if ($PE0_I_PREC == $PREC_06B) 	{$PE0_I_DATApWORD = 16;	$PE0_I_EXP = 6;}
if ($PE0_I_PREC == $PREC_08B) 	{$PE0_I_DATApWORD = 12;	$PE0_I_EXP = 8;}
if ($PE0_I_PREC == $PREC_12B) 	{$PE0_I_DATApWORD = 8;	$PE0_I_EXP = 12;}
if ($PE0_I_PREC == $PREC_16B) 	{$PE0_I_DATApWORD = 6;	$PE0_I_EXP = 16;}
#
my $PE0_W_PREC 		= $COMMON_W_PREC;
if ($PE0_W_PREC != $PREC_06B && $PE0_W_PREC != $PREC_08B && $PE0_W_PREC != $PREC_12B && $PE0_W_PREC != $PREC_16B){
	die "W_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE0_W_EXP;
if ($PE0_W_PREC == $PREC_06B) 	{$PE0_W_DATApWORD = 16;	$PE0_W_EXP = 6;}
if ($PE0_W_PREC == $PREC_08B) 	{$PE0_W_DATApWORD = 12;	$PE0_W_EXP = 8;}
if ($PE0_W_PREC == $PREC_12B) 	{$PE0_W_DATApWORD = 8;	$PE0_W_EXP = 12;}
if ($PE0_W_PREC == $PREC_16B) 	{$PE0_W_DATApWORD = 6;	$PE0_W_EXP = 16;}
#
my $PE0_O_PREC 		= $COMMON_O_PREC;
if ($PE0_O_PREC != $PREC_06B && $PE0_O_PREC != $PREC_08B && $PE0_O_PREC != $PREC_12B && $PE0_O_PREC != $PREC_16B){
	die "O_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE0_O_EXP;
if ($PE0_O_PREC == $PREC_06B) 	{$PE0_O_DATApWORD = 16;	$PE0_O_EXP = 6;}
if ($PE0_O_PREC == $PREC_08B) 	{$PE0_O_DATApWORD = 12;	$PE0_O_EXP = 8;}
if ($PE0_O_PREC == $PREC_12B) 	{$PE0_O_DATApWORD = 8;	$PE0_O_EXP = 12;}
if ($PE0_O_PREC == $PREC_16B) 	{$PE0_O_DATApWORD = 6;	$PE0_O_EXP = 16;}
#
my $PE0_TO_PREC		= $COMMON_TO_PREC;
my $PE0_TO_EXP;
if ($PE0_TO_PREC == $PREC_16B) 	{$PE0_TO_DATApWORD = 6;	$PE0_TO_EXP = 16;}
if ($PE0_TO_PREC == $PREC_24B) 	{$PE0_TO_DATApWORD = 4;	$PE0_TO_EXP = 24;}
if ($PE0_TO_PREC == $PREC_32B) 	{$PE0_TO_DATApWORD = 3;	$PE0_TO_EXP = 32;}
#
#my $OS_PREC		= $PE0_O_PREC
#
# GENERATE PARAMETERS FOR ZERO-PADDING
#
my $PE0_W_WORD_UNIT;
my $PE0_W_ROW_UNIT;

#
# IMPORTANT NOTES
#
# PREC_06B: DATApWORD = 16
# PREC_08B: DATApWORD = 12
# PREC_12B: DATApWORD = 08
# PREC_16B: DATApWORD = 06
if ($PE0_I_PREC == $PREC_06B && $PE0_W_PREC == $PREC_06B){	$PE0_W_WORD_UNIT = 1;	$PE0_W_ROW_UNIT = 1;}
if ($PE0_I_PREC == $PREC_06B && $PE0_W_PREC == $PREC_08B){	$PE0_W_WORD_UNIT = 4;	$PE0_W_ROW_UNIT = 3;}
if ($PE0_I_PREC == $PREC_06B && $PE0_W_PREC == $PREC_12B){	$PE0_W_WORD_UNTI = 2;	$PE0_W_ROW_UNIT = 1;}
if ($PE0_I_PREC == $PREC_06B && $PE0_W_PREC == $PREC_16B){	$PE0_W_WORD_UNIT = 8; 	$PE0_W_ROW_UNIT = 3;}
#
if ($PE0_I_PREC == $PREC_08B && $PE0_W_PREC == $PREC_06B){	$PE0_W_WORD_UNIT = 3;	$PE0_W_ROW_UNIT = 4;}
if ($PE0_I_PREC == $PREC_08B && $PE0_W_PREC == $PREC_08B){	$PE0_W_WORD_UNIT = 1;	$PE0_W_ROW_UNIT = 1;}
if ($PE0_I_PREC == $PREC_08B && $PE0_W_PREC == $PREC_12B){	$PE0_W_WORD_UNTI = 3;	$PE0_W_ROW_UNIT = 2;}
if ($PE0_I_PREC == $PREC_08B && $PE0_W_PREC == $PREC_16B){	$PE0_W_WORD_UNIT = 2; 	$PE0_W_ROW_UNIT = 1;}
#
if ($PE0_I_PREC == $PREC_12B && $PE0_W_PREC == $PREC_06B){	$PE0_W_WORD_UNIT = 3;	$PE0_W_ROW_UNIT = 6;}
if ($PE0_I_PREC == $PREC_12B && $PE0_W_PREC == $PREC_08B){	$PE0_W_WORD_UNIT = 2;	$PE0_W_ROW_UNIT = 3;}
if ($PE0_I_PREC == $PREC_12B && $PE0_W_PREC == $PREC_12B){	$PE0_W_WORD_UNTI = 1;	$PE0_W_ROW_UNIT = 1;}
if ($PE0_I_PREC == $PREC_12B && $PE0_W_PREC == $PREC_16B){	$PE0_W_WORD_UNIT = 4; 	$PE0_W_ROW_UNIT = 3;}
#
if ($PE0_I_PREC == $PREC_16B && $PE0_W_PREC == $PREC_06B){	$PE0_W_WORD_UNIT = 3;	$PE0_W_ROW_UNIT = 8;}
if ($PE0_I_PREC == $PREC_16B && $PE0_W_PREC == $PREC_08B){	$PE0_W_WORD_UNIT = 1;	$PE0_W_ROW_UNIT = 2;}
if ($PE0_I_PREC == $PREC_16B && $PE0_W_PREC == $PREC_12B){	$PE0_W_WORD_UNTI = 3;	$PE0_W_ROW_UNIT = 4;}
if ($PE0_I_PREC == $PREC_16B && $PE0_W_PREC == $PREC_16B){	$PE0_W_WORD_UNIT = 1; 	$PE0_W_ROW_UNIT = 1;}

# PARAMETERS FOR ZERO-PADDING
#
## caogao
my $PE0_MAC_ROW_NUM_ZP = ceil($PE0_MAC_ROW_NUM_ORG/$ROW_NUM_MIN_FACTOR)*$ROW_NUM_MIN_FACTOR;
#my $PE0_MAC_COL_NUM_ZP = ceil($PE0_MAC_COL_NUM_ORG/$PE0_I_DATApWORD)*$PE0_I_DATApWORD;
my $PE0_MAC_COL_NUM_ZP = ceil($PE0_MAC_COL_NUM_ORG/$COL_NUM_MIN_FACTOR)*$COL_NUM_MIN_FACTOR;
# NUM ROWS TO BE ZERO-PADDED = $PE0_MAC_ROW_NUM_ZP - $MAC_ROW_NUM
# NUM COLS TO BE ZERO-PADDED = $MAC_ROW_COL_ZP - $MAC_COL_NUM

# SET MEM SPECIFICATION
#
#my $PE0_MAC_W_COL_NUM 	= int($PE0_I_MEM_SIZE*$PE0_I_DATApWORD/$PE0_W_DATApWORD);
my $PE0_MAC_W_COL_NUM 	= int($PE0_MAC_COL_NUM_ZP/$PE0_W_DATApWORD);

# GENERATE DATA FOR TESTING MAC
#
my $PE0_OP		= $OP_MAC;
#my $PE0_TO_MEM_START	= $MEM_H1B2_START;
#my $PE0_TO_MEM_START 	= $TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE0_TO_MEM_SIZE	= int($PE0_MAC_ROW_NUM_ZP/$PE0_TO_DATApWORD);
#my $PE0_TO_PREC	= 0;
#my $PE0_I_MEM_START_BASE = $MEM_H2B1_START;
my $PE0_I_MEM_START  	= $PE0_I_MEM_START_BASE  + $PE0_I*(2**13);
my $PE0_I_MEM_SIZE 	= int($PE0_MAC_COL_NUM_ZP/$PE0_I_DATApWORD);
#my $PE0_I_MEM_SIZE 	= 128;
#my $PE0_I_PREC		= $PREC_12B;
#my $PE0_O_MEM_START_BASE = $MEM_H2B3_START;
my $PE0_O_MEM_START  	= $PE0_O_MEM_START_BASE  + $PE0_O*(2**13);
my $PE0_O_MEM_SIZE 	= int($PE0_MAC_ROW_NUM_ZP/$PE0_O_DATApWORD);
#my $PE0_O_PREC		= $PE0_I_PREC;
#my $PE0_W_MEM_START_BASE= $MEM_H4B1_START;
my $PE0_W_MEM_START  	= $PE0_W_MEM_START_BASE  + $PE0_W*(2**13);
my $PE0_W_MEM_SIZE	= int($PE0_MAC_COL_NUM_ZP*$PE0_MAC_ROW_NUM_ZP/$PE0_W_DATApWORD);
#my $PE0_OS_MEM_START_BASE= $PE0_W_MEM_START_BASE + $PE0_W_MEM_SIZE + 1;
#my $PE0_OS_MEM_START_BASE= $MEM_H1B1_START;
my $PE0_OS_MEM_START 	= $PE0_OS_MEM_START_BASE + $PE0_OS*(2**13);
#my $PE0_W_PREC		= 0;
#my $MAC_OFFSET		= 0;
#my $MAC_NLI		= 0;
my $PE0_MAC_ROW_NUM	= $PE0_MAC_ROW_NUM_ZP;
my $PE0_FFT_NUM	 	= 0;	
my $PE0_FFT_I_UNIT_SIZE = 0;
my $PE0_FFT_O_UNIT_SIZE	= 0;
#my $PE0_SHIFT		= int(($PE0_I_EXP + $PE0_W_EXP) + (log($PE0_MAC_COL_NUM_ZP)/log(2)) - $PE0_O_EXP);
#my $PE0_SHIFT		= int(($PE0_I_EXP + $PE0_W_EXP) + (log($PE0_MAC_COL_NUM_ZP)/log(2)) - $PE0_O_EXP + 1);
my $PE0_SHIFT		= $PE_SHIFT;
my $PE0_SHIFT_OS 	= $PE_SHIFT_OS;
my $PE0_SHIFT_NL	= $PE_SHIFT_NL;
#
print	sprintf("MAC_ROW_NUM_ZP(output)= %d\n", $PE0_MAC_ROW_NUM_ZP);
print	sprintf("MAC_ROW_NUM_ORG(output)= %d\n", $PE0_MAC_ROW_NUM_ORG);
print	sprintf("MAC_COL_NUM_ZP(input)= %d\n", $PE0_MAC_COL_NUM_ZP);
print	sprintf("MAC_COL_NUM_ORG(output)= %d\n", $PE0_MAC_COL_NUM_ORG);
print	sprintf("PE_SHIFT = %d\n", $PE0_SHIFT);
print	sprintf("PE_SHIFT_OS = %d\n", $PE0_SHIFT_OS);
print	sprintf("PE_SHIFT_NL = %d\n", $PE0_SHIFT_NL);
print 	"\n";
print	sprintf("PE0_TO_SIZE = %d\n", $PE0_TO_MEM_SIZE);
print	sprintf("PE0_I_SIZE = %d\n", $PE0_I_MEM_SIZE);
print	sprintf("PE0_O_SIZE = %d\n", $PE0_O_MEM_SIZE);
print	sprintf("PE0_W_SIZE = %d\n", $PE0_W_MEM_SIZE);
print	sprintf("PE0_OS_SIZE = %d\n", $PE0_O_MEM_SIZE);
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
print	sprintf("PE0_TO_MEM_START_BASE = %d\n", $PE0_TO_MEM_START);
print	sprintf("PE0_TO_MEM_END_BASE = %d\n", $PE0_TO_MEM_START + $PE0_TO_MEM_SIZE - 1);
print	sprintf("PE0_I_MEM_START_BASE  = %d\n", $PE0_I_MEM_START_BASE);
print	sprintf("PE0_I_MEM_END_BASE    = %d\n", $PE0_I_MEM_START_BASE + $PE0_I_MEM_SIZE - 1);
print	sprintf("PE0_O_MEM_START_BASE  = %d\n", $PE0_O_MEM_START_BASE);
print	sprintf("PE0_O_MEM_END_BASE    = %d\n", $PE0_O_MEM_START_BASE + $PE0_O_MEM_SIZE - 1);
print	sprintf("PE0_W_MEM_START_BASE  = %d\n", $PE0_W_MEM_START_BASE);
print	sprintf("PE0_W_MEM_END_BASE    = %d\n", $PE0_W_MEM_START_BASE + $PE0_W_MEM_SIZE - 1);
print	sprintf("PE0_OS_MEM_START_BASE = %d\n", $PE0_OS_MEM_START_BASE);
print	sprintf("PE0_OS_MEM_END_BASE   = %d\n", $PE0_OS_MEM_START_BASE + $PE0_O_MEM_SIZE - 1);
######################

#
my $PE0_INST_0 = "";
my $PE0_INST_1 = "";
my $PE0_INST_2 = "";
my $PE0_INST_3 = "";
my $PE0_INST_4 = "";
my $PE0_INST_5 = "";
#
my $PE0_MAC_EN = 1;
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
if ($PE0_MAC_EN){
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
# GENERATE DATA FOR TESTING MAC
# PE1
#
#
my $PE1_I_DATApWORD;
my $PE1_O_DATApWORD;
my $PE1_W_DATApWORD;
#
# SET DATA PRECISION
#
my $PE1_I_PREC 		= $COMMON_I_PREC;
if ($PE1_I_PREC != $PREC_06B && $PE1_I_PREC != $PREC_08B && $PE1_I_PREC != $PREC_12B && $PE1_I_PREC != $PREC_16B){
	die "I_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE1_I_EXP;
if ($PE1_I_PREC == $PREC_06B) 	{$PE1_I_DATApWORD = 16;	$PE1_I_EXP = 6;}
if ($PE1_I_PREC == $PREC_08B) 	{$PE1_I_DATApWORD = 12;	$PE1_I_EXP = 8;}
if ($PE1_I_PREC == $PREC_12B) 	{$PE1_I_DATApWORD = 8;	$PE1_I_EXP = 12;}
if ($PE1_I_PREC == $PREC_16B) 	{$PE1_I_DATApWORD = 6;	$PE1_I_EXP = 16;}
#
my $PE1_W_PREC 		= $COMMON_W_PREC;
if ($PE1_W_PREC != $PREC_06B && $PE1_W_PREC != $PREC_08B && $PE1_W_PREC != $PREC_12B && $PE1_W_PREC != $PREC_16B){
	die "W_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE1_W_EXP;
if ($PE1_W_PREC == $PREC_06B) 	{$PE1_W_DATApWORD = 16;	$PE1_W_EXP = 6;}
if ($PE1_W_PREC == $PREC_08B) 	{$PE1_W_DATApWORD = 12;	$PE1_W_EXP = 8;}
if ($PE1_W_PREC == $PREC_12B) 	{$PE1_W_DATApWORD = 8;	$PE1_W_EXP = 12;}
if ($PE1_W_PREC == $PREC_16B) 	{$PE1_W_DATApWORD = 6;	$PE1_W_EXP = 16;}
#
my $PE1_O_PREC 		= $COMMON_O_PREC;
if ($PE1_O_PREC != $PREC_06B && $PE1_O_PREC != $PREC_08B && $PE1_O_PREC != $PREC_12B && $PE1_O_PREC != $PREC_16B){
	die "O_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE1_O_EXP;
if ($PE1_O_PREC == $PREC_06B) 	{$PE1_O_DATApWORD = 16;	$PE1_O_EXP = 6;}
if ($PE1_O_PREC == $PREC_08B) 	{$PE1_O_DATApWORD = 12;	$PE1_O_EXP = 8;}
if ($PE1_O_PREC == $PREC_12B) 	{$PE1_O_DATApWORD = 8;	$PE1_O_EXP = 12;}
if ($PE1_O_PREC == $PREC_16B) 	{$PE1_O_DATApWORD = 6;	$PE1_O_EXP = 16;}
#
my $PE1_TO_PREC		= $COMMON_TO_PREC;
my $PE1_TO_EXP;
if ($PE1_TO_PREC == $PREC_16B) 	{$PE1_TO_DATApWORD = 6;	$PE1_TO_EXP = 16;}
if ($PE1_TO_PREC == $PREC_24B) 	{$PE1_TO_DATApWORD = 4;	$PE1_TO_EXP = 24;}
if ($PE1_TO_PREC == $PREC_32B) 	{$PE1_TO_DATApWORD = 3;	$PE1_TO_EXP = 32;}
#
#my $OS_PREC		= $PE0_O_PREC
#
# GENERATE PARAMETERS FOR ZERO-PADDING
#
my $PE1_W_WORD_UNIT;
my $PE1_W_ROW_UNIT;

#
# IMPORTANT NOTES
#
# PREC_06B: DATApWORD = 16
# PREC_08B: DATApWORD = 12
# PREC_12B: DATApWORD = 08
# PREC_16B: DATApWORD = 06
#
# $W_WORD_UNIT = LCM($I_DATApWORD, $W_DATApWORD)/$W_DATApWORD
# $W_ROW_UNTI  = LCM($I_DATApWORD, $W_DATApWORD)/$I_DATApWORD
# *LCM = Least Common Multiple*
#
if ($PE1_I_PREC == $PREC_06B && $PE1_W_PREC == $PREC_06B){	$PE1_W_WORD_UNIT = 1;	$PE1_W_ROW_UNIT = 1;}
if ($PE1_I_PREC == $PREC_06B && $PE1_W_PREC == $PREC_08B){	$PE1_W_WORD_UNIT = 4;	$PE1_W_ROW_UNIT = 3;}
if ($PE1_I_PREC == $PREC_06B && $PE1_W_PREC == $PREC_12B){	$PE1_W_WORD_UNTI = 2;	$PE1_W_ROW_UNIT = 1;}
if ($PE1_I_PREC == $PREC_06B && $PE1_W_PREC == $PREC_16B){	$PE1_W_WORD_UNIT = 8; 	$PE1_W_ROW_UNIT = 3;}
#
if ($PE1_I_PREC == $PREC_08B && $PE1_W_PREC == $PREC_06B){	$PE1_W_WORD_UNIT = 3;	$PE1_W_ROW_UNIT = 4;}
if ($PE1_I_PREC == $PREC_08B && $PE1_W_PREC == $PREC_08B){	$PE1_W_WORD_UNIT = 1;	$PE1_W_ROW_UNIT = 1;}
if ($PE1_I_PREC == $PREC_08B && $PE1_W_PREC == $PREC_12B){	$PE1_W_WORD_UNTI = 3;	$PE1_W_ROW_UNIT = 2;}
if ($PE1_I_PREC == $PREC_08B && $PE1_W_PREC == $PREC_16B){	$PE1_W_WORD_UNIT = 2; 	$PE1_W_ROW_UNIT = 1;}
#
if ($PE1_I_PREC == $PREC_12B && $PE1_W_PREC == $PREC_06B){	$PE1_W_WORD_UNIT = 3;	$PE1_W_ROW_UNIT = 6;}
if ($PE1_I_PREC == $PREC_12B && $PE1_W_PREC == $PREC_08B){	$PE1_W_WORD_UNIT = 2;	$PE1_W_ROW_UNIT = 3;}
if ($PE1_I_PREC == $PREC_12B && $PE1_W_PREC == $PREC_12B){	$PE1_W_WORD_UNTI = 1;	$PE1_W_ROW_UNIT = 1;}
if ($PE1_I_PREC == $PREC_12B && $PE1_W_PREC == $PREC_16B){	$PE1_W_WORD_UNIT = 4; 	$PE1_W_ROW_UNIT = 3;}
#
if ($PE1_I_PREC == $PREC_16B && $PE1_W_PREC == $PREC_06B){	$PE1_W_WORD_UNIT = 3;	$PE1_W_ROW_UNIT = 8;}
if ($PE1_I_PREC == $PREC_16B && $PE1_W_PREC == $PREC_08B){	$PE1_W_WORD_UNIT = 1;	$PE1_W_ROW_UNIT = 2;}
if ($PE1_I_PREC == $PREC_16B && $PE1_W_PREC == $PREC_12B){	$PE1_W_WORD_UNTI = 3;	$PE1_W_ROW_UNIT = 4;}
if ($PE1_I_PREC == $PREC_16B && $PE1_W_PREC == $PREC_16B){	$PE1_W_WORD_UNIT = 1; 	$PE1_W_ROW_UNIT = 1;}

# UPDATE $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
# ADD CODE TO CALCUATE LCM LATER!
# $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#

# PARAMETERS FOR ZERO-PADDING
#
my $PE1_MAC_ROW_NUM_ZP = ceil($PE1_MAC_ROW_NUM_ORG/$ROW_NUM_MIN_FACTOR)*$ROW_NUM_MIN_FACTOR;
#my $PE1_MAC_COL_NUM_ZP = ceil($PE1_MAC_COL_NUM_ORG/$PE1_I_DATApWORD)*$PE1_I_DATApWORD;
my $PE1_MAC_COL_NUM_ZP = ceil($PE1_MAC_COL_NUM_ORG/$COL_NUM_MIN_FACTOR)*$COL_NUM_MIN_FACTOR;
# NUM ROWS TO BE ZERO-PADDED = $PE0_MAC_ROW_NUM_ZP - $MAC_ROW_NUM
# NUM COLS TO BE ZERO-PADDED = $MAC_ROW_COL_ZP - $MAC_COL_NUM

# SET MEM SPECIFICATION
#
#my $PE1_MAC_W_COL_NUM 	= int($PE1_I_MEM_SIZE*$PE1_I_DATApWORD/$PE1_W_DATApWORD);
my $PE1_MAC_W_COL_NUM 	= int($PE1_MAC_COL_NUM_ZP/$PE1_W_DATApWORD);

# GENERATE DATA FOR TESTING MAC
#
my $PE1_OP		= $OP_MAC;
#my $PE1_TO_MEM_START	= $MEM_H1B2_START;
#my $PE1_TO_MEM_START 	= $TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE1_TO_MEM_SIZE	= int($PE1_MAC_ROW_NUM_ZP/$PE1_TO_DATApWORD);
#my $PE1_TO_PREC	= 0;
#my $PE1_I_MEM_START_BASE = $MEM_H2B1_START;
my $PE1_I_MEM_START  	= $PE1_I_MEM_START_BASE  + $PE1_I*(2**13);
my $PE1_I_MEM_SIZE 	= int($PE1_MAC_COL_NUM_ZP/$PE1_I_DATApWORD);
#my $PE1_I_MEM_SIZE 	= 128;
#my $PE1_I_PREC		= $PREC_12B;
#my $PE1_O_MEM_START_BASE = $MEM_H2B3_START;
my $PE1_O_MEM_START  	= $PE1_O_MEM_START_BASE  + $PE1_O*(2**13);
my $PE1_O_MEM_SIZE 	= int($PE1_MAC_ROW_NUM_ZP/$PE1_O_DATApWORD);
#my $PE1_O_PREC		= $PE0_I_PREC;
#my $PE1_W_MEM_START_BASE= $MEM_H4B1_START;
my $PE1_W_MEM_START  	= $PE1_W_MEM_START_BASE  + $PE1_W*(2**13);
my $PE1_W_MEM_SIZE	= int($PE1_MAC_COL_NUM_ZP*$PE1_MAC_ROW_NUM_ZP/$PE1_W_DATApWORD);
#my $PE1_OS_MEM_START_BASE= $PE1_W_MEM_START_BASE + $PE1_W_MEM_SIZE + 1;
#my $PE1_OS_MEM_START_BASE= $MEM_H1B1_START;
my $PE1_OS_MEM_START 	= $PE1_OS_MEM_START_BASE + $PE1_OS*(2**13);
#my $PE0_W_PREC		= 0;
#my $MAC_OFFSET		= 0;
#my $MAC_NLI		= 0;
my $PE1_MAC_ROW_NUM	= $PE1_MAC_ROW_NUM_ZP;
my $PE1_FFT_NUM	 	= 0;	
my $PE1_FFT_I_UNIT_SIZE = 0;
my $PE1_FFT_O_UNIT_SIZE	= 0;
my $PE1_SHIFT		= int(($PE1_I_EXP + $PE1_W_EXP) + (log($PE1_MAC_COL_NUM_ZP)/log(2)) - $PE1_O_EXP);
#my $PE1_SHIFT		= int(($PE1_I_EXP + $PE1_W_EXP) + (log($PE1_MAC_COL_NUM_ZP)/log(2)) - $PE1_O_EXP + 1);
my $PE1_SHIFT_OS 	= 0;
my $PE1_SHIFT_NL	= 10;
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
print	sprintf("PE1_TO_MEM_START_BASE = %d\n", $PE1_TO_MEM_START);
print	sprintf("PE1_TO_MEM_END_BASE = %d\n", $PE1_TO_MEM_START + $PE1_TO_MEM_SIZE - 1);
print	sprintf("PE1_I_MEM_START_BASE  = %d\n", $PE1_I_MEM_START_BASE);
print	sprintf("PE1_I_MEM_END_BASE    = %d\n", $PE1_I_MEM_START_BASE + $PE1_I_MEM_SIZE - 1);
print	sprintf("PE1_O_MEM_START_BASE  = %d\n", $PE1_O_MEM_START_BASE);
print	sprintf("PE1_O_MEM_END_BASE    = %d\n", $PE1_O_MEM_START_BASE + $PE1_O_MEM_SIZE - 1);
print	sprintf("PE1_W_MEM_START_BASE  = %d\n", $PE1_W_MEM_START_BASE);
print	sprintf("PE1_W_MEM_END_BASE    = %d\n", $PE1_W_MEM_START_BASE + $PE1_W_MEM_SIZE - 1);
print	sprintf("PE1_OS_MEM_START_BASE = %d\n", $PE1_OS_MEM_START_BASE);
print	sprintf("PE1_OS_MEM_END_BASE   = %d\n", $PE1_OS_MEM_START_BASE + $PE1_O_MEM_SIZE - 1);
#
my $PE1_INST_0 = "";
my $PE1_INST_1 = "";
my $PE1_INST_2 = "";
my $PE1_INST_3 = "";
my $PE1_INST_4 = "";
my $PE1_INST_5 = "";
#
my $PE1_MAC_EN = 1;
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
if ($PE1_MAC_EN){
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
# GENERATE DATA FOR TESTING MAC
# PE2
#
#
my $PE2_I_DATApWORD;
my $PE2_O_DATApWORD;
my $PE2_W_DATApWORD;
#
# SET DATA PRECISION
#
my $PE2_I_PREC 		= $COMMON_I_PREC;
if ($PE2_I_PREC != $PREC_06B && $PE2_I_PREC != $PREC_08B && $PE2_I_PREC != $PREC_12B && $PE2_I_PREC != $PREC_16B){
	die "I_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE2_I_EXP;
if ($PE2_I_PREC == $PREC_06B) 	{$PE2_I_DATApWORD = 16;	$PE2_I_EXP = 6;}
if ($PE2_I_PREC == $PREC_08B) 	{$PE2_I_DATApWORD = 12;	$PE2_I_EXP = 8;}
if ($PE2_I_PREC == $PREC_12B) 	{$PE2_I_DATApWORD = 8;	$PE2_I_EXP = 12;}
if ($PE2_I_PREC == $PREC_16B) 	{$PE2_I_DATApWORD = 6;	$PE2_I_EXP = 16;}
#
my $PE2_W_PREC 		= $COMMON_W_PREC;
if ($PE2_W_PREC != $PREC_06B && $PE2_W_PREC != $PREC_08B && $PE2_W_PREC != $PREC_12B && $PE2_W_PREC != $PREC_16B){
	die "W_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE2_W_EXP;
if ($PE2_W_PREC == $PREC_06B) 	{$PE2_W_DATApWORD = 16;	$PE2_W_EXP = 6;}
if ($PE2_W_PREC == $PREC_08B) 	{$PE2_W_DATApWORD = 12;	$PE2_W_EXP = 8;}
if ($PE2_W_PREC == $PREC_12B) 	{$PE2_W_DATApWORD = 8;	$PE2_W_EXP = 12;}
if ($PE2_W_PREC == $PREC_16B) 	{$PE2_W_DATApWORD = 6;	$PE2_W_EXP = 16;}
#
my $PE2_O_PREC 		= $COMMON_O_PREC;
if ($PE2_O_PREC != $PREC_06B && $PE2_O_PREC != $PREC_08B && $PE2_O_PREC != $PREC_12B && $PE2_O_PREC != $PREC_16B){
	die "O_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE2_O_EXP;
if ($PE2_O_PREC == $PREC_06B) 	{$PE2_O_DATApWORD = 16;	$PE2_O_EXP = 6;}
if ($PE2_O_PREC == $PREC_08B) 	{$PE2_O_DATApWORD = 12;	$PE2_O_EXP = 8;}
if ($PE2_O_PREC == $PREC_12B) 	{$PE2_O_DATApWORD = 8;	$PE2_O_EXP = 12;}
if ($PE2_O_PREC == $PREC_16B) 	{$PE2_O_DATApWORD = 6;	$PE2_O_EXP = 16;}
#
my $PE2_TO_PREC		= $COMMON_TO_PREC;
my $PE2_TO_EXP;
if ($PE2_TO_PREC == $PREC_16B) 	{$PE2_TO_DATApWORD = 6;	$PE2_TO_EXP = 16;}
if ($PE2_TO_PREC == $PREC_24B) 	{$PE2_TO_DATApWORD = 4;	$PE2_TO_EXP = 24;}
if ($PE2_TO_PREC == $PREC_32B) 	{$PE2_TO_DATApWORD = 3;	$PE2_TO_EXP = 32;}
#
#my $OS_PREC		= $PE0_O_PREC
#
# GENERATE PARAMETERS FOR ZERO-PADDING
#
my $PE2_W_WORD_UNIT;
my $PE2_W_ROW_UNIT;

#
# IMPORTANT NOTES
#
# PREC_06B: DATApWORD = 16
# PREC_08B: DATApWORD = 12
# PREC_12B: DATApWORD = 08
# PREC_16B: DATApWORD = 06
#
# $W_WORD_UNIT = LCM($I_DATApWORD, $W_DATApWORD)/$W_DATApWORD
# $W_ROW_UNTI  = LCM($I_DATApWORD, $W_DATApWORD)/$I_DATApWORD
# *LCM = Least Common Multiple*
#
if ($PE2_I_PREC == $PREC_06B && $PE2_W_PREC == $PREC_06B){	$PE2_W_WORD_UNIT = 1;	$PE2_W_ROW_UNIT = 1;}
if ($PE2_I_PREC == $PREC_06B && $PE2_W_PREC == $PREC_08B){	$PE2_W_WORD_UNIT = 4;	$PE2_W_ROW_UNIT = 3;}
if ($PE2_I_PREC == $PREC_06B && $PE2_W_PREC == $PREC_12B){	$PE2_W_WORD_UNTI = 2;	$PE2_W_ROW_UNIT = 1;}
if ($PE2_I_PREC == $PREC_06B && $PE2_W_PREC == $PREC_16B){	$PE2_W_WORD_UNIT = 8; 	$PE2_W_ROW_UNIT = 3;}
#
if ($PE2_I_PREC == $PREC_08B && $PE2_W_PREC == $PREC_06B){	$PE2_W_WORD_UNIT = 3;	$PE2_W_ROW_UNIT = 4;}
if ($PE2_I_PREC == $PREC_08B && $PE2_W_PREC == $PREC_08B){	$PE2_W_WORD_UNIT = 1;	$PE2_W_ROW_UNIT = 1;}
if ($PE2_I_PREC == $PREC_08B && $PE2_W_PREC == $PREC_12B){	$PE2_W_WORD_UNTI = 3;	$PE2_W_ROW_UNIT = 2;}
if ($PE2_I_PREC == $PREC_08B && $PE2_W_PREC == $PREC_16B){	$PE2_W_WORD_UNIT = 2; 	$PE2_W_ROW_UNIT = 1;}
#
if ($PE2_I_PREC == $PREC_12B && $PE2_W_PREC == $PREC_06B){	$PE2_W_WORD_UNIT = 3;	$PE2_W_ROW_UNIT = 6;}
if ($PE2_I_PREC == $PREC_12B && $PE2_W_PREC == $PREC_08B){	$PE2_W_WORD_UNIT = 2;	$PE2_W_ROW_UNIT = 3;}
if ($PE2_I_PREC == $PREC_12B && $PE2_W_PREC == $PREC_12B){	$PE2_W_WORD_UNTI = 1;	$PE2_W_ROW_UNIT = 1;}
if ($PE2_I_PREC == $PREC_12B && $PE2_W_PREC == $PREC_16B){	$PE2_W_WORD_UNIT = 4; 	$PE2_W_ROW_UNIT = 3;}
#
if ($PE2_I_PREC == $PREC_16B && $PE2_W_PREC == $PREC_06B){	$PE2_W_WORD_UNIT = 3;	$PE2_W_ROW_UNIT = 8;}
if ($PE2_I_PREC == $PREC_16B && $PE2_W_PREC == $PREC_08B){	$PE2_W_WORD_UNIT = 1;	$PE2_W_ROW_UNIT = 2;}
if ($PE2_I_PREC == $PREC_16B && $PE2_W_PREC == $PREC_12B){	$PE2_W_WORD_UNTI = 3;	$PE2_W_ROW_UNIT = 4;}
if ($PE2_I_PREC == $PREC_16B && $PE2_W_PREC == $PREC_16B){	$PE2_W_WORD_UNIT = 1; 	$PE2_W_ROW_UNIT = 1;}

# UPDATE $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
# ADD CODE TO CALCUATE LCM LATER!
# $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#

# PARAMETERS FOR ZERO-PADDING
#
my $PE2_MAC_ROW_NUM_ZP = ceil($PE2_MAC_ROW_NUM_ORG/$ROW_NUM_MIN_FACTOR)*$ROW_NUM_MIN_FACTOR;
#my $PE2_MAC_COL_NUM_ZP = ceil($PE2_MAC_COL_NUM_ORG/$PE2_I_DATApWORD)*$PE2_I_DATApWORD;
my $PE2_MAC_COL_NUM_ZP = ceil($PE2_MAC_COL_NUM_ORG/$COL_NUM_MIN_FACTOR)*$COL_NUM_MIN_FACTOR;
# NUM ROWS TO BE ZERO-PADDED = $PE0_MAC_ROW_NUM_ZP - $MAC_ROW_NUM
# NUM COLS TO BE ZERO-PADDED = $MAC_ROW_COL_ZP - $MAC_COL_NUM

# SET MEM SPECIFICATION
#
#my $PE2_MAC_W_COL_NUM 	= int($PE2_I_MEM_SIZE*$PE2_I_DATApWORD/$PE2_W_DATApWORD);
my $PE2_MAC_W_COL_NUM 	= int($PE2_MAC_COL_NUM_ZP/$PE2_W_DATApWORD);

# GENERATE DATA FOR TESTING MAC
#
my $PE2_OP		= $OP_MAC;
#my $PE2_TO_MEM_START	= $MEM_H1B2_START;
#my $PE2_TO_MEM_START 	= $TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE2_TO_MEM_SIZE	= int($PE2_MAC_ROW_NUM_ZP/$PE2_TO_DATApWORD);
#my $PE2_TO_PREC	= 0;
#my $PE2_I_MEM_START_BASE = $MEM_H2B1_START;
my $PE2_I_MEM_START  	= $PE2_I_MEM_START_BASE  + $PE2_I*(2**13);
my $PE2_I_MEM_SIZE 	= int($PE2_MAC_COL_NUM_ZP/$PE2_I_DATApWORD);
#my $PE2_I_MEM_SIZE 	= 128;
#my $PE2_I_PREC		= $PREC_12B;
#my $PE2_O_MEM_START_BASE = $MEM_H2B3_START;
my $PE2_O_MEM_START  	= $PE2_O_MEM_START_BASE  + $PE2_O*(2**13);
my $PE2_O_MEM_SIZE 	= int($PE2_MAC_ROW_NUM_ZP/$PE2_O_DATApWORD);
#my $PE2_O_PREC		= $PE0_I_PREC;
#my $PE2_W_MEM_START_BASE= $MEM_H4B1_START;
my $PE2_W_MEM_START  	= $PE2_W_MEM_START_BASE  + $PE2_W*(2**13);
my $PE2_W_MEM_SIZE	= int($PE2_MAC_COL_NUM_ZP*$PE2_MAC_ROW_NUM_ZP/$PE2_W_DATApWORD);
#my $PE2_OS_MEM_START_BASE= $PE2_W_MEM_START_BASE + $PE2_W_MEM_SIZE + 1;
#my $PE2_OS_MEM_START_BASE= $MEM_H1B1_START;
my $PE2_OS_MEM_START 	= $PE2_OS_MEM_START_BASE + $PE2_OS*(2**13);
#my $PE0_W_PREC		= 0;
#my $MAC_OFFSET		= 0;
#my $MAC_NLI		= 0;
my $PE2_MAC_ROW_NUM	= $PE2_MAC_ROW_NUM_ZP;
my $PE2_FFT_NUM	 	= 0;	
my $PE2_FFT_I_UNIT_SIZE = 0;
my $PE2_FFT_O_UNIT_SIZE	= 0;
my $PE2_SHIFT		= int(($PE2_I_EXP + $PE2_W_EXP) + (log($PE2_MAC_COL_NUM_ZP)/log(2)) - $PE2_O_EXP);
#my $PE2_SHIFT		= int(($PE2_I_EXP + $PE2_W_EXP) + (log($PE2_MAC_COL_NUM_ZP)/log(2)) - $PE2_O_EXP + 1);
my $PE2_SHIFT_OS 	= 0;
my $PE2_SHIFT_NL	= 10;
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
print	sprintf("PE2_TO_MEM_START_BASE = %d\n", $PE2_TO_MEM_START);
print	sprintf("PE2_TO_MEM_END_BASE = %d\n", $PE2_TO_MEM_START + $PE2_TO_MEM_SIZE - 1);
print	sprintf("PE2_I_MEM_START_BASE  = %d\n", $PE2_I_MEM_START_BASE);
print	sprintf("PE2_I_MEM_END_BASE    = %d\n", $PE2_I_MEM_START_BASE + $PE2_I_MEM_SIZE - 1);
print	sprintf("PE2_O_MEM_START_BASE  = %d\n", $PE2_O_MEM_START_BASE);
print	sprintf("PE2_O_MEM_END_BASE    = %d\n", $PE2_O_MEM_START_BASE + $PE2_O_MEM_SIZE - 1);
print	sprintf("PE2_W_MEM_START_BASE  = %d\n", $PE2_W_MEM_START_BASE);
print	sprintf("PE2_W_MEM_END_BASE    = %d\n", $PE2_W_MEM_START_BASE + $PE2_W_MEM_SIZE - 1);
print	sprintf("PE2_OS_MEM_START_BASE = %d\n", $PE2_OS_MEM_START_BASE);
print	sprintf("PE2_OS_MEM_END_BASE   = %d\n", $PE2_OS_MEM_START_BASE + $PE2_O_MEM_SIZE - 1);
#
my $PE2_INST_0 = "";
my $PE2_INST_1 = "";
my $PE2_INST_2 = "";
my $PE2_INST_3 = "";
my $PE2_INST_4 = "";
my $PE2_INST_5 = "";
#
my $PE2_MAC_EN = 1;
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
if ($PE2_MAC_EN){
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
# GENERATE DATA FOR TESTING MAC
# PE3
#
#
my $PE3_I_DATApWORD;
my $PE3_O_DATApWORD;
my $PE3_W_DATApWORD;
#
# SET DATA PRECISION
#
my $PE3_I_PREC 		= $COMMON_I_PREC;
if ($PE3_I_PREC != $PREC_06B && $PE3_I_PREC != $PREC_08B && $PE3_I_PREC != $PREC_12B && $PE3_I_PREC != $PREC_16B){
	die "I_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE3_I_EXP;
if ($PE3_I_PREC == $PREC_06B) 	{$PE3_I_DATApWORD = 16;	$PE3_I_EXP = 6;}
if ($PE3_I_PREC == $PREC_08B) 	{$PE3_I_DATApWORD = 12;	$PE3_I_EXP = 8;}
if ($PE3_I_PREC == $PREC_12B) 	{$PE3_I_DATApWORD = 8;	$PE3_I_EXP = 12;}
if ($PE3_I_PREC == $PREC_16B) 	{$PE3_I_DATApWORD = 6;	$PE3_I_EXP = 16;}
#
my $PE3_W_PREC 		= $COMMON_W_PREC;
if ($PE3_W_PREC != $PREC_06B && $PE3_W_PREC != $PREC_08B && $PE3_W_PREC != $PREC_12B && $PE3_W_PREC != $PREC_16B){
	die "W_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE3_W_EXP;
if ($PE3_W_PREC == $PREC_06B) 	{$PE3_W_DATApWORD = 16;	$PE3_W_EXP = 6;}
if ($PE3_W_PREC == $PREC_08B) 	{$PE3_W_DATApWORD = 12;	$PE3_W_EXP = 8;}
if ($PE3_W_PREC == $PREC_12B) 	{$PE3_W_DATApWORD = 8;	$PE3_W_EXP = 12;}
if ($PE3_W_PREC == $PREC_16B) 	{$PE3_W_DATApWORD = 6;	$PE3_W_EXP = 16;}
#
my $PE3_O_PREC 		= $COMMON_O_PREC;
if ($PE3_O_PREC != $PREC_06B && $PE3_O_PREC != $PREC_08B && $PE3_O_PREC != $PREC_12B && $PE3_O_PREC != $PREC_16B){
	die "O_PREC SPECIFICATION IS OUT OF RANGE\n\n";
}
my $PE3_O_EXP;
if ($PE3_O_PREC == $PREC_06B) 	{$PE3_O_DATApWORD = 16;	$PE3_O_EXP = 6;}
if ($PE3_O_PREC == $PREC_08B) 	{$PE3_O_DATApWORD = 12;	$PE3_O_EXP = 8;}
if ($PE3_O_PREC == $PREC_12B) 	{$PE3_O_DATApWORD = 8;	$PE3_O_EXP = 12;}
if ($PE3_O_PREC == $PREC_16B) 	{$PE3_O_DATApWORD = 6;	$PE3_O_EXP = 16;}
#
my $PE3_TO_PREC		= $COMMON_TO_PREC;
my $PE3_TO_EXP;
if ($PE3_TO_PREC == $PREC_16B) 	{$PE3_TO_DATApWORD = 6;	$PE3_TO_EXP = 16;}
if ($PE3_TO_PREC == $PREC_24B) 	{$PE3_TO_DATApWORD = 4;	$PE3_TO_EXP = 24;}
if ($PE3_TO_PREC == $PREC_32B) 	{$PE3_TO_DATApWORD = 3;	$PE3_TO_EXP = 32;}
#
#my $OS_PREC		= $PE0_O_PREC
#
# GENERATE PARAMETERS FOR ZERO-PADDING
#
my $PE3_W_WORD_UNIT;
my $PE3_W_ROW_UNIT;

#
# IMPORTANT NOTES
#
# PREC_06B: DATApWORD = 16
# PREC_08B: DATApWORD = 12
# PREC_12B: DATApWORD = 08
# PREC_16B: DATApWORD = 06
#
# $W_WORD_UNIT = LCM($I_DATApWORD, $W_DATApWORD)/$W_DATApWORD
# $W_ROW_UNTI  = LCM($I_DATApWORD, $W_DATApWORD)/$I_DATApWORD
# *LCM = Least Common Multiple*
#
if ($PE3_I_PREC == $PREC_06B && $PE3_W_PREC == $PREC_06B){	$PE3_W_WORD_UNIT = 1;	$PE3_W_ROW_UNIT = 1;}
if ($PE3_I_PREC == $PREC_06B && $PE3_W_PREC == $PREC_08B){	$PE3_W_WORD_UNIT = 4;	$PE3_W_ROW_UNIT = 3;}
if ($PE3_I_PREC == $PREC_06B && $PE3_W_PREC == $PREC_12B){	$PE3_W_WORD_UNTI = 2;	$PE3_W_ROW_UNIT = 1;}
if ($PE3_I_PREC == $PREC_06B && $PE3_W_PREC == $PREC_16B){	$PE3_W_WORD_UNIT = 8; 	$PE3_W_ROW_UNIT = 3;}
#
if ($PE3_I_PREC == $PREC_08B && $PE3_W_PREC == $PREC_06B){	$PE3_W_WORD_UNIT = 3;	$PE3_W_ROW_UNIT = 4;}
if ($PE3_I_PREC == $PREC_08B && $PE3_W_PREC == $PREC_08B){	$PE3_W_WORD_UNIT = 1;	$PE3_W_ROW_UNIT = 1;}
if ($PE3_I_PREC == $PREC_08B && $PE3_W_PREC == $PREC_12B){	$PE3_W_WORD_UNTI = 3;	$PE3_W_ROW_UNIT = 2;}
if ($PE3_I_PREC == $PREC_08B && $PE3_W_PREC == $PREC_16B){	$PE3_W_WORD_UNIT = 2; 	$PE3_W_ROW_UNIT = 1;}
#
if ($PE3_I_PREC == $PREC_12B && $PE3_W_PREC == $PREC_06B){	$PE3_W_WORD_UNIT = 3;	$PE3_W_ROW_UNIT = 6;}
if ($PE3_I_PREC == $PREC_12B && $PE3_W_PREC == $PREC_08B){	$PE3_W_WORD_UNIT = 2;	$PE3_W_ROW_UNIT = 3;}
if ($PE3_I_PREC == $PREC_12B && $PE3_W_PREC == $PREC_12B){	$PE3_W_WORD_UNTI = 1;	$PE3_W_ROW_UNIT = 1;}
if ($PE3_I_PREC == $PREC_12B && $PE3_W_PREC == $PREC_16B){	$PE3_W_WORD_UNIT = 4; 	$PE3_W_ROW_UNIT = 3;}
#
if ($PE3_I_PREC == $PREC_16B && $PE3_W_PREC == $PREC_06B){	$PE3_W_WORD_UNIT = 3;	$PE3_W_ROW_UNIT = 8;}
if ($PE3_I_PREC == $PREC_16B && $PE3_W_PREC == $PREC_08B){	$PE3_W_WORD_UNIT = 1;	$PE3_W_ROW_UNIT = 2;}
if ($PE3_I_PREC == $PREC_16B && $PE3_W_PREC == $PREC_12B){	$PE3_W_WORD_UNTI = 3;	$PE3_W_ROW_UNIT = 4;}
if ($PE3_I_PREC == $PREC_16B && $PE3_W_PREC == $PREC_16B){	$PE3_W_WORD_UNIT = 1; 	$PE3_W_ROW_UNIT = 1;}

# UPDATE $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
# ADD CODE TO CALCUATE LCM LATER!
# $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#

# PARAMETERS FOR ZERO-PADDING
#
my $PE3_MAC_ROW_NUM_ZP = ceil($PE3_MAC_ROW_NUM_ORG/$ROW_NUM_MIN_FACTOR)*$ROW_NUM_MIN_FACTOR;
#my $PE3_MAC_COL_NUM_ZP = ceil($PE3_MAC_COL_NUM_ORG/$PE3_I_DATApWORD)*$PE3_I_DATApWORD;
my $PE3_MAC_COL_NUM_ZP = ceil($PE3_MAC_COL_NUM_ORG/$COL_NUM_MIN_FACTOR)*$COL_NUM_MIN_FACTOR;
# NUM ROWS TO BE ZERO-PADDED = $PE0_MAC_ROW_NUM_ZP - $MAC_ROW_NUM
# NUM COLS TO BE ZERO-PADDED = $MAC_ROW_COL_ZP - $MAC_COL_NUM

# SET MEM SPECIFICATION
#
#my $PE3_MAC_W_COL_NUM 	= int($PE3_I_MEM_SIZE*$PE3_I_DATApWORD/$PE3_W_DATApWORD);
my $PE3_MAC_W_COL_NUM 	= int($PE3_MAC_COL_NUM_ZP/$PE3_W_DATApWORD);

# GENERATE DATA FOR TESTING MAC
#
my $PE3_OP		= $OP_MAC;
#my $PE3_TO_MEM_START	= $MEM_H1B2_START;
#my $PE3_TO_MEM_START 	= $TO_MEM_START_BASE + $TEST_PE*(2**13);
my $PE3_TO_MEM_SIZE	= int($PE3_MAC_ROW_NUM_ZP/$PE3_TO_DATApWORD);
#my $PE3_TO_PREC	= 0;
#my $PE3_I_MEM_START_BASE = $MEM_H2B1_START;
my $PE3_I_MEM_START  	= $PE3_I_MEM_START_BASE  + $PE3_I*(2**13);
my $PE3_I_MEM_SIZE 	= int($PE3_MAC_COL_NUM_ZP/$PE3_I_DATApWORD);
#my $PE3_I_MEM_SIZE 	= 128;
#my $PE3_I_PREC		= $PREC_12B;
#my $PE3_O_MEM_START_BASE = $MEM_H2B3_START;
my $PE3_O_MEM_START  	= $PE3_O_MEM_START_BASE  + $PE3_O*(2**13);
my $PE3_O_MEM_SIZE 	= int($PE3_MAC_ROW_NUM_ZP/$PE3_O_DATApWORD);
#my $PE3_O_PREC		= $PE0_I_PREC;
#my $PE3_W_MEM_START_BASE= $MEM_H4B1_START;
my $PE3_W_MEM_START  	= $PE3_W_MEM_START_BASE  + $PE3_W*(2**13);
my $PE3_W_MEM_SIZE	= int($PE3_MAC_COL_NUM_ZP*$PE3_MAC_ROW_NUM_ZP/$PE3_W_DATApWORD);
#my $PE3_OS_MEM_START_BASE= $PE3_W_MEM_START_BASE + $PE3_W_MEM_SIZE + 1;
#my $PE3_OS_MEM_START_BASE= $MEM_H1B1_START;
my $PE3_OS_MEM_START 	= $PE3_OS_MEM_START_BASE + $PE3_OS*(2**13);
#my $PE0_W_PREC		= 0;
#my $MAC_OFFSET		= 0;
#my $MAC_NLI		= 0;
my $PE3_MAC_ROW_NUM	= $PE3_MAC_ROW_NUM_ZP;
my $PE3_FFT_NUM	 	= 0;	
my $PE3_FFT_I_UNIT_SIZE = 0;
my $PE3_FFT_O_UNIT_SIZE	= 0;
my $PE3_SHIFT		= int(($PE3_I_EXP + $PE3_W_EXP) + (log($PE3_MAC_COL_NUM_ZP)/log(2)) - $PE3_O_EXP);
#my $PE3_SHIFT		= int(($PE3_I_EXP + $PE3_W_EXP) + (log($PE3_MAC_COL_NUM_ZP)/log(2)) - $PE3_O_EXP + 1);
my $PE3_SHIFT_OS 	= 0;
my $PE3_SHIFT_NL	= 10;
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
print	sprintf("PE3_TO_MEM_START_BASE = %d\n", $PE3_TO_MEM_START);
print	sprintf("PE3_TO_MEM_END_BASE = %d\n", $PE3_TO_MEM_START + $PE3_TO_MEM_SIZE - 1);
print	sprintf("PE3_I_MEM_START_BASE  = %d\n", $PE3_I_MEM_START_BASE);
print	sprintf("PE3_I_MEM_END_BASE    = %d\n", $PE3_I_MEM_START_BASE + $PE3_I_MEM_SIZE - 1);
print	sprintf("PE3_O_MEM_START_BASE  = %d\n", $PE3_O_MEM_START_BASE);
print	sprintf("PE3_O_MEM_END_BASE    = %d\n", $PE3_O_MEM_START_BASE + $PE3_O_MEM_SIZE - 1);
print	sprintf("PE3_W_MEM_START_BASE  = %d\n", $PE3_W_MEM_START_BASE);
print	sprintf("PE3_W_MEM_END_BASE    = %d\n", $PE3_W_MEM_START_BASE + $PE3_W_MEM_SIZE - 1);
print	sprintf("PE3_OS_MEM_START_BASE = %d\n", $PE3_OS_MEM_START_BASE);
print	sprintf("PE3_OS_MEM_END_BASE   = %d\n", $PE3_OS_MEM_START_BASE + $PE3_O_MEM_SIZE - 1);
#
my $PE3_INST_0 = "";
my $PE3_INST_1 = "";
my $PE3_INST_2 = "";
my $PE3_INST_3 = "";
my $PE3_INST_4 = "";
my $PE3_INST_5 = "";
#
my $PE3_MAC_EN = 1;
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
if ($PE3_MAC_EN){
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
print 	"\n";
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
# DATA GENERATION
# GENERATE Xi, Xi+1, Ai, AND Bi
#
#########
## caogao
my @PE0_DATA_A = ();
my @PE0_DATA_B = ();
my @PE0_DATA_X = ();

my @PE1_DATA_A = ();
my @PE1_DATA_B = ();
my @PE1_DATA_X = ();

my @PE2_DATA_A = ();
my @PE2_DATA_B = ();
my @PE2_DATA_X = ();

my @PE3_DATA_A = ();
my @PE3_DATA_B = ();
my @PE3_DATA_X = ();

$LINE = <$nli_file>;
@line = split(',', $LINE);
if (scalar @line != $NLI_SEGMENT + 1) {
  die "nli file error: incorrect number of segments";
} 
for($i = 0; $i < $NLI_SEGMENT + 1; $i++){
  $PE0_DATA_X[$i] = $line[$i]; 
  $PE1_DATA_X[$i] = $line[$i]; 
  $PE2_DATA_X[$i] = $line[$i]; 
  $PE3_DATA_X[$i] = $line[$i]; 
  #### debug
#  print $PE0_DATA_X[$i], "\n";
#  print $PE1_DATA_X[$i], "\n";
#  print $PE2_DATA_X[$i], "\n";
#  print $PE3_DATA_X[$i], "\n";
#  print "\n";
  #### debug
}

$LINE = <$nli_file>;
@line = split(',', $LINE);
if (scalar @line != $NLI_SEGMENT) {
  die "nli file error: incorrect number of segments";
} 
for($i = 0; $i < $NLI_SEGMENT; $i++){
  $PE0_DATA_A[$i] = $line[$i]; 
  $PE1_DATA_A[$i] = $line[$i]; 
  $PE2_DATA_A[$i] = $line[$i]; 
  $PE3_DATA_A[$i] = $line[$i]; 
  #### debug
#  print $PE0_DATA_A[$i], "\n";
#  print $PE1_DATA_A[$i], "\n";
#  print $PE2_DATA_A[$i], "\n";
#  print $PE3_DATA_A[$i], "\n";
#  print "\n";
  #### debug
}

$LINE = <$nli_file>;
@line = split(',', $LINE);
if (scalar @line != $NLI_SEGMENT) {
  die "nli file error: incorrect number of segments";
} 
for($i = 0; $i < $NLI_SEGMENT; $i++){
  $PE0_DATA_B[$i] = $line[$i]; 
  $PE1_DATA_B[$i] = $line[$i]; 
  $PE2_DATA_B[$i] = $line[$i]; 
  $PE3_DATA_B[$i] = $line[$i]; 
#  $PE0_DATA_B[$i] = 0; 
#  $PE1_DATA_B[$i] = 0; 
#  $PE2_DATA_B[$i] = 0; 
#  $PE3_DATA_B[$i] = 0; 
  #### debug
#  print $PE0_DATA_B[$i], "\n";
#  print $PE1_DATA_B[$i], "\n";
#  print $PE2_DATA_B[$i], "\n";
#  print $PE3_DATA_B[$i], "\n";
#  print "\n";
  #### debug
}
close($nli_file);
#########

# NLI_DATA LOAD
#
$ADDR			= 0;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 1;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 2;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 3;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 4;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 5;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 6;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 7;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 8;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 9;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

$ADDR			= 10;
if ($PE0_INST == 1) {	$NLI_LD_0 = 1; 		$NLI_LD_ADDR_0 = $ADDR;		$NLI_LD_DATA_A_0 = $PE0_DATA_A[$ADDR-1]; 	$NLI_LD_DATA_B_0 = $PE0_DATA_B[$ADDR-1]; 	$NLI_LD_DATA_X_0 = $PE0_DATA_X[$ADDR];}
if ($PE1_INST == 1) {	$NLI_LD_1 = 1; 		$NLI_LD_ADDR_1 = $ADDR;		$NLI_LD_DATA_A_1 = $PE1_DATA_A[$ADDR-1]; 	$NLI_LD_DATA_B_1 = $PE1_DATA_B[$ADDR-1]; 	$NLI_LD_DATA_X_1 = $PE1_DATA_X[$ADDR];}
if ($PE2_INST == 1) {	$NLI_LD_2 = 1; 		$NLI_LD_ADDR_2 = $ADDR;		$NLI_LD_DATA_A_2 = $PE2_DATA_A[$ADDR-1]; 	$NLI_LD_DATA_B_2 = $PE2_DATA_B[$ADDR-1]; 	$NLI_LD_DATA_X_2 = $PE2_DATA_X[$ADDR];}
if ($PE3_INST == 1) {	$NLI_LD_3 = 1; 		$NLI_LD_ADDR_3 = $ADDR;		$NLI_LD_DATA_A_3 = $PE3_DATA_A[$ADDR-1]; 	$NLI_LD_DATA_B_3 = $PE3_DATA_B[$ADDR-1]; 	$NLI_LD_DATA_X_3 = $PE3_DATA_X[$ADDR];}
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

#########
## caogao
printf OUT_DNN_INST sprintf("inst\n");
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

for ($i = 0; $i < $NLI_SEGMENT + 1; $i++) { 
  if ($PE0_INST == 1 && $PE0_MAC_NLI == 1) {
    printf OUT_DNN_NLI sprintf("NLI_X[i][0][%d] = 0x%4.4x;\n", $i, $PE0_DATA_X[$i] & 0xFFFF);
  }
  if ($PE1_INST == 1 && $PE1_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_X[i][1][%d] = 0x%4.4x;\n", $i, $PE1_DATA_X[$i] & 0xFFFF);
  }
  if ($PE2_INST == 1 && $PE2_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_X[i][2][%d] = 0x%4.4x;\n", $i, $PE2_DATA_X[$i] & 0xFFFF);
  }
  if ($PE3_INST == 1 && $PE3_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_X[i][3][%d] = 0x%4.4x;\n", $i, $PE3_DATA_X[$i] & 0xFFFF);
  }
}
for ($i = 0; $i < $NLI_SEGMENT; $i++) { 
  if ($PE0_INST == 1 && $PE0_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_A[i][0][%d] = 0x%4.4x;\n", $i, $PE0_DATA_A[$i] & 0xFFFF);
  }
  if ($PE1_INST == 1 && $PE1_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_A[i][1][%d] = 0x%4.4x;\n", $i, $PE1_DATA_A[$i] & 0xFFFF);
  }
  if ($PE2_INST == 1 && $PE2_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_A[i][2][%d] = 0x%4.4x;\n", $i, $PE2_DATA_A[$i] & 0xFFFF);
  }
  if ($PE3_INST == 1 && $PE3_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_A[i][3][%d] = 0x%4.4x;\n", $i, $PE3_DATA_A[$i] & 0xFFFF);
  }
}
for ($i = 0; $i < $NLI_SEGMENT; $i++) { 
  if ($PE0_INST == 1 && $PE0_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_B[i][0][%d] = 0x%8.8x;\n", $i, $PE0_DATA_B[$i] & 0xFFFFFFFF);
  }
  if ($PE1_INST == 1 && $PE1_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_B[i][1][%d] = 0x%8.8x;\n", $i, $PE1_DATA_B[$i] & 0xFFFFFFFF);
  }
  if ($PE2_INST == 1 && $PE2_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_B[i][2][%d] = 0x%8.8x;\n", $i, $PE2_DATA_B[$i] & 0xFFFFFFFF);
  }
  if ($PE3_INST == 1 && $PE3_MAC_NLI == 1){
    printf OUT_DNN_NLI sprintf("NLI_B[i][3][%d] = 0x%8.8x;\n", $i, $PE3_DATA_B[$i] & 0xFFFFFFFF);
  }
}

#########

# DATA GENERATION & MEMORY PRINT
#
my $p=0;
my $q=0;
my $k=0;
my $PE0_I_SELECTED;
my $PE1_I_SELECTED;
my $PE2_I_SELECTED;
my $PE3_I_SELECTED;
my $A_SEL = 0;
my $B_SEL = 0;

my $LINE_MEM;
my $LINES_MEM;
my @PE0_MEM_LINES 	= ();
my @PE1_MEM_LINES 	= ();
my @PE2_MEM_LINES 	= ();
my @PE3_MEM_LINES 	= ();

#
#
# GENERATE RANDOM DATA OF I, W, OS FOR MAC
# PE0
#
#
my @PE0_DATA_I 		= ();
my @PE0_DATA_W 		= ();
my @PE0_DATA_O_NOSH	= ();
my @PE0_DATA_O_32B_OV	= ();
my @PE0_DATA_O		= ();
my @PE0_DATA_O_OS	= ();
my @PE0_DATA_O_NLI	= ();
my @PE0_DATA_OS		= ();
my @PE0_DATA_TO		= ();
my @PE0_LINE_O 		= ();
my @PE0_LINE_TO 	= ();
#
# MAC DATA SUMMARY
#
printf OUT_LOG "\n";
printf OUT_LOG "PE0 ===============\n";
printf OUT_LOG sprintf("MAC_ROW_NUM_ZP = %d\t", $PE0_MAC_ROW_NUM_ZP);
printf OUT_LOG sprintf("MAC_ROW_NUM_ORG = %d\n", $PE0_MAC_ROW_NUM_ORG);
printf OUT_LOG sprintf("MAC_COL_NUM_ZP = %d\t", $PE0_MAC_COL_NUM_ZP);
printf OUT_LOG sprintf("MAC_COL_NUM_ORG = %d\n", $PE0_MAC_COL_NUM_ORG);
printf OUT_LOG sprintf("MAC_W_COL_NUM = %d\n", $PE0_MAC_W_COL_NUM);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("SHIFT    = %d\n", $PE0_SHIFT);
printf OUT_LOG sprintf("SHIFT_OS = %d\n", $PE0_SHIFT_OS);
printf OUT_LOG sprintf("SHIFT_NL = %d\n", $PE0_SHIFT_NL);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL I WITH ZERO-PADDING = %d\n", $PE0_I_MEM_SIZE*$PE0_I_DATApWORD);
printf OUT_LOG sprintf("TOTAL O WITH ZERO-PADDING = %d\n", $PE0_O_MEM_SIZE*$PE0_O_DATApWORD);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL WORD SIZE OF I  WITH ZERO-PADDING = %d\n", $PE0_I_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF O  WITH ZERO-PADDING = %d\n", $PE0_O_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF W  WITH ZERO-PADDING = %d\n", $PE0_W_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF TO WITH ZERO-PADDING = %d\n", $PE0_TO_MEM_SIZE);
printf OUT_LOG "\n";
if ($PE0_W_PREC == $PREC_06B) 	{printf OUT_LOG "W_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE0_W_PREC == $PREC_08B) 	{printf OUT_LOG "W_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE0_W_PREC == $PREC_12B) 	{printf OUT_LOG "W_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE0_W_PREC == $PREC_16B) 	{printf OUT_LOG "W_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE0_I_PREC == $PREC_06B) 	{printf OUT_LOG "I_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE0_I_PREC == $PREC_08B) 	{printf OUT_LOG "I_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE0_I_PREC == $PREC_12B) 	{printf OUT_LOG "I_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE0_I_PREC == $PREC_16B) 	{printf OUT_LOG "I_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE0_O_PREC == $PREC_06B) 	{printf OUT_LOG "O_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE0_O_PREC == $PREC_08B) 	{printf OUT_LOG "O_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE0_O_PREC == $PREC_12B) 	{printf OUT_LOG "O_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE0_O_PREC == $PREC_16B) 	{printf OUT_LOG "O_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE0_TO_PREC == $PREC_16B) 	{printf OUT_LOG "TO_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE0_TO_PREC == $PREC_24B) 	{printf OUT_LOG "TO_PREC = 24B\t";	printf OUT_LOG "DATApWORD = 04\n";}
if ($PE0_TO_PREC == $PREC_32B) 	{printf OUT_LOG "TO_PREC = 32B\t";	printf OUT_LOG "DATApWORD = 03\n";}
printf OUT_LOG "\n";
#
if ($PE0_I_PREC == $PREC_06B) 	{print "I_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE0_I_PREC == $PREC_08B) 	{print "I_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE0_I_PREC == $PREC_12B) 	{print "I_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE0_I_PREC == $PREC_16B) 	{print "I_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE0_W_PREC == $PREC_06B) 	{print "W_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE0_W_PREC == $PREC_08B) 	{print "W_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE0_W_PREC == $PREC_12B) 	{print "W_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE0_W_PREC == $PREC_16B) 	{print "W_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE0_O_PREC == $PREC_06B) 	{print "O_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE0_O_PREC == $PREC_08B) 	{print "O_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE0_O_PREC == $PREC_12B) 	{print "O_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE0_O_PREC == $PREC_16B) 	{print "O_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE0_TO_PREC == $PREC_16B) 	{print "TO_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE0_TO_PREC == $PREC_24B) 	{print "TO_PREC = 24B\t";	print "DATApWORD = 04\n";}
if ($PE0_TO_PREC == $PREC_32B) 	{print "TO_PREC = 32B\t";	print "DATApWORD = 03\n";}
print "\n";
#
# MAC DATA RANDOM GENERATION & CALCULATION
#
###############################################################
# caogao
$k=0;
for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
	## 
	if ($i < $PE0_MAC_ROW_NUM_ORG) {
	  $LINE = <$weight_file>;			# size 
	  @line = split / /,$LINE;		## TODO: check number of variables
	}
	##
	for($j = 0; $j < $PE0_MAC_COL_NUM_ZP; $j++){
		if($i >= $PE0_MAC_ROW_NUM_ORG || $j >= $PE0_MAC_COL_NUM_ORG){
			$PE0_DATA_W[$k] = 0;
		}else{ 	
			if($PE0_W_EXP != 16){ 	$PE0_DATA_W[$k] = int(rand(2**($PE0_W_EXP)-1) - 2**($PE0_W_EXP-1));}
			if($PE0_W_EXP == 16){ 	$PE0_DATA_W[$k] = int(rand(2**(13)-1) - 2**(13-1));}
			$PE0_DATA_W[$k] = $line[$j];
      if (abs($PE0_DATA_W[$k]) > (2**($PE0_W_EXP) - 1)) {
        print ("weight exceeds fixed point precision range!!\n");
        exit(0);
      }
		}
#		print sprintf("At i= %d: %.2x %.2x\n", $k, $PE0_DATA_W[$k]);
		$k++;
	}
}
$k=0;
##
$LINE = <$input_file>;			# size 
my @input_line = split / /,$LINE;		## TODO: check number of variables
##
for($j = 0; $j < $PE0_MAC_COL_NUM_ZP; $j++){
	if($j >= $PE0_MAC_COL_NUM_ORG){
		$PE0_DATA_I[$k] = 0;
	}else{
		$PE0_DATA_I[$k] = int(rand(2**($PE0_I_EXP)-1) - 2**($PE0_I_EXP-1));
		$PE0_DATA_I[$k] = $input_line[$j];
    if (abs($PE0_DATA_I[$k]) > (2**($PE0_I_EXP) - 1)) {
      print ("input exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
$k=0;
## 
$LINE = <$offset_file>;			# size 
my @offset_line = split / /,$LINE;		## TODO: check number of variables
##
for($j = 0; $j < $PE0_MAC_ROW_NUM_ZP; $j++){
	if($j >= $PE0_MAC_ROW_NUM_ORG){
		$PE0_DATA_OS[$k] = 0;
	}else{
		$PE0_DATA_OS[$k] = int(rand(2**($PE0_O_EXP)-1) - 2**($PE0_O_EXP-1));
		$PE0_DATA_OS[$k] = $offset_line[$j + $MAC_ROW_NUM_ORG * 0];
    if (abs($PE0_DATA_OS[$k]) > (2**($PE0_O_EXP) - 1)) {
      print ("offset exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
############################################################################
#
# CALCULATE MAC OUTPUTS $PE0_DATA_O
#
$k=0;
printf OUT_LOG "\n";
printf OUT_LOG "DATA_TO TRACKING\n\t";
	for($j = 0; $j < $PE0_MAC_COL_NUM_ZP; $j++){
		if(int($j % $PE0_I_DATApWORD) eq int($PE0_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t", floor($j/ $PE0_I_DATApWORD));}
	}
	printf OUT_LOG "\n";
for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
	$PE0_DATA_TO[$i] = 0;
	$PE0_DATA_O_NOSH[$i] = 0;
	printf OUT_LOG sprintf("%7d\t",$i);
	for($j = 0; $j < $PE0_MAC_COL_NUM_ZP; $j++){
		if($j < $PE0_MAC_COL_NUM_ZP - $PE0_I_DATApWORD){
			$PE0_DATA_TO[$i] = $PE0_DATA_TO[$i] + $PE0_DATA_I[$j]*$PE0_DATA_W[$k];
			if(int($j % $PE0_I_DATApWORD) eq int($PE0_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_TO[$i]);}
		}
		$PE0_DATA_O_NOSH[$i] = $PE0_DATA_O_NOSH[$i] + $PE0_DATA_I[$j]*$PE0_DATA_W[$k];
		$k++;
	}
	printf OUT_LOG "\n";
	#print sprintf("(i=%3d, j=%3d) PE0_DATA_TO= %7d\t",$i,$j,$PE0_DATA_TO[$i]);
	#print "\n";
	#print sprintf("At i=%4.4d j=%4.4d: PE0_DATA_O_NOSH= %.d\t = %.d\n",$i,$j,$PE0_DATA_O_NOSH[$i]);

	if($PE0_DATA_O_NOSH[$i] > 2**($PE0_TO_EXP-1)-1) {	$PE0_DATA_O_32B_OV[$i] = int($PE0_DATA_O_NOSH[$i] -  floor($PE0_DATA_O_NOSH[$i]/(2**$PE0_TO_EXP))*(2**$PE0_TO_EXP));}
	elsif($PE0_DATA_O_NOSH[$i] < -2**($PE0_TO_EXP-1))  {	$PE0_DATA_O_32B_OV[$i] = -int($PE0_DATA_O_NOSH[$i] -  floor($PE0_DATA_O_NOSH[$i]/(2**$PE0_TO_EXP))*(2**$PE0_TO_EXP));}
	else {						$PE0_DATA_O_32B_OV[$i] = $PE0_DATA_O_NOSH[$i];}
	
	$PE0_DATA_O_32B_OV[$i] = $PE0_DATA_O_NOSH[$i];
	#$PE0_DATA_O_32B_OV[$i] = int($PE0_DATA_O_NOSH[$i] & 0xFFFFFFFF);
	#$PE0_DATA_O_32B_OV[$i] = int($PE0_DATA_O_NOSH[$i] -  floor($PE0_DATA_O_NOSH[$i]/(2**32))*(2**32));
		
	#$PE0_DATA_O[$i] = floor($PE0_DATA_O_NOSH[$i]/(2**$SHIFT));
	$PE0_DATA_O[$i] = floor($PE0_DATA_O_32B_OV[$i]/(2**$PE0_SHIFT));
	if($PE0_DATA_O[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O[$i] = 2**($PE0_O_EXP-1)-1;}
	if($PE0_DATA_O[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O[$i] = -2**($PE0_O_EXP-1);}

	print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE0_DATA_O[$i],$PE0_SHIFT);
  ## new 5/24/16
#	if($PE0_MAC_OFFSET){ 
	if($PE0_MAC_OFFSET == 1 && $PE0_MAC_NLI == 0){ 
  ## new 5/24/16
		$PE0_DATA_O_OS[$i] = floor(($PE0_DATA_O[$i] + $PE0_DATA_OS[$i])/(2**$PE0_SHIFT_OS));
	}else{
		$PE0_DATA_O_OS[$i] = $PE0_DATA_O[$i];
	}
	# NLI FUNCTION IMPLEMENTATION	
	#
	if($PE0_MAC_NLI){
		$PE0_I_SELECTED = -1;
		for($q = 0; $q < $NLI_SEGMENT; $q++){
			#if($DATA_O_OS[$i] >= $DATA_X[$q] && $DATA_O_OS[$i] < $DATA_X[$q+1]){
			if($PE0_DATA_O_OS[$i] >= $PE0_DATA_X[$q] && $PE0_DATA_O_OS[$i] <= $PE0_DATA_X[$q+1]){
				$PE0_I_SELECTED = $q;
			}
		}
		if($PE0_I_SELECTED == -1) {
			$A_SEL = 0;
			$B_SEL = 0;
		}else{
			$A_SEL = $PE0_DATA_A[$PE0_I_SELECTED];
			$B_SEL = $PE0_DATA_B[$PE0_I_SELECTED];
		}
    ########
    ## caogao
    # floor is more accurate representation of Verilog implementation
#		$PE0_DATA_O_NLI[$i] = int(($PE0_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE0_SHIFT_NL));
		$PE0_DATA_O_NLI[$i] = floor(($PE0_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE0_SHIFT_NL));
    #### 

		if($PE0_DATA_O_NLI[$i] > 2**($PE0_O_EXP-1)-1) {		$PE0_DATA_O_NLI[$i] = 2**($PE0_O_EXP-1)-1;}
		if($PE0_DATA_O_NLI[$i] < -2**($PE0_O_EXP-1))  {		$PE0_DATA_O_NLI[$i] = -2**($PE0_O_EXP-1);}

		$LINE_DBG	= sprintf("PE0_X = %4.4x  ", $PE0_DATA_O_OS[$i] & 0xFFFF);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("%+6d ", $PE0_DATA_O_OS[$i]);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE0_I_SELECTED = %2d ", $PE0_I_SELECTED);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE0_A_SELECTED = %+6d ", $A_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE0_B_SELECTED = %+12d ", $B_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tA*X+B (dec) = %+12d ", $PE0_DATA_O_OS[$i]*$A_SEL+$B_SEL);	
		#printf OUT_LOG sprintf("A*X+B (hex) = %8.8x  ", ($DATA_O_OS[$i]*$A_SEL[]+$B_SEL[]) & 0xFFFFFFFF);
		#printf OUT_LOG sprintf("(A*X+B)/2^SHIFT_NL (dec) = %+d  ", $DATA_O_NLI[$i]);					
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\t(A*X+B)/2^SHIFT_NL (hex) = %4.4x\n", $PE0_DATA_O_NLI[$i] & 0xFFFF);			
		$LINES_DBG[$IDX_DBG] = "$LINE_DBG";
		$IDX_DBG++;

	}else{
		$PE0_DATA_O_NLI[$i] = $PE0_DATA_O_OS[$i];
	}
	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE0_DATA_O[$i]);

  ## new 5/25/16
  if ($PE0_O_SIGN == 0 && $PE0_DATA_O_NLI[$i] < 0) {
		$PE0_DATA_O_NLI[$i] = 0;
  }
}
#
if (1){
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_W\t";
	for($i = 0; $i < $PE0_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n";
	$k = 0;
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$i);
		for($j = 0; $j < $PE0_MAC_COL_NUM_ZP; $j++){
			printf OUT_LOG sprintf("%7d\t",$PE0_DATA_W[$k]);
			$k++;
		}
		printf OUT_LOG "\n";
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_I\t";
	for($i = 0; $i < $PE0_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_COL_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_I[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_OS\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_OS[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_TO\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_TO[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noSH)\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_O_NOSH[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		#printf OUT_LOG sprintf("%3.3d= %12.12x\t",$i,$PE0_DATA_O_NOSH[$i]&0xFFFFFFFFFFFF);
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE0_DATA_O_NOSH[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(32bOV)\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_O_32B_OV[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE0_DATA_O_32B_OV[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noOS)\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_O[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE0_DATA_O[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/OS)\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_O_OS[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE0_DATA_O_OS[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/NLI)\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE0_DATA_O_NLI[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE0_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE0_DATA_O_NLI[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "\n";
}
#
#
# GENERATE RANDOM DATA OF I, W, OS FOR MAC
# PE1
#
#
my @PE1_DATA_I 		= ();
my @PE1_DATA_W 		= ();
my @PE1_DATA_O_NOSH	= ();
my @PE1_DATA_O_32B_OV	= ();
my @PE1_DATA_O		= ();
my @PE1_DATA_O_OS	= ();
my @PE1_DATA_O_NLI = ();
my @PE1_DATA_OS		= ();
my @PE1_DATA_TO		= ();
my @PE1_LINE_O 		= ();
my @PE1_LINE_TO 	= ();
#
# MAC DATA SUMMARY
#
printf OUT_LOG "\n";
printf OUT_LOG "PE1 ===============\n";
printf OUT_LOG sprintf("MAC_ROW_NUM_ZP = %d\t", $PE1_MAC_ROW_NUM_ZP);
printf OUT_LOG sprintf("MAC_ROW_NUM_ORG = %d\n", $PE1_MAC_ROW_NUM_ORG);
printf OUT_LOG sprintf("MAC_COL_NUM_ZP = %d\t", $PE1_MAC_COL_NUM_ZP);
printf OUT_LOG sprintf("MAC_COL_NUM_ORG = %d\n", $PE1_MAC_COL_NUM_ORG);
printf OUT_LOG sprintf("MAC_W_COL_NUM = %d\n", $PE1_MAC_W_COL_NUM);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("SHIFT    = %d\n", $PE1_SHIFT);
printf OUT_LOG sprintf("SHIFT_OS = %d\n", $PE1_SHIFT_OS);
printf OUT_LOG sprintf("SHIFT_NL = %d\n", $PE1_SHIFT_NL);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL I WITH ZERO-PADDING = %d\n", $PE1_I_MEM_SIZE*$PE1_I_DATApWORD);
printf OUT_LOG sprintf("TOTAL O WITH ZERO-PADDING = %d\n", $PE1_O_MEM_SIZE*$PE1_O_DATApWORD);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL WORD SIZE OF I  WITH ZERO-PADDING = %d\n", $PE1_I_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF O  WITH ZERO-PADDING = %d\n", $PE1_O_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF W  WITH ZERO-PADDING = %d\n", $PE1_W_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF TO WITH ZERO-PADDING = %d\n", $PE1_TO_MEM_SIZE);
printf OUT_LOG "\n";
if ($PE1_W_PREC == $PREC_06B) 	{printf OUT_LOG "W_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE1_W_PREC == $PREC_08B) 	{printf OUT_LOG "W_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE1_W_PREC == $PREC_12B) 	{printf OUT_LOG "W_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE1_W_PREC == $PREC_16B) 	{printf OUT_LOG "W_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE1_I_PREC == $PREC_06B) 	{printf OUT_LOG "I_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE1_I_PREC == $PREC_08B) 	{printf OUT_LOG "I_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE1_I_PREC == $PREC_12B) 	{printf OUT_LOG "I_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE1_I_PREC == $PREC_16B) 	{printf OUT_LOG "I_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE1_O_PREC == $PREC_06B) 	{printf OUT_LOG "O_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE1_O_PREC == $PREC_08B) 	{printf OUT_LOG "O_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE1_O_PREC == $PREC_12B) 	{printf OUT_LOG "O_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE1_O_PREC == $PREC_16B) 	{printf OUT_LOG "O_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE1_TO_PREC == $PREC_16B) 	{printf OUT_LOG "TO_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE1_TO_PREC == $PREC_24B) 	{printf OUT_LOG "TO_PREC = 24B\t";	printf OUT_LOG "DATApWORD = 04\n";}
if ($PE1_TO_PREC == $PREC_32B) 	{printf OUT_LOG "TO_PREC = 32B\t";	printf OUT_LOG "DATApWORD = 03\n";}
printf OUT_LOG "\n";
#
if ($PE1_I_PREC == $PREC_06B) 	{print "I_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE1_I_PREC == $PREC_08B) 	{print "I_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE1_I_PREC == $PREC_12B) 	{print "I_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE1_I_PREC == $PREC_16B) 	{print "I_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE1_W_PREC == $PREC_06B) 	{print "W_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE1_W_PREC == $PREC_08B) 	{print "W_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE1_W_PREC == $PREC_12B) 	{print "W_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE1_W_PREC == $PREC_16B) 	{print "W_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE1_O_PREC == $PREC_06B) 	{print "O_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE1_O_PREC == $PREC_08B) 	{print "O_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE1_O_PREC == $PREC_12B) 	{print "O_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE1_O_PREC == $PREC_16B) 	{print "O_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE1_TO_PREC == $PREC_16B) 	{print "TO_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE1_TO_PREC == $PREC_24B) 	{print "TO_PREC = 24B\t";	print "DATApWORD = 04\n";}
if ($PE1_TO_PREC == $PREC_32B) 	{print "TO_PREC = 32B\t";	print "DATApWORD = 03\n";}
print "\n";
#
# MAC DATA RANDOM GENERATION & CALCULATION
#
###############################################################
# caogao
$k=0;
for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
	## caogao
  if ($PE1_INST) {
	  if ($i < $PE1_MAC_ROW_NUM_ORG) {
	    $LINE = <$weight_file>;			# size 
	    @line = split / /,$LINE;		## TODO: check number of variables
	  }
	##
	  for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
	  	if($i >= $PE1_MAC_ROW_NUM_ORG || $j >= $PE1_MAC_COL_NUM_ORG){
	  		$PE1_DATA_W[$k] = 0;
	  	}else{ 	
	  		if($PE1_W_EXP != 16){ 	$PE1_DATA_W[$k] = int(rand(2**($PE1_W_EXP)-1) - 2**($PE1_W_EXP-1));}
	  		if($PE1_W_EXP == 16){ 	$PE1_DATA_W[$k] = int(rand(2**(13)-1) - 2**(13-1));}
	  		$PE1_DATA_W[$k] = $line[$j];
        if (abs($PE1_DATA_W[$k]) > (2**($PE1_W_EXP) - 1)) {
          print ("weight exceeds fixed point precision range!!\n");
          exit(0);
        }
	  	}
#	  	print sprintf("At i= %d: %.2x %.2x\n", $k, $PE0_DATA_W[$k]);
	  	$k++;
	  }
  } else {
	  for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
	    $PE1_DATA_W[$j + $i * $PE1_MAC_COL_NUM_ZP] = 0;
    }
  }
}
##
# no need to read line -- still using the same line
## 
$k=0;
for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
	if($j >= $PE1_MAC_COL_NUM_ORG){
		$PE1_DATA_I[$k] = 0;
	}else{
		$PE1_DATA_I[$k] = int(rand(2**($PE1_I_EXP)-1) - 2**($PE1_I_EXP-1));
		$PE1_DATA_I[$k] = $input_line[$j];
    if (abs($PE1_DATA_I[$k]) > (2**($PE1_I_EXP) - 1)) {
      print ("input exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
##
# no need to read line -- still using the same line
## 
$k=0;
for($j = 0; $j < $PE1_MAC_ROW_NUM_ZP; $j++){
	if($j >= $PE1_MAC_ROW_NUM_ORG || (not $PE1_INST)) {
		$PE1_DATA_OS[$k] = 0;
	}else{
		$PE1_DATA_OS[$k] = int(rand(2**($PE1_O_EXP)-1) - 2**($PE1_O_EXP-1));
		$PE1_DATA_OS[$k] = $offset_line[$j + $MAC_ROW_NUM_ORG * 1];
    if (abs($PE1_DATA_OS[$k]) > (2**($PE1_O_EXP) - 1)) {
      print ("offset exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
###############################################################
#
# CALCULATE MAC OUTPUTS $PE1_DATA_O
#
$k=0;
printf OUT_LOG "\n";
printf OUT_LOG "DATA_TO TRACKING\n\t";
	for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
		if(int($j % $PE1_I_DATApWORD) eq int($PE1_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t", floor($j/ $PE1_I_DATApWORD));}
	}
	printf OUT_LOG "\n";
for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
	$PE1_DATA_TO[$i] = 0;
	$PE1_DATA_O_NOSH[$i] = 0;
	printf OUT_LOG sprintf("%7d\t",$i);
	for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
		if($j < $PE1_MAC_COL_NUM_ZP - $PE1_I_DATApWORD){
			$PE1_DATA_TO[$i] = $PE1_DATA_TO[$i] + $PE1_DATA_I[$j]*$PE1_DATA_W[$k];
			if(int($j % $PE1_I_DATApWORD) eq int($PE1_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_TO[$i]);}
		}
		$PE1_DATA_O_NOSH[$i] = $PE1_DATA_O_NOSH[$i] + $PE1_DATA_I[$j]*$PE1_DATA_W[$k];
		$k++;
	}
	printf OUT_LOG "\n";
	#print sprintf("(i=%3d, j=%3d) PE1_DATA_TO= %7d\t",$i,$j,$PE1_DATA_TO[$i]);
	#print "\n";
	#print sprintf("At i=%4.4d j=%4.4d: PE1_DATA_O_NOSH= %.d\t = %.d\n",$i,$j,$PE1_DATA_O_NOSH[$i]);

	if($PE1_DATA_O_NOSH[$i] > 2**($PE1_TO_EXP-1)-1) {	$PE1_DATA_O_32B_OV[$i] = int($PE1_DATA_O_NOSH[$i] -  floor($PE1_DATA_O_NOSH[$i]/(2**$PE1_TO_EXP))*(2**$PE1_TO_EXP));}
	elsif($PE1_DATA_O_NOSH[$i] < -2**($PE1_TO_EXP-1))  {	$PE1_DATA_O_32B_OV[$i] = -int($PE1_DATA_O_NOSH[$i] -  floor($PE1_DATA_O_NOSH[$i]/(2**$PE1_TO_EXP))*(2**$PE1_TO_EXP));}
	else {						$PE1_DATA_O_32B_OV[$i] = $PE1_DATA_O_NOSH[$i];}
	
	$PE1_DATA_O_32B_OV[$i] = $PE1_DATA_O_NOSH[$i];
	#$PE1_DATA_O_32B_OV[$i] = int($PE1_DATA_O_NOSH[$i] & 0xFFFFFFFF);
	#$PE1_DATA_O_32B_OV[$i] = int($PE1_DATA_O_NOSH[$i] -  floor($PE1_DATA_O_NOSH[$i]/(2**32))*(2**32));
		
	#$PE1_DATA_O[$i] = floor($PE1_DATA_O_NOSH[$i]/(2**$SHIFT));
	$PE1_DATA_O[$i] = floor($PE1_DATA_O_32B_OV[$i]/(2**$PE1_SHIFT));
	if($PE1_DATA_O[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O[$i] = 2**($PE1_O_EXP-1)-1;}
	if($PE1_DATA_O[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O[$i] = -2**($PE1_O_EXP-1);}

	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE1_DATA_O[$i]);
  ## new 5/24/16
#	if($PE1_MAC_OFFSET){ 
	if($PE1_MAC_OFFSET == 1 && $PE1_MAC_NLI == 0){ 
  ## new 5/24/16
		$PE1_DATA_O_OS[$i] = floor(($PE1_DATA_O[$i] + $PE1_DATA_OS[$i])/(2**$PE1_SHIFT_OS));
	}else{
		$PE1_DATA_O_OS[$i] = $PE1_DATA_O[$i];
	}
	# NLI FUNCTION IMPLEMENTATION	
	#
	if($PE1_MAC_NLI){
		$PE1_I_SELECTED = -1;
		for($q = 0; $q < $NLI_SEGMENT; $q++){
			#if($DATA_O_OS[$i] >= $DATA_X[$q] && $DATA_O_OS[$i] < $DATA_X[$q+1]){
			if($PE1_DATA_O_OS[$i] >= $PE1_DATA_X[$q] && $PE1_DATA_O_OS[$i] <= $PE1_DATA_X[$q+1]){
				$PE1_I_SELECTED = $q;
			}
		}
		if($PE1_I_SELECTED == -1) {
			$A_SEL = 0;
			$B_SEL = 0;
		}else{
			$A_SEL = $PE1_DATA_A[$PE1_I_SELECTED];
			$B_SEL = $PE1_DATA_B[$PE1_I_SELECTED];
		}
    ######## 
    ## caogao
#		$PE1_DATA_O_NLI[$i] = int(($PE1_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE1_SHIFT_NL));
		$PE1_DATA_O_NLI[$i] = floor(($PE1_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE1_SHIFT_NL));
    ######## 

		if($PE1_DATA_O_NLI[$i] > 2**($PE1_O_EXP-1)-1) {		$PE1_DATA_O_NLI[$i] = 2**($PE1_O_EXP-1)-1;}
		if($PE1_DATA_O_NLI[$i] < -2**($PE1_O_EXP-1))  {		$PE1_DATA_O_NLI[$i] = -2**($PE1_O_EXP-1);}

		$LINE_DBG	= sprintf("PE1_X = %4.4x  ", $PE1_DATA_O_OS[$i] & 0xFFFF);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("%+6d ", $PE1_DATA_O_OS[$i]);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE1_I_SELECTED = %2d ", $PE1_I_SELECTED);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE1_A_SELECTED = %+6d ", $A_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE1_B_SELECTED = %+12d ", $B_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tA*X+B (dec) = %+12d ", $PE1_DATA_O_OS[$i]*$A_SEL+$B_SEL);	
		#printf OUT_LOG sprintf("A*X+B (hex) = %8.8x  ", ($DATA_O_OS[$i]*$A_SEL[]+$B_SEL[]) & 0xFFFFFFFF);
		#printf OUT_LOG sprintf("(A*X+B)/2^SHIFT_NL (dec) = %+d  ", $DATA_O_NLI[$i]);					
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\t(A*X+B)/2^SHIFT_NL (hex) = %4.4x\n", $PE1_DATA_O_NLI[$i] & 0xFFFF);			
		$LINES_DBG[$IDX_DBG] = "$LINE_DBG";
		$IDX_DBG++;

	}else{
		$PE1_DATA_O_NLI[$i] = $PE1_DATA_O_OS[$i];
	}
	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE1_DATA_O[$i]);

  ## new 5/25/16
  if ($PE1_O_SIGN == 0 && $PE1_DATA_O_NLI[$i] < 0) {
		$PE1_DATA_O_NLI[$i] = 0;
  }
  ## new 5/25/16
}
#
if (1){
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_W\t";
	for($i = 0; $i < $PE1_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n";
	$k = 0;
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$i);
		for($j = 0; $j < $PE1_MAC_COL_NUM_ZP; $j++){
			printf OUT_LOG sprintf("%7d\t",$PE1_DATA_W[$k]);
			$k++;
		}
		printf OUT_LOG "\n";
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_I\t";
	for($i = 0; $i < $PE1_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_COL_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_I[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_OS\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_OS[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_TO\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_TO[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noSH)\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_O_NOSH[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		#printf OUT_LOG sprintf("%3.3d= %12.12x\t",$i,$PE1_DATA_O_NOSH[$i]&0xFFFFFFFFFFFF);
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE1_DATA_O_NOSH[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(32bOV)\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_O_32B_OV[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE1_DATA_O_32B_OV[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noOS)\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_O[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE1_DATA_O[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/OS)\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_O_OS[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE1_DATA_O_OS[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/NLI)\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE1_DATA_O_NLI[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE1_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE1_DATA_O_NLI[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "\n";
}
#
#
# GENERATE RANDOM DATA OF I, W, OS FOR MAC
# PE2
#
#
my @PE2_DATA_I 		= ();
my @PE2_DATA_W 		= ();
my @PE2_DATA_O_NOSH	= ();
my @PE2_DATA_O_32B_OV	= ();
my @PE2_DATA_O		= ();
my @PE2_DATA_O_OS	= ();
my @PE2_DATA_O_NLI	= ();
my @PE2_DATA_OS		= ();
my @PE2_DATA_TO		= ();
my @PE2_LINE_O 		= ();
my @PE2_LINE_TO 	= ();
#
# MAC DATA SUMMARY
#
printf OUT_LOG "\n";
printf OUT_LOG "PE2 ===============\n";
printf OUT_LOG sprintf("MAC_ROW_NUM_ZP = %d\t", $PE2_MAC_ROW_NUM_ZP);
printf OUT_LOG sprintf("MAC_ROW_NUM_ORG = %d\n", $PE2_MAC_ROW_NUM_ORG);
printf OUT_LOG sprintf("MAC_COL_NUM_ZP = %d\t", $PE2_MAC_COL_NUM_ZP);
printf OUT_LOG sprintf("MAC_COL_NUM_ORG = %d\n", $PE2_MAC_COL_NUM_ORG);
printf OUT_LOG sprintf("MAC_W_COL_NUM = %d\n", $PE2_MAC_W_COL_NUM);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("SHIFT    = %d\n", $PE2_SHIFT);
printf OUT_LOG sprintf("SHIFT_OS = %d\n", $PE2_SHIFT_OS);
printf OUT_LOG sprintf("SHIFT_NL = %d\n", $PE2_SHIFT_NL);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL I WITH ZERO-PADDING = %d\n", $PE2_I_MEM_SIZE*$PE2_I_DATApWORD);
printf OUT_LOG sprintf("TOTAL O WITH ZERO-PADDING = %d\n", $PE2_O_MEM_SIZE*$PE2_O_DATApWORD);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL WORD SIZE OF I  WITH ZERO-PADDING = %d\n", $PE2_I_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF O  WITH ZERO-PADDING = %d\n", $PE2_O_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF W  WITH ZERO-PADDING = %d\n", $PE2_W_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF TO WITH ZERO-PADDING = %d\n", $PE2_TO_MEM_SIZE);
printf OUT_LOG "\n";
if ($PE2_W_PREC == $PREC_06B) 	{printf OUT_LOG "W_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE2_W_PREC == $PREC_08B) 	{printf OUT_LOG "W_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE2_W_PREC == $PREC_12B) 	{printf OUT_LOG "W_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE2_W_PREC == $PREC_16B) 	{printf OUT_LOG "W_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE2_I_PREC == $PREC_06B) 	{printf OUT_LOG "I_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE2_I_PREC == $PREC_08B) 	{printf OUT_LOG "I_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE2_I_PREC == $PREC_12B) 	{printf OUT_LOG "I_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE2_I_PREC == $PREC_16B) 	{printf OUT_LOG "I_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE2_O_PREC == $PREC_06B) 	{printf OUT_LOG "O_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE2_O_PREC == $PREC_08B) 	{printf OUT_LOG "O_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE2_O_PREC == $PREC_12B) 	{printf OUT_LOG "O_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE2_O_PREC == $PREC_16B) 	{printf OUT_LOG "O_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE2_TO_PREC == $PREC_16B) 	{printf OUT_LOG "TO_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE2_TO_PREC == $PREC_24B) 	{printf OUT_LOG "TO_PREC = 24B\t";	printf OUT_LOG "DATApWORD = 04\n";}
if ($PE2_TO_PREC == $PREC_32B) 	{printf OUT_LOG "TO_PREC = 32B\t";	printf OUT_LOG "DATApWORD = 03\n";}
printf OUT_LOG "\n";
#
if ($PE2_I_PREC == $PREC_06B) 	{print "I_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE2_I_PREC == $PREC_08B) 	{print "I_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE2_I_PREC == $PREC_12B) 	{print "I_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE2_I_PREC == $PREC_16B) 	{print "I_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE2_W_PREC == $PREC_06B) 	{print "W_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE2_W_PREC == $PREC_08B) 	{print "W_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE2_W_PREC == $PREC_12B) 	{print "W_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE2_W_PREC == $PREC_16B) 	{print "W_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE2_O_PREC == $PREC_06B) 	{print "O_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE2_O_PREC == $PREC_08B) 	{print "O_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE2_O_PREC == $PREC_12B) 	{print "O_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE2_O_PREC == $PREC_16B) 	{print "O_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE2_TO_PREC == $PREC_16B) 	{print "TO_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE2_TO_PREC == $PREC_24B) 	{print "TO_PREC = 24B\t";	print "DATApWORD = 04\n";}
if ($PE2_TO_PREC == $PREC_32B) 	{print "TO_PREC = 32B\t";	print "DATApWORD = 03\n";}
print "\n";
#
# MAC DATA RANDOM GENERATION & CALCULATION
#
##########################################
## caogao
$k=0;
for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
	## 
  if ($PE2_INST) {
	  if ($i < $PE2_MAC_ROW_NUM_ORG) {
	    $LINE = <$weight_file>; 	# size
	    @line = split / /,$LINE;	
	  }
	  ##
	  for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
	  	if($i >= $PE2_MAC_ROW_NUM_ORG || $j >= $PE2_MAC_COL_NUM_ORG){
	  		$PE2_DATA_W[$k] = 0;
	  	}else{ 	
	  		if($PE2_W_EXP != 16){ 	$PE2_DATA_W[$k] = int(rand(2**($PE2_W_EXP)-1) - 2**($PE2_W_EXP-1));}
	  		if($PE2_W_EXP == 16){ 	$PE2_DATA_W[$k] = int(rand(2**(13)-1) - 2**(13-1));}
	  		$PE2_DATA_W[$k] = $line[$j];
        if (abs($PE2_DATA_W[$k]) > (2**($PE2_W_EXP) - 1)) {
          print ("weight exceeds fixed point precision range!!\n");
          exit(0);
        }
	  	}
	  	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE2_DATA_W);
	  	$k++;
	  }
  } else {
	  for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
	    $PE2_DATA_W[$j + $i * $PE2_MAC_COL_NUM_ZP] = 0;
    }
  }
}
$k=0;
##
# no need to read line -- still using the same line
## 
for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
	if($j >= $PE2_MAC_COL_NUM_ORG){
		$PE2_DATA_I[$k] = 0;
	}else{
		$PE2_DATA_I[$k] = int(rand(2**($PE2_I_EXP)-1) - 2**($PE2_I_EXP-1));
		$PE2_DATA_I[$k] = $input_line[$j];
    if (abs($PE2_DATA_I[$k]) > (2**($PE2_I_EXP) - 1)) {
      print ("weight exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
##
# no need to read line -- still using the sam line
##
$k=0;
for($j = 0; $j < $PE2_MAC_ROW_NUM_ZP; $j++){
	if($j >= $PE2_MAC_ROW_NUM_ORG || (not $PE2_INST)) {
		$PE2_DATA_OS[$k] = 0;
	}else{
		$PE2_DATA_OS[$k] = int(rand(2**($PE2_O_EXP)-1) - 2**($PE2_O_EXP-1));
		$PE2_DATA_OS[$k] = $offset_line[$j + $MAC_ROW_NUM_ORG * 2]; 
    if (abs($PE2_DATA_OS[$k]) > (2**($PE2_O_EXP) - 1)) {
      print ("offset exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
##########################################
#
# CALCULATE MAC OUTPUTS $PE2_DATA_O
#
$k=0;
printf OUT_LOG "\n";
printf OUT_LOG "DATA_TO TRACKING\n\t";
	for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
		if(int($j % $PE2_I_DATApWORD) eq int($PE2_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t", floor($j/ $PE2_I_DATApWORD));}
	}
	printf OUT_LOG "\n";
for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
	$PE2_DATA_TO[$i] = 0;
	$PE2_DATA_O_NOSH[$i] = 0;
	printf OUT_LOG sprintf("%7d\t",$i);
	for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
		if($j < $PE2_MAC_COL_NUM_ZP - $PE2_I_DATApWORD){
			$PE2_DATA_TO[$i] = $PE2_DATA_TO[$i] + $PE2_DATA_I[$j]*$PE2_DATA_W[$k];
			if(int($j % $PE2_I_DATApWORD) eq int($PE2_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_TO[$i]);}
		}
		$PE2_DATA_O_NOSH[$i] = $PE2_DATA_O_NOSH[$i] + $PE2_DATA_I[$j]*$PE2_DATA_W[$k];
		$k++;
	}
	printf OUT_LOG "\n";
	#print sprintf("(i=%3d, j=%3d) PE2_DATA_TO= %7d\t",$i,$j,$PE2_DATA_TO[$i]);
	#print "\n";
	#print sprintf("At i=%4.4d j=%4.4d: PE2_DATA_O_NOSH= %.d\t = %.d\n",$i,$j,$PE2_DATA_O_NOSH[$i]);

	if($PE2_DATA_O_NOSH[$i] > 2**($PE2_TO_EXP-1)-1) {	$PE2_DATA_O_32B_OV[$i] = int($PE2_DATA_O_NOSH[$i] -  floor($PE2_DATA_O_NOSH[$i]/(2**$PE2_TO_EXP))*(2**$PE2_TO_EXP));}
	elsif($PE2_DATA_O_NOSH[$i] < -2**($PE2_TO_EXP-1))  {	$PE2_DATA_O_32B_OV[$i] = -int($PE2_DATA_O_NOSH[$i] -  floor($PE2_DATA_O_NOSH[$i]/(2**$PE2_TO_EXP))*(2**$PE2_TO_EXP));}
	else {						$PE2_DATA_O_32B_OV[$i] = $PE2_DATA_O_NOSH[$i];}
	
	$PE2_DATA_O_32B_OV[$i] = $PE2_DATA_O_NOSH[$i];
	#$PE2_DATA_O_32B_OV[$i] = int($PE2_DATA_O_NOSH[$i] & 0xFFFFFFFF);
	#$PE2_DATA_O_32B_OV[$i] = int($PE2_DATA_O_NOSH[$i] -  floor($PE2_DATA_O_NOSH[$i]/(2**32))*(2**32));
		
	#$PE2_DATA_O[$i] = floor($PE2_DATA_O_NOSH[$i]/(2**$SHIFT));
	$PE2_DATA_O[$i] = floor($PE2_DATA_O_32B_OV[$i]/(2**$PE2_SHIFT));
	if($PE2_DATA_O[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O[$i] = 2**($PE2_O_EXP-1)-1;}
	if($PE2_DATA_O[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O[$i] = -2**($PE2_O_EXP-1);}

	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE2_DATA_O[$i]);
  ## new 5/24/16
#	if($PE2_MAC_OFFSET){ 
	if($PE2_MAC_OFFSET == 1 && $PE2_MAC_NLI == 0){
  ## new 5/24/16
		$PE2_DATA_O_OS[$i] = floor(($PE2_DATA_O[$i] + $PE2_DATA_OS[$i])/(2**$PE2_SHIFT_OS));
	}else{
		$PE2_DATA_O_OS[$i] = $PE2_DATA_O[$i];
	}
	# NLI FUNCTION IMPLEMENTATION	
	#
	if($PE2_MAC_NLI){
		$PE2_I_SELECTED = -1;
		for($q = 0; $q < $NLI_SEGMENT; $q++){
			#if($DATA_O_OS[$i] >= $DATA_X[$q] && $DATA_O_OS[$i] < $DATA_X[$q+1]){
			if($PE2_DATA_O_OS[$i] >= $PE2_DATA_X[$q] && $PE2_DATA_O_OS[$i] <= $PE2_DATA_X[$q+1]){
				$PE2_I_SELECTED = $q;
			}
		}
		if($PE2_I_SELECTED == -1) {
			$A_SEL = 0;
			$B_SEL = 0;
		}else{
			$A_SEL = $PE2_DATA_A[$PE2_I_SELECTED];
			$B_SEL = $PE2_DATA_B[$PE2_I_SELECTED];
		}
    #########
    ## caogao
		$PE2_DATA_O_NLI[$i] = int(($PE2_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE2_SHIFT_NL));
		$PE2_DATA_O_NLI[$i] = floor(($PE2_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE2_SHIFT_NL));
    ######

		if($PE2_DATA_O_NLI[$i] > 2**($PE2_O_EXP-1)-1) {		$PE2_DATA_O_NLI[$i] = 2**($PE2_O_EXP-1)-1;}
		if($PE2_DATA_O_NLI[$i] < -2**($PE2_O_EXP-1))  {		$PE2_DATA_O_NLI[$i] = -2**($PE2_O_EXP-1);}

		$LINE_DBG	= sprintf("PE2_X = %4.4x  ", $PE2_DATA_O_OS[$i] & 0xFFFF);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("%+6d ", $PE2_DATA_O_OS[$i]);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE2_I_SELECTED = %2d ", $PE2_I_SELECTED);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE2_A_SELECTED = %+6d ", $A_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE2_B_SELECTED = %+12d ", $B_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tA*X+B (dec) = %+12d ", $PE2_DATA_O_OS[$i]*$A_SEL+$B_SEL);	
		#printf OUT_LOG sprintf("A*X+B (hex) = %8.8x  ", ($DATA_O_OS[$i]*$A_SEL[]+$B_SEL[]) & 0xFFFFFFFF);
		#printf OUT_LOG sprintf("(A*X+B)/2^SHIFT_NL (dec) = %+d  ", $DATA_O_NLI[$i]);					
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\t(A*X+B)/2^SHIFT_NL (hex) = %4.4x\n", $PE2_DATA_O_NLI[$i] & 0xFFFF);			
		$LINES_DBG[$IDX_DBG] = "$LINE_DBG";
		$IDX_DBG++;

	}else{
		$PE2_DATA_O_NLI[$i] = $PE2_DATA_O_OS[$i];
	}
	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE2_DATA_O[$i]);

  ## new 5/25/16
  if ($PE2_O_SIGN == 0 && $PE2_DATA_O_NLI[$i] < 0) {
		$PE2_DATA_O_NLI[$i] = 0;
  }
  ## new 5/25/16
}
#
if (1){
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_W\t";
	for($i = 0; $i < $PE2_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n";
	$k = 0;
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$i);
		for($j = 0; $j < $PE2_MAC_COL_NUM_ZP; $j++){
			printf OUT_LOG sprintf("%7d\t",$PE2_DATA_W[$k]);
			$k++;
		}
		printf OUT_LOG "\n";
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_I\t";
	for($i = 0; $i < $PE2_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_COL_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_I[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_OS\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_OS[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_TO\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_TO[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noSH)\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_O_NOSH[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		#printf OUT_LOG sprintf("%3.3d= %12.12x\t",$i,$PE2_DATA_O_NOSH[$i]&0xFFFFFFFFFFFF);
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE2_DATA_O_NOSH[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(32bOV)\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_O_32B_OV[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE2_DATA_O_32B_OV[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noOS)\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_O[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE2_DATA_O[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/OS)\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_O_OS[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE2_DATA_O_OS[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/NLI)\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE2_DATA_O_NLI[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE2_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE2_DATA_O_NLI[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "\n";
}
#
#
# GENERATE RANDOM DATA OF I, W, OS FOR MAC
# PE3
#
#
my @PE3_DATA_I 		= ();
my @PE3_DATA_W 		= ();
my @PE3_DATA_O_NOSH	= ();
my @PE3_DATA_O_32B_OV	= ();
my @PE3_DATA_O		= ();
my @PE3_DATA_O_OS	= ();
my @PE3_DATA_O_NLI	= ();
my @PE3_DATA_OS		= ();
my @PE3_DATA_TO		= ();
my @PE3_LINE_O 		= ();
my @PE3_LINE_TO 	= ();
#
# MAC DATA SUMMARY
#
printf OUT_LOG "\n";
printf OUT_LOG "PE3 ===============\n";
printf OUT_LOG sprintf("MAC_ROW_NUM_ZP = %d\t", $PE3_MAC_ROW_NUM_ZP);
printf OUT_LOG sprintf("MAC_ROW_NUM_ORG = %d\n", $PE3_MAC_ROW_NUM_ORG);
printf OUT_LOG sprintf("MAC_COL_NUM_ZP = %d\t", $PE3_MAC_COL_NUM_ZP);
printf OUT_LOG sprintf("MAC_COL_NUM_ORG = %d\n", $PE3_MAC_COL_NUM_ORG);
printf OUT_LOG sprintf("MAC_W_COL_NUM = %d\n", $PE3_MAC_W_COL_NUM);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("SHIFT    = %d\n", $PE3_SHIFT);
printf OUT_LOG sprintf("SHIFT_OS = %d\n", $PE3_SHIFT_OS);
printf OUT_LOG sprintf("SHIFT_NL = %d\n", $PE3_SHIFT_NL);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL I WITH ZERO-PADDING = %d\n", $PE3_I_MEM_SIZE*$PE3_I_DATApWORD);
printf OUT_LOG sprintf("TOTAL O WITH ZERO-PADDING = %d\n", $PE3_O_MEM_SIZE*$PE3_O_DATApWORD);
printf OUT_LOG "\n";
printf OUT_LOG sprintf("TOTAL WORD SIZE OF I  WITH ZERO-PADDING = %d\n", $PE3_I_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF O  WITH ZERO-PADDING = %d\n", $PE3_O_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF W  WITH ZERO-PADDING = %d\n", $PE3_W_MEM_SIZE);
printf OUT_LOG sprintf("TOTAL WORD SIZE OF TO WITH ZERO-PADDING = %d\n", $PE3_TO_MEM_SIZE);
printf OUT_LOG "\n";
if ($PE3_W_PREC == $PREC_06B) 	{printf OUT_LOG "W_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE3_W_PREC == $PREC_08B) 	{printf OUT_LOG "W_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE3_W_PREC == $PREC_12B) 	{printf OUT_LOG "W_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE3_W_PREC == $PREC_16B) 	{printf OUT_LOG "W_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE3_I_PREC == $PREC_06B) 	{printf OUT_LOG "I_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE3_I_PREC == $PREC_08B) 	{printf OUT_LOG "I_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE3_I_PREC == $PREC_12B) 	{printf OUT_LOG "I_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE3_I_PREC == $PREC_16B) 	{printf OUT_LOG "I_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE3_O_PREC == $PREC_06B) 	{printf OUT_LOG "O_PREC = 06B\t";	printf OUT_LOG "DATApWORD = 16\n";}
if ($PE3_O_PREC == $PREC_08B) 	{printf OUT_LOG "O_PREC = 08B\t";	printf OUT_LOG "DATApWORD = 12\n";}
if ($PE3_O_PREC == $PREC_12B) 	{printf OUT_LOG "O_PREC = 12B\t";	printf OUT_LOG "DATApWORD = 08\n";}
if ($PE3_O_PREC == $PREC_16B) 	{printf OUT_LOG "O_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE3_TO_PREC == $PREC_16B) 	{printf OUT_LOG "TO_PREC = 16B\t";	printf OUT_LOG "DATApWORD = 06\n";}
if ($PE3_TO_PREC == $PREC_24B) 	{printf OUT_LOG "TO_PREC = 24B\t";	printf OUT_LOG "DATApWORD = 04\n";}
if ($PE3_TO_PREC == $PREC_32B) 	{printf OUT_LOG "TO_PREC = 32B\t";	printf OUT_LOG "DATApWORD = 03\n";}
printf OUT_LOG "\n";
#
if ($PE3_I_PREC == $PREC_06B) 	{print "I_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE3_I_PREC == $PREC_08B) 	{print "I_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE3_I_PREC == $PREC_12B) 	{print "I_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE3_I_PREC == $PREC_16B) 	{print "I_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE3_W_PREC == $PREC_06B) 	{print "W_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE3_W_PREC == $PREC_08B) 	{print "W_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE3_W_PREC == $PREC_12B) 	{print "W_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE3_W_PREC == $PREC_16B) 	{print "W_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE3_O_PREC == $PREC_06B) 	{print "O_PREC = 06B\t";	print "DATApWORD = 16\n";}
if ($PE3_O_PREC == $PREC_08B) 	{print "O_PREC = 08B\t";	print "DATApWORD = 12\n";}
if ($PE3_O_PREC == $PREC_12B) 	{print "O_PREC = 12B\t";	print "DATApWORD = 08\n";}
if ($PE3_O_PREC == $PREC_16B) 	{print "O_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE3_TO_PREC == $PREC_16B) 	{print "TO_PREC = 16B\t";	print "DATApWORD = 06\n";}
if ($PE3_TO_PREC == $PREC_24B) 	{print "TO_PREC = 24B\t";	print "DATApWORD = 04\n";}
if ($PE3_TO_PREC == $PREC_32B) 	{print "TO_PREC = 32B\t";	print "DATApWORD = 03\n";}
print "\n";
#
# MAC DATA RANDOM GENERATION & CALCULATION
#
##########################################
# caogao
$k=0;
for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
	##
  if ($PE3_INST) {
	  if ($i < $PE3_MAC_ROW_NUM_ORG) {
	    $LINE = <$weight_file>;
	    @line = split / /,$LINE;
	  }
	  ##
	  for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
	  	if($i >= $PE3_MAC_ROW_NUM_ORG || $j >= $PE3_MAC_COL_NUM_ORG){
	  		$PE3_DATA_W[$k] = 0;
	  	}else{ 	
	  		if($PE3_W_EXP != 16){ 	$PE3_DATA_W[$k] = int(rand(2**($PE3_W_EXP)-1) - 2**($PE3_W_EXP-1));}
	  		if($PE3_W_EXP == 16){ 	$PE3_DATA_W[$k] = int(rand(2**(13)-1) - 2**(13-1));}
	  		$PE3_DATA_W[$k] = $line[$j];
        if (abs($PE3_DATA_W[$k]) > (2**($PE3_W_EXP) - 1)) {
          print ("weight exceeds fixed point precision range!!\n");
          exit(0);
        }
	  	}
	  	#print sprintf("At i= %d: %.2x %.2x\n", $i, $PE3_DATA_W_RE[$i]&0xFF, $PE3_DATA_W_IM[$i]&0xFF);
	  	$k++;
	  }
  } else {
	  for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
	    $PE3_DATA_W[$j + $i * $PE3_MAC_COL_NUM_ZP] = 0;
    }
  }
}
## 
# no need to read line -- still using the same line
##
$k=0;
for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
	if($j >= $PE3_MAC_COL_NUM_ORG){
		$PE3_DATA_I[$k] = 0;
	}else{
		$PE3_DATA_I[$k] = int(rand(2**($PE3_I_EXP)-1) - 2**($PE3_I_EXP-1));
		$PE3_DATA_I[$k] = $input_line[$j];
    if (abs($PE3_DATA_I[$k]) > (2**($PE3_I_EXP) - 1)) {
      print ("input exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
## 
# no need to read line -- still using the same line
##
$k=0;
for($j = 0; $j < $PE3_MAC_ROW_NUM_ZP; $j++){
	if($j >= $PE3_MAC_ROW_NUM_ORG || (not $PE3_INST)) {
		$PE3_DATA_OS[$k] = 0;
	}else{
		$PE3_DATA_OS[$k] = int(rand(2**($PE3_O_EXP)-1) - 2**($PE3_O_EXP-1));
		$PE3_DATA_OS[$k] = $offset_line[$j + $MAC_ROW_NUM_ORG * 3];
    if (abs($PE3_DATA_OS[$k]) > (2**($PE3_O_EXP) - 1)) {
      print ("offset exceeds fixed point precision range!!\n");
      exit(0);
    }
	}
	$k++;
}
##
close($weight_file);
close($input_file);
close($offset_file);
##

##########################################
#
# CALCULATE MAC OUTPUTS $PE3_DATA_O
#
$k=0;
printf OUT_LOG "\n";
printf OUT_LOG "DATA_TO TRACKING\n\t";
	for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
		if(int($j % $PE3_I_DATApWORD) eq int($PE3_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t", floor($j/ $PE3_I_DATApWORD));}
	}
	printf OUT_LOG "\n";
for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
	$PE3_DATA_TO[$i] = 0;
	$PE3_DATA_O_NOSH[$i] = 0;
	printf OUT_LOG sprintf("%7d\t",$i);
	for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
		if($j < $PE3_MAC_COL_NUM_ZP - $PE3_I_DATApWORD){
			$PE3_DATA_TO[$i] = $PE3_DATA_TO[$i] + $PE3_DATA_I[$j]*$PE3_DATA_W[$k];
			if(int($j % $PE3_I_DATApWORD) eq int($PE3_I_DATApWORD-1)){		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_TO[$i]);}
		}
		$PE3_DATA_O_NOSH[$i] = $PE3_DATA_O_NOSH[$i] + $PE3_DATA_I[$j]*$PE3_DATA_W[$k];
		$k++;
	}
	printf OUT_LOG "\n";
	#print sprintf("(i=%3d, j=%3d) PE3_DATA_TO= %7d\t",$i,$j,$PE3_DATA_TO[$i]);
	#print "\n";
	#print sprintf("At i=%4.4d j=%4.4d: PE3_DATA_O_NOSH= %.d\t = %.d\n",$i,$j,$PE3_DATA_O_NOSH[$i]);

	if($PE3_DATA_O_NOSH[$i] > 2**($PE3_TO_EXP-1)-1) {	$PE3_DATA_O_32B_OV[$i] = int($PE3_DATA_O_NOSH[$i] -  floor($PE3_DATA_O_NOSH[$i]/(2**$PE3_TO_EXP))*(2**$PE3_TO_EXP));}
	elsif($PE3_DATA_O_NOSH[$i] < -2**($PE3_TO_EXP-1))  {	$PE3_DATA_O_32B_OV[$i] = -int($PE3_DATA_O_NOSH[$i] -  floor($PE3_DATA_O_NOSH[$i]/(2**$PE3_TO_EXP))*(2**$PE3_TO_EXP));}
	else {						$PE3_DATA_O_32B_OV[$i] = $PE3_DATA_O_NOSH[$i];}
	
	$PE3_DATA_O_32B_OV[$i] = $PE3_DATA_O_NOSH[$i];
	#$PE3_DATA_O_32B_OV[$i] = int($PE3_DATA_O_NOSH[$i] & 0xFFFFFFFF);
	#$PE3_DATA_O_32B_OV[$i] = int($PE3_DATA_O_NOSH[$i] -  floor($PE3_DATA_O_NOSH[$i]/(2**32))*(2**32));
		
	#$PE3_DATA_O[$i] = floor($PE3_DATA_O_NOSH[$i]/(2**$SHIFT));
	$PE3_DATA_O[$i] = floor($PE3_DATA_O_32B_OV[$i]/(2**$PE3_SHIFT));
	if($PE3_DATA_O[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O[$i] = 2**($PE3_O_EXP-1)-1;}
	if($PE3_DATA_O[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O[$i] = -2**($PE3_O_EXP-1);}

	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE3_DATA_O[$i]);
  # new 5/24/16
#	if($PE3_MAC_OFFSET){ 
	if($PE3_MAC_OFFSET == 1 && $PE3_MAC_NLI == 0){
  # new 5/24/16
		$PE3_DATA_O_OS[$i] = floor(($PE3_DATA_O[$i] + $PE3_DATA_OS[$i])/(2**$PE3_SHIFT_OS));
	}else{
		$PE3_DATA_O_OS[$i] = $PE3_DATA_O[$i];
	}

	# NLI FUNCTION IMPLEMENTATION	
	#
	if($PE3_MAC_NLI){
		$PE3_I_SELECTED = -1;
		for($q = 0; $q < $NLI_SEGMENT; $q++){
			#if($DATA_O_OS[$i] >= $DATA_X[$q] && $DATA_O_OS[$i] < $DATA_X[$q+1]){
			if($PE3_DATA_O_OS[$i] >= $PE3_DATA_X[$q] && $PE3_DATA_O_OS[$i] <= $PE3_DATA_X[$q+1]){
				$PE3_I_SELECTED = $q;
			}
		}
		if($PE3_I_SELECTED == -1) {
			$A_SEL = 0;
			$B_SEL = 0;
		}else{
			$A_SEL = $PE3_DATA_A[$PE3_I_SELECTED];
			$B_SEL = $PE3_DATA_B[$PE3_I_SELECTED];
		}
    ###########
    ### caogao
		$PE3_DATA_O_NLI[$i] = int(($PE3_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE3_SHIFT_NL));
		$PE3_DATA_O_NLI[$i] = floor(($PE3_DATA_O_OS[$i]*$A_SEL+$B_SEL)/(2**$PE3_SHIFT_NL));
    ### 

		if($PE3_DATA_O_NLI[$i] > 2**($PE3_O_EXP-1)-1) {		$PE3_DATA_O_NLI[$i] = 2**($PE3_O_EXP-1)-1;}
		if($PE3_DATA_O_NLI[$i] < -2**($PE3_O_EXP-1))  {		$PE3_DATA_O_NLI[$i] = -2**($PE3_O_EXP-1);}

		$LINE_DBG	= sprintf("PE3_X = %4.4x  ", $PE3_DATA_O_OS[$i] & 0xFFFF);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("%+6d ", $PE3_DATA_O_OS[$i]);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE3_I_SELECTED = %2d ", $PE3_I_SELECTED);							
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE3_A_SELECTED = %+6d ", $A_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tPE3_B_SELECTED = %+12d ", $B_SEL);						
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\tA*X+B (dec) = %+12d ", $PE3_DATA_O_OS[$i]*$A_SEL+$B_SEL);	
		#printf OUT_LOG sprintf("A*X+B (hex) = %8.8x  ", ($DATA_O_OS[$i]*$A_SEL[]+$B_SEL[]) & 0xFFFFFFFF);
		#printf OUT_LOG sprintf("(A*X+B)/2^SHIFT_NL (dec) = %+d  ", $DATA_O_NLI[$i]);					
		$LINE_DBG	= join "", $LINE_DBG, sprintf("\t(A*X+B)/2^SHIFT_NL (hex) = %4.4x\n", $PE3_DATA_O_NLI[$i] & 0xFFFF);			
		$LINES_DBG[$IDX_DBG] = "$LINE_DBG";
		$IDX_DBG++;

	}else{
		$PE3_DATA_O_NLI[$i] = $PE3_DATA_O_OS[$i];
	}
	#print sprintf("At i=%4.4d j=%4.4d: DATA_O= %.d\t = %.d\n",$i,$j,$PE3_DATA_O[$i]);

  ## new 5/25/16
  if ($PE3_O_SIGN == 0 && $PE3_DATA_O_NLI[$i] < 0) {
		$PE3_DATA_O_NLI[$i] = 0;
  }
  ## new 5/25/16
}
#
if (1){
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_W\t";
	for($i = 0; $i < $PE3_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n";
	$k = 0;
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$i);
		for($j = 0; $j < $PE3_MAC_COL_NUM_ZP; $j++){
			printf OUT_LOG sprintf("%7d\t",$PE3_DATA_W[$k]);
			$k++;
		}
		printf OUT_LOG "\n";
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_I\t";
	for($i = 0; $i < $PE3_MAC_COL_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_COL_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_I[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_OS\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_OS[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "DATA_TO\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_TO[$i]);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noSH)\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_O_NOSH[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		#printf OUT_LOG sprintf("%3.3d= %12.12x\t",$i,$PE3_DATA_O_NOSH[$i]&0xFFFFFFFFFFFF);
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE3_DATA_O_NOSH[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(32bOV)\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_O_32B_OV[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%3.3d= %8.8x\t",$i,$PE3_DATA_O_32B_OV[$i]&0xFFFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(noOS)\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_O[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE3_DATA_O[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/OS)\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_O_OS[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE3_DATA_O_OS[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "O(w/NLI)\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){
		printf OUT_LOG sprintf("%7d\t",$i);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%7d\t",$PE3_DATA_O_NLI[$i]);
	}
	printf OUT_LOG "\n\t";
	for($i = 0; $i < $PE3_MAC_ROW_NUM_ZP; $i++){	
		printf OUT_LOG sprintf("%.6x\t",$PE3_DATA_O_NLI[$i]&0xFFFFFF);
	}
	printf OUT_LOG "\n";
	printf OUT_LOG "\n";
}

#
#
# MEM_* INITIALIZATION
#
my $FILE;
if (0) {    ## not using this feature here
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
my $idx1 = 0;
my $idx2 = 0;
#
# MAC DATA PACKING INTO 96B/WORD
# PE0
#
$k = 0;
$i = 0;
$j = 0;
$p = 0;
$q = 0;
$idx1 = 0;
$idx2 = 0;
$LINE_MEM = ""; 	
printf OUT_LOG "\n";
printf OUT_LOG "W_INDEX INFO IN STORING ORDER: W_INDEX = CURRENT_ROW_INDEX x TOT_COL_NUM + INDEX_OF_COL_IN_CURRENT_ROW\n";
printf OUT_LOG "\n";
while ($k < $PE0_W_MEM_SIZE){
	$idx1 = $j*$PE0_MAC_COL_NUM_ZP + $i*$PE0_I_DATApWORD + $p*2;
	$idx2 = $idx1 + 1;
	printf OUT_LOG sprintf("%4d\t%4d\t",$idx1,$idx2);
	if($PE0_W_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_W[$idx2]*(2**6))&0xFC0 | $PE0_DATA_W[$idx1]&0x3F), $LINE_MEM; }
	if($PE0_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_W[$idx2]&0xFF  ), sprintf("%2.2x",$PE0_DATA_W[$idx1]&0xFF  ), $LINE_MEM;}
	if($PE0_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_W[$idx2]&0xFFF ), sprintf("%3.3x",$PE0_DATA_W[$idx1]&0xFFF ), $LINE_MEM;}
	if($PE0_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_W[$idx2]&0xFFFF), sprintf("%4.4x",$PE0_DATA_W[$idx1]&0xFFFF), $LINE_MEM;}
	#printf OUT_LOG "$k\t$LINE_MEM\n";
	$q++;
	if($q == $PE0_W_DATApWORD/2){	
		$q = 0; 	
		if ($PE0_INST && $PE0_W == 0){		$PE0_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_W == 1){		$PE1_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_W == 2){		$PE2_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE0_INST && $PE0_W == 3){		$PE3_MEM_LINES[$k + $PE0_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
		$LINE_MEM = ""; 	
		printf OUT_LOG "\n";
	}
	$p++;
	if($p == $PE0_I_DATApWORD/2){	$p = 0;		$j++; }
	if($j == $PE0_MAC_ROW_NUM_ZP){	$j = 0;		$i++; 	printf OUT_LOG "\n";}
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $MAC_COL_NUM_ZP){
while($k < $PE0_I_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE0_I_DATApWORD/2; $q++){	
		#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_I0_IM[$i]&0x3F,$PE0_DATA_I0_RE[$i]&0x3F);
		if($PE0_I_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_I[$i+1]*(2**6))&0xFC0 | $PE0_DATA_I[$i]&0x3F), $LINE_MEM; }
		if($PE0_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_I[$i+1]&0xFF  ), sprintf("%2.2x",$PE0_DATA_I[$i]&0xFF  ), $LINE_MEM;}
		if($PE0_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_I[$i+1]&0xFFF ), sprintf("%3.3x",$PE0_DATA_I[$i]&0xFFF ), $LINE_MEM;}
		if($PE0_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_I[$i+1]&0xFFFF), sprintf("%4.4x",$PE0_DATA_I[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE0_INST && $PE0_I == 0){		$PE0_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_I == 1){		$PE1_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_I == 2){		$PE2_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_I == 3){		$PE3_MEM_LINES[$k + $PE0_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
# NOTE THAT 
# $OS_MEM_SIZE = $O_MEM_SIZE
# $OS_PREC = $PE0_O_PREC
#
#while($i < $PE0_MAC_ROW_NUM_ZP){
while($k < $PE0_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE0_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_O0_IM[$i]&0x3F,$PE0_DATA_O0_RE[$i]&0x3F);
		if($PE0_O_PREC==$PREC_06B){	$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_OS[$i+1]*(2**6))&0xFC0 | $PE0_DATA_OS[$i]&0x3F), $LINE_MEM; }
		if($PE0_O_PREC==$PREC_08B){	$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_OS[$i+1]&0xFF  ), sprintf("%2.2x",$PE0_DATA_OS[$i]&0xFF  ), $LINE_MEM;}
		if($PE0_O_PREC==$PREC_12B){	$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_OS[$i+1]&0xFFF ), sprintf("%3.3x",$PE0_DATA_OS[$i]&0xFFF ), $LINE_MEM;}
		if($PE0_O_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_OS[$i+1]&0xFFFF), sprintf("%4.4x",$PE0_DATA_OS[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE0_INST && $PE0_OS == 0){		$PE0_MEM_LINES[$k + $PE0_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_OS == 1){		$PE1_MEM_LINES[$k + $PE0_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_OS == 2){		$PE2_MEM_LINES[$k + $PE0_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE0_INST && $PE0_OS == 3){		$PE3_MEM_LINES[$k + $PE0_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE0_MAC_ROW_NUM_ZP){
while($k < $PE0_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE0_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_O0_IM[$i]&0x3F,$PE0_DATA_O0_RE[$i]&0x3F);
		if($PE0_O_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE0_DATA_O_NLI[$i+1]*(2**6))&0xFC0 | $PE0_DATA_O_NLI[$i]&0x3F), $LINE_MEM; }
		if($PE0_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE0_DATA_O_NLI[$i+1]&0xFF  ), sprintf("%2.2x",$PE0_DATA_O_NLI[$i]&0xFF  ), $LINE_MEM;}
		if($PE0_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE0_DATA_O_NLI[$i+1]&0xFFF ), sprintf("%3.3x",$PE0_DATA_O_NLI[$i]&0xFFF ), $LINE_MEM;}
		if($PE0_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_O_NLI[$i+1]&0xFFFF), sprintf("%4.4x",$PE0_DATA_O_NLI[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE0_LINE_O[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE0_MAC_ROW_NUM_ZP){
while($k < $PE0_TO_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE0_TO_DATApWORD; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE0_DATA_O0_IM[$i]&0x3F,$PE0_DATA_O0_RE[$i]&0x3F);
		if($PE0_TO_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE0_DATA_TO[$i]&0xFFFF), $LINE_MEM;}
		if($PE0_TO_PREC==$PREC_24B){	$LINE_MEM = join "", sprintf("%6.6x",$PE0_DATA_TO[$i]&0xFFFFFF), $LINE_MEM;}
		if($PE0_TO_PREC==$PREC_32B){	$LINE_MEM = join "", sprintf("%8.8x",$PE0_DATA_TO[$i]&0xFFFFFFFF), $LINE_MEM;}
		$i = $i + 1;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE0_LINE_TO[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
#
# MAC DATA PACKING INTO 96B/WORD
# PE1
#
$k = 0;
$i = 0;
$j = 0;
$p = 0;
$q = 0;
$idx1 = 0;
$idx2 = 0;
$LINE_MEM = ""; 	
printf OUT_LOG "\n";
printf OUT_LOG "W_INDEX INFO IN STORING ORDER: W_INDEX = CURRENT_ROW_INDEX x TOT_COL_NUM + INDEX_OF_COL_IN_CURRENT_ROW\n";
printf OUT_LOG "\n";
while ($k < $PE1_W_MEM_SIZE){
	$idx1 = $j*$PE1_MAC_COL_NUM_ZP + $i*$PE1_I_DATApWORD + $p*2;
	$idx2 = $idx1 + 1;
	printf OUT_LOG sprintf("%4d\t%4d\t",$idx1,$idx2);
	if($PE1_W_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_W[$idx2]*(2**6))&0xFC0 | $PE1_DATA_W[$idx1]&0x3F), $LINE_MEM; }
	if($PE1_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_W[$idx2]&0xFF  ), sprintf("%2.2x",$PE1_DATA_W[$idx1]&0xFF  ), $LINE_MEM;}
	if($PE1_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_W[$idx2]&0xFFF ), sprintf("%3.3x",$PE1_DATA_W[$idx1]&0xFFF ), $LINE_MEM;}
	if($PE1_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_W[$idx2]&0xFFFF), sprintf("%4.4x",$PE1_DATA_W[$idx1]&0xFFFF), $LINE_MEM;}
	#printf OUT_LOG "$k\t$LINE_MEM\n";
	$q++;
	if($q == $PE1_W_DATApWORD/2){	
		$q = 0; 	
		if ($PE1_INST && $PE1_W == 0){		$PE0_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_W == 1){		$PE1_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_W == 2){		$PE2_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE1_INST && $PE1_W == 3){		$PE3_MEM_LINES[$k + $PE1_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
		$LINE_MEM = ""; 	
		printf OUT_LOG "\n";
	}
	$p++;
	if($p == $PE1_I_DATApWORD/2){	$p = 0;		$j++; }
	if($j == $PE1_MAC_ROW_NUM_ZP){	$j = 0;		$i++; 	printf OUT_LOG "\n";}
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $MAC_COL_NUM_ZP){
while($k < $PE1_I_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE1_I_DATApWORD/2; $q++){	
		#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_I0_IM[$i]&0x3F,$PE1_DATA_I0_RE[$i]&0x3F);
		if($PE1_I_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_I[$i+1]*(2**6))&0xFC0 | $PE1_DATA_I[$i]&0x3F), $LINE_MEM; }
		if($PE1_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_I[$i+1]&0xFF  ), sprintf("%2.2x",$PE1_DATA_I[$i]&0xFF  ), $LINE_MEM;}
		if($PE1_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_I[$i+1]&0xFFF ), sprintf("%3.3x",$PE1_DATA_I[$i]&0xFFF ), $LINE_MEM;}
		if($PE1_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_I[$i+1]&0xFFFF), sprintf("%4.4x",$PE1_DATA_I[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE1_INST && $PE1_I == 0){		$PE0_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_I == 1){		$PE1_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_I == 2){		$PE2_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_I == 3){		$PE3_MEM_LINES[$k + $PE1_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
# NOTE THAT 
# $OS_MEM_SIZE = $O_MEM_SIZE
# $OS_PREC = $PE1_O_PREC
#
#while($i < $PE1_MAC_ROW_NUM_ZP){
while($k < $PE1_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE1_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_O0_IM[$i]&0x3F,$PE1_DATA_O0_RE[$i]&0x3F);
		if($PE1_O_PREC==$PREC_06B){	$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_OS[$i+1]*(2**6))&0xFC0 | $PE1_DATA_OS[$i]&0x3F), $LINE_MEM; }
		if($PE1_O_PREC==$PREC_08B){	$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_OS[$i+1]&0xFF  ), sprintf("%2.2x",$PE1_DATA_OS[$i]&0xFF  ), $LINE_MEM;}
		if($PE1_O_PREC==$PREC_12B){	$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_OS[$i+1]&0xFFF ), sprintf("%3.3x",$PE1_DATA_OS[$i]&0xFFF ), $LINE_MEM;}
		if($PE1_O_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_OS[$i+1]&0xFFFF), sprintf("%4.4x",$PE1_DATA_OS[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE1_INST && $PE1_OS == 0){		$PE0_MEM_LINES[$k + $PE1_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_OS == 1){		$PE1_MEM_LINES[$k + $PE1_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_OS == 2){		$PE2_MEM_LINES[$k + $PE1_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE1_INST && $PE1_OS == 3){		$PE3_MEM_LINES[$k + $PE1_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE1_MAC_ROW_NUM_ZP){
while($k < $PE1_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE1_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_O0_IM[$i]&0x3F,$PE1_DATA_O0_RE[$i]&0x3F);
		if($PE1_O_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE1_DATA_O_NLI[$i+1]*(2**6))&0xFC0 | $PE1_DATA_O_NLI[$i]&0x3F), $LINE_MEM; }
		if($PE1_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE1_DATA_O_NLI[$i+1]&0xFF  ), sprintf("%2.2x",$PE1_DATA_O_NLI[$i]&0xFF  ), $LINE_MEM;}
		if($PE1_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE1_DATA_O_NLI[$i+1]&0xFFF ), sprintf("%3.3x",$PE1_DATA_O_NLI[$i]&0xFFF ), $LINE_MEM;}
		if($PE1_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_O_NLI[$i+1]&0xFFFF), sprintf("%4.4x",$PE1_DATA_O_NLI[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE1_LINE_O[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE1_MAC_ROW_NUM_ZP){
while($k < $PE1_TO_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE1_TO_DATApWORD; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE1_DATA_O0_IM[$i]&0x3F,$PE1_DATA_O0_RE[$i]&0x3F);
		if($PE1_TO_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE1_DATA_TO[$i]&0xFFFF), $LINE_MEM;}
		if($PE1_TO_PREC==$PREC_24B){	$LINE_MEM = join "", sprintf("%6.6x",$PE1_DATA_TO[$i]&0xFFFFFF), $LINE_MEM;}
		if($PE1_TO_PREC==$PREC_32B){	$LINE_MEM = join "", sprintf("%8.8x",$PE1_DATA_TO[$i]&0xFFFFFFFF), $LINE_MEM;}
		$i = $i + 1;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE1_LINE_TO[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
#
# MAC DATA PACKING INTO 96B/WORD
# PE2
#
$k = 0;
$i = 0;
$j = 0;
$p = 0;
$q = 0;
$idx1 = 0;
$idx2 = 0;
$LINE_MEM = ""; 	
printf OUT_LOG "\n";
printf OUT_LOG "W_INDEX INFO IN STORING ORDER: W_INDEX = CURRENT_ROW_INDEX x TOT_COL_NUM + INDEX_OF_COL_IN_CURRENT_ROW\n";
printf OUT_LOG "\n";
while ($k < $PE2_W_MEM_SIZE){
	$idx1 = $j*$PE2_MAC_COL_NUM_ZP + $i*$PE2_I_DATApWORD + $p*2;
	$idx2 = $idx1 + 1;
	printf OUT_LOG sprintf("%4d\t%4d\t",$idx1,$idx2);
	if($PE2_W_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_W[$idx2]*(2**6))&0xFC0 | $PE2_DATA_W[$idx1]&0x3F), $LINE_MEM; }
	if($PE2_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_W[$idx2]&0xFF  ), sprintf("%2.2x",$PE2_DATA_W[$idx1]&0xFF  ), $LINE_MEM;}
	if($PE2_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_W[$idx2]&0xFFF ), sprintf("%3.3x",$PE2_DATA_W[$idx1]&0xFFF ), $LINE_MEM;}
	if($PE2_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_W[$idx2]&0xFFFF), sprintf("%4.4x",$PE2_DATA_W[$idx1]&0xFFFF), $LINE_MEM;}
	#printf OUT_LOG "$k\t$LINE_MEM\n";
	$q++;
	if($q == $PE2_W_DATApWORD/2){	
		$q = 0; 	
		if ($PE2_INST && $PE2_W == 0){		$PE0_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_W == 1){		$PE1_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_W == 2){		$PE2_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE2_INST && $PE2_W == 3){		$PE3_MEM_LINES[$k + $PE2_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
		$LINE_MEM = ""; 	
		printf OUT_LOG "\n";
	}
	$p++;
	if($p == $PE2_I_DATApWORD/2){	$p = 0;		$j++; }
	if($j == $PE2_MAC_ROW_NUM_ZP){	$j = 0;		$i++; 	printf OUT_LOG "\n";}
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $MAC_COL_NUM_ZP){
while($k < $PE2_I_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE2_I_DATApWORD/2; $q++){	
		#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_I0_IM[$i]&0x3F,$PE2_DATA_I0_RE[$i]&0x3F);
		if($PE2_I_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_I[$i+1]*(2**6))&0xFC0 | $PE2_DATA_I[$i]&0x3F), $LINE_MEM; }
		if($PE2_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_I[$i+1]&0xFF  ), sprintf("%2.2x",$PE2_DATA_I[$i]&0xFF  ), $LINE_MEM;}
		if($PE2_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_I[$i+1]&0xFFF ), sprintf("%3.3x",$PE2_DATA_I[$i]&0xFFF ), $LINE_MEM;}
		if($PE2_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_I[$i+1]&0xFFFF), sprintf("%4.4x",$PE2_DATA_I[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE2_INST && $PE2_I == 0){		$PE0_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_I == 1){		$PE1_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_I == 2){		$PE2_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_I == 3){		$PE3_MEM_LINES[$k + $PE2_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
# NOTE THAT 
# $OS_MEM_SIZE = $O_MEM_SIZE
# $OS_PREC = $PE2_O_PREC
#
#while($i < $PE2_MAC_ROW_NUM_ZP){
while($k < $PE2_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE2_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_O0_IM[$i]&0x3F,$PE2_DATA_O0_RE[$i]&0x3F);
		if($PE2_O_PREC==$PREC_06B){	$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_OS[$i+1]*(2**6))&0xFC0 | $PE2_DATA_OS[$i]&0x3F), $LINE_MEM; }
		if($PE2_O_PREC==$PREC_08B){	$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_OS[$i+1]&0xFF  ), sprintf("%2.2x",$PE2_DATA_OS[$i]&0xFF  ), $LINE_MEM;}
		if($PE2_O_PREC==$PREC_12B){	$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_OS[$i+1]&0xFFF ), sprintf("%3.3x",$PE2_DATA_OS[$i]&0xFFF ), $LINE_MEM;}
		if($PE2_O_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_OS[$i+1]&0xFFFF), sprintf("%4.4x",$PE2_DATA_OS[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE2_INST && $PE2_OS == 0){		$PE0_MEM_LINES[$k + $PE2_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_OS == 1){		$PE1_MEM_LINES[$k + $PE2_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_OS == 2){		$PE2_MEM_LINES[$k + $PE2_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE2_INST && $PE2_OS == 3){		$PE3_MEM_LINES[$k + $PE2_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE2_MAC_ROW_NUM_ZP){
while($k < $PE2_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE2_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_O0_IM[$i]&0x3F,$PE2_DATA_O0_RE[$i]&0x3F);
		if($PE2_O_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE2_DATA_O_NLI[$i+1]*(2**6))&0xFC0 | $PE2_DATA_O_NLI[$i]&0x3F), $LINE_MEM; }
		if($PE2_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE2_DATA_O_NLI[$i+1]&0xFF  ), sprintf("%2.2x",$PE2_DATA_O_NLI[$i]&0xFF  ), $LINE_MEM;}
		if($PE2_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE2_DATA_O_NLI[$i+1]&0xFFF ), sprintf("%3.3x",$PE2_DATA_O_NLI[$i]&0xFFF ), $LINE_MEM;}
		if($PE2_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_O_NLI[$i+1]&0xFFFF), sprintf("%4.4x",$PE2_DATA_O_NLI[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE2_LINE_O[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE2_MAC_ROW_NUM_ZP){
while($k < $PE2_TO_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE2_TO_DATApWORD; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE2_DATA_O0_IM[$i]&0x3F,$PE2_DATA_O0_RE[$i]&0x3F);
		if($PE2_TO_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE2_DATA_TO[$i]&0xFFFF), $LINE_MEM;}
		if($PE2_TO_PREC==$PREC_24B){	$LINE_MEM = join "", sprintf("%6.6x",$PE2_DATA_TO[$i]&0xFFFFFF), $LINE_MEM;}
		if($PE2_TO_PREC==$PREC_32B){	$LINE_MEM = join "", sprintf("%8.8x",$PE2_DATA_TO[$i]&0xFFFFFFFF), $LINE_MEM;}
		$i = $i + 1;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE2_LINE_TO[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
#
# MAC DATA PACKING INTO 96B/WORD
# PE3
#
$k = 0;
$i = 0;
$j = 0;
$p = 0;
$q = 0;
$idx1 = 0;
$idx2 = 0;
$LINE_MEM = ""; 	
printf OUT_LOG "\n";
printf OUT_LOG "W_INDEX INFO IN STORING ORDER: W_INDEX = CURRENT_ROW_INDEX x TOT_COL_NUM + INDEX_OF_COL_IN_CURRENT_ROW\n";
printf OUT_LOG "\n";
while ($k < $PE3_W_MEM_SIZE){
	$idx1 = $j*$PE3_MAC_COL_NUM_ZP + $i*$PE3_I_DATApWORD + $p*2;
	$idx2 = $idx1 + 1;
	printf OUT_LOG sprintf("%4d\t%4d\t",$idx1,$idx2);
	if($PE3_W_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_W[$idx2]*(2**6))&0xFC0 | $PE3_DATA_W[$idx1]&0x3F), $LINE_MEM; }
	if($PE3_W_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_W[$idx2]&0xFF  ), sprintf("%2.2x",$PE3_DATA_W[$idx1]&0xFF  ), $LINE_MEM;}
	if($PE3_W_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_W[$idx2]&0xFFF ), sprintf("%3.3x",$PE3_DATA_W[$idx1]&0xFFF ), $LINE_MEM;}
	if($PE3_W_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_W[$idx2]&0xFFFF), sprintf("%4.4x",$PE3_DATA_W[$idx1]&0xFFFF), $LINE_MEM;}
	#printf OUT_LOG "$k\t$LINE_MEM\n";
	$q++;
	if($q == $PE3_W_DATApWORD/2){	
		$q = 0; 	
		if ($PE3_INST && $PE3_W == 0){		$PE0_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_W == 1){		$PE1_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_W == 2){		$PE2_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		if ($PE3_INST && $PE3_W == 3){		$PE3_MEM_LINES[$k + $PE3_W_MEM_START_BASE] = "$LINE_MEM\n"; }
		$k++;
		$LINE_MEM = ""; 	
		printf OUT_LOG "\n";
	}
	$p++;
	if($p == $PE3_I_DATApWORD/2){	$p = 0;		$j++; }
	if($j == $PE3_MAC_ROW_NUM_ZP){	$j = 0;		$i++; 	printf OUT_LOG "\n";}
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $MAC_COL_NUM_ZP){
while($k < $PE3_I_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE3_I_DATApWORD/2; $q++){	
		#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_I0_IM[$i]&0x3F,$PE3_DATA_I0_RE[$i]&0x3F);
		if($PE3_I_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_I[$i+1]*(2**6))&0xFC0 | $PE3_DATA_I[$i]&0x3F), $LINE_MEM; }
		if($PE3_I_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_I[$i+1]&0xFF  ), sprintf("%2.2x",$PE3_DATA_I[$i]&0xFF  ), $LINE_MEM;}
		if($PE3_I_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_I[$i+1]&0xFFF ), sprintf("%3.3x",$PE3_DATA_I[$i]&0xFFF ), $LINE_MEM;}
		if($PE3_I_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_I[$i+1]&0xFFFF), sprintf("%4.4x",$PE3_DATA_I[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE3_INST && $PE3_I == 0){		$PE0_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_I == 1){		$PE1_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_I == 2){		$PE2_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_I == 3){		$PE3_MEM_LINES[$k + $PE3_I_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
# NOTE THAT 
# $OS_MEM_SIZE = $O_MEM_SIZE
# $OS_PREC = $PE3_O_PREC
#
#while($i < $PE3_MAC_ROW_NUM_ZP){
while($k < $PE3_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE3_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_O0_IM[$i]&0x3F,$PE3_DATA_O0_RE[$i]&0x3F);
		if($PE3_O_PREC==$PREC_06B){	$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_OS[$i+1]*(2**6))&0xFC0 | $PE3_DATA_OS[$i]&0x3F), $LINE_MEM; }
		if($PE3_O_PREC==$PREC_08B){	$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_OS[$i+1]&0xFF  ), sprintf("%2.2x",$PE3_DATA_OS[$i]&0xFF  ), $LINE_MEM;}
		if($PE3_O_PREC==$PREC_12B){	$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_OS[$i+1]&0xFFF ), sprintf("%3.3x",$PE3_DATA_OS[$i]&0xFFF ), $LINE_MEM;}
		if($PE3_O_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_OS[$i+1]&0xFFFF), sprintf("%4.4x",$PE3_DATA_OS[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	if ($PE3_INST && $PE3_OS == 0){		$PE0_MEM_LINES[$k + $PE3_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_OS == 1){		$PE1_MEM_LINES[$k + $PE3_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_OS == 2){		$PE2_MEM_LINES[$k + $PE3_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	if ($PE3_INST && $PE3_OS == 3){		$PE3_MEM_LINES[$k + $PE3_OS_MEM_START_BASE] = "$LINE_MEM\n"; }
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE3_MAC_ROW_NUM_ZP){
while($k < $PE3_O_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE3_O_DATApWORD/2; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_O0_IM[$i]&0x3F,$PE3_DATA_O0_RE[$i]&0x3F);
		if($PE3_O_PREC==$PREC_06B){		$LINE_MEM = join "", sprintf("%3.3x",($PE3_DATA_O_NLI[$i+1]*(2**6))&0xFC0 | $PE3_DATA_O_NLI[$i]&0x3F), $LINE_MEM; }
		if($PE3_O_PREC==$PREC_08B){		$LINE_MEM = join "", sprintf("%2.2x",$PE3_DATA_O_NLI[$i+1]&0xFF  ), sprintf("%2.2x",$PE3_DATA_O_NLI[$i]&0xFF  ), $LINE_MEM;}
		if($PE3_O_PREC==$PREC_12B){		$LINE_MEM = join "", sprintf("%3.3x",$PE3_DATA_O_NLI[$i+1]&0xFFF ), sprintf("%3.3x",$PE3_DATA_O_NLI[$i]&0xFFF ), $LINE_MEM;}
		if($PE3_O_PREC==$PREC_16B){		$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_O_NLI[$i+1]&0xFFFF), sprintf("%4.4x",$PE3_DATA_O_NLI[$i]&0xFFFF), $LINE_MEM;}
		$i = $i + 2;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE3_LINE_O[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";
$k = 0;
$i = 0;
#while($i < $PE3_MAC_ROW_NUM_ZP){
while($k < $PE3_TO_MEM_SIZE){
	$LINE_MEM = "";
	for($q = 0; $q < $PE3_TO_DATApWORD; $q++){
		#print sprintf("%6.6b\t%6.6b\n",$PE3_DATA_O0_IM[$i]&0x3F,$PE3_DATA_O0_RE[$i]&0x3F);
		if($PE3_TO_PREC==$PREC_16B){	$LINE_MEM = join "", sprintf("%4.4x",$PE3_DATA_TO[$i]&0xFFFF), $LINE_MEM;}
		if($PE3_TO_PREC==$PREC_24B){	$LINE_MEM = join "", sprintf("%6.6x",$PE3_DATA_TO[$i]&0xFFFFFF), $LINE_MEM;}
		if($PE3_TO_PREC==$PREC_32B){	$LINE_MEM = join "", sprintf("%8.8x",$PE3_DATA_TO[$i]&0xFFFFFFFF), $LINE_MEM;}
		$i = $i + 1;
	}
	#printf OUT_LOG "$LINE_MEM\n";
	$PE3_LINE_TO[$k] = "$LINE_MEM\n";
	$k++;
}
printf OUT_LOG "\n";
printf OUT_LOG "\n";

#
# WRITE PE_MEM w/ INPUT DATA
# MAC I/W/OS DATA WRITING INTO MEMORY
#
# debug purpose: this should be the same as DNN_INIT
my $test_OF0 = join "", $inst, "_debug_MEM_0.data";
open ($test_OUT0, ">$test_OF0")
	or die "cannot open < ", $test_OF0, ": $!";
my $test_OF1 = join "", $inst, "_debug_MEM_1.data";
open ($test_OUT1, ">$test_OF1")
	or die "cannot open < ", $test_OF1, ": $!";
my $test_OF2 = join "", $inst, "_debug_MEM_2.data";
open ($test_OUT2, ">$test_OF2")
	or die "cannot open < ", $test_OF2, ": $!";
my $test_OF3 = join "", $inst, "_debug_MEM_3.data";
open ($test_OUT3, ">$test_OF3")
	or die "cannot open < ", $test_OF3, ": $!";

for($i=0; $i<$MEM_OUT_BOUND; $i++){
	printf $test_OUT0 $PE0_MEM_LINES[$i];
	printf $test_OUT1 $PE1_MEM_LINES[$i];
	printf $test_OUT2 $PE2_MEM_LINES[$i];
	printf $test_OUT3 $PE3_MEM_LINES[$i];
}
close ($test_OUT0);
close ($test_OUT1);
close ($test_OUT2);
close ($test_OUT3);

## caogao
printf $output_file sprintf("%d %d %d %d\n", $PE0_MAC_ROW_NUM_ORG, $PE0_O_EXP, $PE0_O_EXP, $PE0_TO_EXP);
for ($i = 0; $i < $MAC_ROW_NUM_ORG; $i++){
  printf $output_file sprintf("%d ", $PE0_DATA_O_NLI[$i]);
}

close ($output_file);
#
#
# WRITE PE_MEM w/ OUTPUT DATA
# MAC I/W/OS + O/TO DATA WRITING INTO MEMORY
#
if (defined $argv[1] and ($argv[1] ne "NULL")) {
  $OF0 = join "", $inst, "_MEM_0.data";
  $OF1 = join "", $inst, "_MEM_1.data";
  $OF2 = join "", $inst, "_MEM_2.data";
  $OF3 = join "", $inst, "_MEM_3.data";
} else {
  $OF0 = join "", $inst, "_MEM_0.goldendata";
  $OF1 = join "", $inst, "_MEM_1.goldendata";
  $OF2 = join "", $inst, "_MEM_2.goldendata";
  $OF3 = join "", $inst, "_MEM_3.goldendata";
}
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
$k = 0;
for($i=$PE0_TO_MEM_START; $i<($PE0_TO_MEM_START + $PE0_TO_MEM_SIZE); $i++){
	$LINE_MEM = $PE0_LINE_TO[$k];
	$k++;
	if ($PE0_INST == 1){
				$PE0_MEM_LINES[$i] =  $LINE_MEM; 
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
$k = 0;
for($i=$PE1_TO_MEM_START; $i<($PE1_TO_MEM_START + $PE1_TO_MEM_SIZE); $i++){
	$LINE_MEM = $PE1_LINE_TO[$k];
	$k++;
	if ($PE1_INST == 1){
				$PE1_MEM_LINES[$i] =  $LINE_MEM; 
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
$k = 0;
for($i=$PE2_TO_MEM_START; $i<($PE2_TO_MEM_START + $PE2_TO_MEM_SIZE); $i++){
	$LINE_MEM = $PE2_LINE_TO[$k];
	$k++;
	if ($PE2_INST == 1){
				$PE2_MEM_LINES[$i] =  $LINE_MEM; 
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
$k = 0;
for($i=$PE3_TO_MEM_START; $i<($PE3_TO_MEM_START + $PE3_TO_MEM_SIZE); $i++){
	$LINE_MEM = $PE3_LINE_TO[$k];
	$k++;
	if ($PE3_INST == 1){
				$PE3_MEM_LINES[$i] =  $LINE_MEM; 
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
################################################################################
### caogao
close(OUT_DNN_NLI);
close(OUT_DNN_INST);

print ("\n");
print ("EXECUTING   ./V2_MEM.pl\n");

system ($^X,"V2_MEM.pl", $OF0);
system ($^X,"V2_MEM.pl", $OF1);
system ($^X,"V2_MEM.pl", $OF2);
system ($^X,"V2_MEM.pl", $OF3);

if (not (defined $argv[1] and $argv[1] ne "NULL")) {
  $OF0 = join "", $inst, "_MEM_0.data";
  system ($^X,"V2_MEM.pl", $OF0);
  $OF1 = join "", $inst, "_MEM_1.data";
  system ($^X,"V2_MEM.pl", $OF1);
  $OF2 = join "", $inst, "_MEM_2.data";
  system ($^X,"V2_MEM.pl", $OF2);
  $OF3 = join "", $inst, "_MEM_3.data";
  system ($^X,"V2_MEM.pl", $OF3);
}

print ("COMPLETED\n");
print ("\n");
##########
} ## end MAC_sub_face_M1
