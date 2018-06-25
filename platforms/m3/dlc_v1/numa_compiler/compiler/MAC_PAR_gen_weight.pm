#!/usr/bin/env perl
package MAC_PAR_gen_weight;
#use strict;
use warnings;
use POSIX ;
use Exporter;

our @ISA=qw(Exporter);
our @EXPORT=qw(MAC_PAR_gen_weight);

######################################
## input: $inst, $INPUT_MEM_FILE, input, ADDR, (precision), $OUTPUT_MEM_FILE, output, config [input size, fft_num]
## output: none (output_mem_file & output as file)

sub MAC_PAR_gen_weight {

my (@argv) = @_;

foreach my $nn (@argv) {
  print $nn, "\t";
}
print "\n";

my $inst = "MAC_INIT";
if (defined $argv[0]) {
  $inst = $argv[0];
}

my $OF_LOG = join "", $inst, "_GEN.log";
open (OUT_LOG, ">$OF_LOG")
	or die "cannot open < ", $OF_LOG, ": $!";

my $INPUT_MEM_FILE; 
my $OF0;
my $OF1;
my $OF2;
my $OF3;
if (defined $argv[1] and ($argv[1] ne "NULL")) {
  $INPUT_MEM_FILE = $argv[1];    ## memory footprint before this MAC (should include inputs) 
}
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

my $PE0_MAC_NLI		= 1;
my $PE1_MAC_NLI		= 1;
my $PE2_MAC_NLI		= 1;
my $PE3_MAC_NLI		= 1;

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
if (defined $argv[2]) { $weight_filename = $argv[2]; }
if (defined $argv[3]) { $input_filename = $argv[3]; }
if (defined $argv[4]) { $offset_filename = $argv[4]; }
my $PE0_import_input_file = 1;
my $PE1_import_input_file = 0;
my $PE2_import_input_file = 0;
my $PE3_import_input_file = 0;
if (defined $argv[5]) {
  $PE0_import_input_file = $argv[5] & 0b0001;
  $PE1_import_input_file = $argv[5] & 0b0010;
  $PE2_import_input_file = $argv[5] & 0b0100;
  $PE3_import_input_file = $argv[5] & 0b1000;
}

open($weight_file, "<", $weight_filename) 
	or die "cannot open < ", $weight_filename, ": $!";
open($input_file, "<", $input_filename)     ## need input file even import_input_file are all 0: precisions are stored in input files 
	or die "cannot open < ", $input_filename, ": $!";
open($offset_file, "<", $offset_filename) 
	or die "cannot open < ", $offset_filename, ": $!";

my $PE0_INST		= 1;
my $PE1_INST		= 1;
my $PE2_INST		= 1;
my $PE3_INST		= 1;
if (defined $argv[6]) {
  $PE0_INST	= $argv[6] & 0b0001;
  $PE1_INST	= $argv[6] & 0b0010;
  $PE2_INST	= $argv[6] & 0b0100;
  $PE3_INST	= $argv[6] & 0b1000;
}
#
my $PE0_MAC_OFFSET	= 1;
my $PE1_MAC_OFFSET	= 1;
my $PE2_MAC_OFFSET	= 1;
my $PE3_MAC_OFFSET	= 1;
if (defined $argv[7]) {
  $PE0_MAC_OFFSET	= $argv[7] & 0b0001;
  $PE1_MAC_OFFSET	= $argv[7] & 0b0010;
  $PE2_MAC_OFFSET	= $argv[7] & 0b0100;
  $PE3_MAC_OFFSET	= $argv[7] & 0b1000;
}

my $PE0_I_MEM_START_BASE  = $MEM_H1B1_START; 
my $PE0_W_MEM_START_BASE  = $MEM_H4B1_START; 
my $PE0_OS_MEM_START_BASE = $MEM_H2B3_START; 
if (defined $argv[9] and defined $argv[10] and defined $argv[11]) {
  $PE0_I_MEM_START_BASE = $argv[9]; 
  $PE0_W_MEM_START_BASE = $argv[10]; 
  $PE0_OS_MEM_START_BASE = $argv[11]; 
}
#
my $PE1_I_MEM_START_BASE  = $MEM_H1B1_START; 
my $PE1_W_MEM_START_BASE  = $MEM_H4B1_START; 
my $PE1_OS_MEM_START_BASE = $MEM_H2B3_START; 
if (defined $argv[12] and defined $argv[13] and defined $argv[14]) {
  $PE1_I_MEM_START_BASE = $argv[12]; 
  $PE1_W_MEM_START_BASE = $argv[13]; 
  $PE1_OS_MEM_START_BASE = $argv[14]; 
}
#
my $PE2_I_MEM_START_BASE  = $MEM_H1B1_START;  
my $PE2_W_MEM_START_BASE  = $MEM_H4B1_START;  
my $PE2_OS_MEM_START_BASE = $MEM_H2B3_START;  
if (defined $argv[15] and defined $argv[16] and defined $argv[17]) {
  $PE2_I_MEM_START_BASE = $argv[15]; 
  $PE2_W_MEM_START_BASE = $argv[16]; 
  $PE2_OS_MEM_START_BASE = $argv[17]; 
}
#
my $PE3_I_MEM_START_BASE  = $MEM_H1B1_START;  
my $PE3_W_MEM_START_BASE  = $MEM_H4B1_START;  
my $PE3_OS_MEM_START_BASE = $MEM_H2B3_START;  
if (defined $argv[18] and defined $argv[19] and defined $argv[20]) {
  $PE3_I_MEM_START_BASE = $argv[18]; 
  $PE3_W_MEM_START_BASE = $argv[19]; 
  $PE3_OS_MEM_START_BASE = $argv[20]; 
}

## doesn't matter
my $PE0_TO_MEM_START	    = 0;  
my $PE0_O_MEM_START_BASE  = 0;  
my $PE1_TO_MEM_START	    = 0;  
my $PE1_O_MEM_START_BASE  = 0;  
my $PE2_TO_MEM_START	    = 0;  
my $PE2_O_MEM_START_BASE  = 0;  
my $PE3_TO_MEM_START	    = 0;  
my $PE3_O_MEM_START_BASE  = 0;  
###########
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
if (defined $argv[5] and $argv[5] != 0) {
  $MAC_COL_NUM_ORG == $line[0]
	  or die "input file does not match weight file";
}
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

$MAC_ROW_NUM_ORG = ceil($MAC_ROW_NUM_ORG / 4);
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
my $ROW_NUM_MIN_FACTOR = 24;
my $COL_NUM_MIN_FACTOR = 8; ## same as $PE_I_DATApWORD

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

# FOLLOWING IS THE MAGIC NUMBER!
#
#my $ROW_NUM_MIN_FACTOR	= 48;
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

# UPDATE $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#
# ADD CODE TO CALCUATE LCM LATER!
# $ROW_NUM_MIN_FACTPR = LCM($W_ROW_UNIT, $O_DATApWORD, $TO_DATApWORD)
#

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
my $PE0_O_SIGN		= 1;
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
my $PE0_SHIFT		= int(($PE0_I_EXP + $PE0_W_EXP) + (log($PE0_MAC_COL_NUM_ZP)/log(2)) - $PE0_O_EXP);
#my $PE0_SHIFT		= int(($PE0_I_EXP + $PE0_W_EXP) + (log($PE0_MAC_COL_NUM_ZP)/log(2)) - $PE0_O_EXP + 1);
my $PE0_SHIFT_OS 	= 0;
my $PE0_SHIFT_NL	= 10;
#
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
my $PE1_O_SIGN		= 1;
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
my $PE2_O_SIGN		= 1;
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
my $PE3_O_SIGN		= 1;
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
	}
	$k++;
}
############################################################################
#
# CALCULATE MAC OUTPUTS $PE0_DATA_O
#
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
	}
	$k++;
}
###############################################################
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
	}
	$k++;
}
##########################################
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
if ($PE0_import_input_file) {
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
}
if ($PE0_MAC_OFFSET) {
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
} ## end if $PE0_MAC_OFFSET
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
if ($PE1_import_input_file) {
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
}
if ($PE1_MAC_OFFSET) {
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
}
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
if ($PE2_import_input_file) {
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
}
if ($PE2_MAC_OFFSET) {
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
}
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
if ($PE3_import_input_file) {
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
}
if ($PE3_MAC_OFFSET) {
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
}
################################################################################
### caogao
#
# WRITE PE_MEM w/ INPUT DATA
# MAC I/W/OS DATA WRITING INTO MEMORY
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
close (OUT_LOG);

system ($^X,"V2_MEM.pl", $OF0);
system ($^X,"V2_MEM.pl", $OF1);
system ($^X,"V2_MEM.pl", $OF2);
system ($^X,"V2_MEM.pl", $OF3);
##########
} ## end FFT_GEN_WEIGHT
