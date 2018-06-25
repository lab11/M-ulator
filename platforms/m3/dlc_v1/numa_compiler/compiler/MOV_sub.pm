#!/usr/bin/perl
package MOV_sub;
use warnings;
use POSIX ;
use Exporter;

our @ISA=qw(Exporter);
our @EXPORT=qw(MOV_sub);

######################################
##( input: $inst, $INPUT_MEM_FILE, input, ADDR, $OUTPUT_MEM_FILE, output, config [input size])
## input: inst_name, memory_file, PE_used, dest_PE0, dest_PE1, dest_PE2, dest_PE3, addresses) 
## output: output_mem_file & output as file

sub MOV_sub {

my (@argv) = @_;

foreach my $n (@argv) {
  print $n, "\t";
}
print "\n";

my $inst = "MOV";
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

my $out_dnn_inst_filename = "dnn.inst.c";
open (OUT_DNN_INST, ">>$out_dnn_inst_filename")
	or die "cannot open < ", $out_dnn_inst_filename, ": $!";

my $i;
my $j;
my $LINE;

##########
## caogao

my $OP_FFT		= 0;
my $OP_MAC		= 1;
my $OP_NLI		= 2;
my $OP_MOV		= 3;

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

my $PREC_06B 		= 0;
my $PREC_08B 		= 1;
my $PREC_12B 		= 2;
my $PREC_16B 		= 3;
my $PREC_24B 		= 4;
my $PREC_32B 		= 5;

################################################################################
my $PE0_INST		= 1;
my $PE1_INST		= 0;
my $PE2_INST		= 0;
my $PE3_INST		= 0;
if (defined $argv[2]) {
  $PE0_INST	 = ($argv[2] & 0b0001) != 0;
  $PE1_INST	 = ($argv[2] & 0b0010) != 0;
  $PE2_INST	 = ($argv[2] & 0b0100) != 0;
  $PE3_INST	 = ($argv[2] & 0b1000) != 0;
}

my $PE0_I 		= 0;
my $PE1_I 		= 1;
my $PE2_I 		= 2;
my $PE3_I 		= 3;
my $PE0_O 		= 0;
if (defined $argv[3]) { $PE0_O = $argv[3];}
my $PE1_O 		= 1;
if (defined $argv[4]) { $PE1_O = $argv[4];}
my $PE2_O 		= 2;
if (defined $argv[5]) { $PE2_O = $argv[5];}
my $PE3_O 		= 3;
if (defined $argv[6]) { $PE3_O = $argv[6];}

###################
my $PE0_I_MEM_START_BASE = 0;
my $PE0_O_MEM_START_BASE = 1000;		## equal to old PE1_O_MEM_START + PE1_O_MEM_SIZE
my $PE0_I_MEM_SIZE = 6;
if (defined $argv[7] and defined $argv[8] and defined $argv[9]) {
  $PE0_I_MEM_START_BASE = $argv[7]; 
  $PE0_O_MEM_START_BASE = $argv[8]; 
  $PE0_I_MEM_SIZE = $argv[9]; 
}
my $PE0_I_MEM_START = $PE0_I_MEM_START_BASE + $PE0_I * (2**13);
my $PE0_O_MEM_START = $PE0_O_MEM_START_BASE + $PE0_O * (2**13);	
my $PE0_O_MEM_SIZE 	= $PE0_I_MEM_SIZE;

my $PE1_I_MEM_START_BASE = 0;
my $PE1_O_MEM_START_BASE = 1000;		## equal to old PE1_O_MEM_START + PE1_O_MEM_SIZE
my $PE1_I_MEM_SIZE = 6;
if (defined $argv[10] and defined $argv[11] and defined $argv[12]) {
  $PE1_I_MEM_START_BASE = $argv[10]; 
  $PE1_O_MEM_START_BASE = $argv[11]; 
  $PE1_I_MEM_SIZE = $argv[12]; 
}
my $PE1_I_MEM_START = $PE1_I_MEM_START_BASE + $PE1_I * (2**13);
my $PE1_O_MEM_START = $PE1_O_MEM_START_BASE + $PE1_O * (2**13);	
my $PE1_O_MEM_SIZE 	= $PE1_I_MEM_SIZE;

my $PE2_I_MEM_START_BASE = 0;
my $PE2_O_MEM_START_BASE = 1000;		## equal to old PE2_O_MEM_START + PE2_O_MEM_SIZE
my $PE2_I_MEM_SIZE = 6;
if (defined $argv[13] and defined $argv[14] and defined $argv[15]) {
  $PE2_I_MEM_START_BASE = $argv[13]; 
  $PE2_O_MEM_START_BASE = $argv[14]; 
  $PE2_I_MEM_SIZE = $argv[15]; 
}
my $PE2_I_MEM_START = $PE2_I_MEM_START_BASE + $PE2_I * (2**13);
my $PE2_O_MEM_START = $PE2_O_MEM_START_BASE + $PE2_O * (2**13);	
my $PE2_O_MEM_SIZE 	= $PE2_I_MEM_SIZE;

my $PE3_I_MEM_START_BASE = 0;
my $PE3_O_MEM_START_BASE = 1000;		## equal to old PE1_O_MEM_START + PE1_O_MEM_SIZE
my $PE3_I_MEM_SIZE = 6;
if (defined $argv[16] and defined $argv[17] and defined $argv[18]) {
  $PE3_I_MEM_START_BASE = $argv[16]; 
  $PE3_O_MEM_START_BASE = $argv[17]; 
  $PE3_I_MEM_SIZE = $argv[18]; 
}
my $PE3_I_MEM_START = $PE3_I_MEM_START_BASE + $PE3_I * (2**13);
my $PE3_O_MEM_START = $PE3_O_MEM_START_BASE + $PE3_O * (2**13);	
my $PE3_O_MEM_SIZE 	= $PE3_I_MEM_SIZE;
#####
$PE0_OP		= $OP_MOV;
$PE0_TO_MEM_START	= 0;
$PE0_TO_MEM_SIZE	= 0;
$PE0_TO_PREC		= 0;
$PE0_I_PREC		= 0;
$PE0_O_PREC		= 0;
$PE0_O_SIGN		= 1;
$PE0_OS_MEM_START 	= 0;
$PE0_W_MEM_START  	= 0;
$PE0_W_MEM_SIZE	= 0;
$PE0_W_PREC		= 0;
$PE0_MAC_OFFSET	= 0;
$PE0_MAC_NLI		= 0;
$PE0_MAC_ROW_NUM	= 0;
$PE0_FFT_NUM		= 0;
$PE0_FFT_I_UNIT_SIZE	= 0;
$PE0_FFT_O_UNIT_SIZE	= 0;
$PE0_SHIFT 		= 0;
$PE0_SHIFT_OS 	= 0;
$PE0_SHIFT_NL	= 0;
#
$PE1_OP		= $OP_MOV;
$PE1_TO_MEM_START	= 0;
$PE1_TO_MEM_SIZE	= 0;
$PE1_TO_PREC		= 0;
$PE1_I_PREC		= 0;
$PE1_O_PREC		= 0;
$PE1_O_SIGN		= 1;
$PE1_OS_MEM_START 	= 0;
$PE1_W_MEM_START  	= 0;
$PE1_W_MEM_SIZE	= 0;
$PE1_W_PREC		= 0;
$PE1_MAC_OFFSET	= 0;
$PE1_MAC_NLI		= 0;
$PE1_MAC_ROW_NUM	= 0;
$PE1_FFT_NUM		= 0;
$PE1_FFT_I_UNIT_SIZE	= 0;
$PE1_FFT_O_UNIT_SIZE	= 0;
$PE1_SHIFT 		= 0;
$PE1_SHIFT_OS 	= 0;
$PE1_SHIFT_NL	= 0;
#
$PE2_OP		= $OP_MOV;
$PE2_TO_MEM_START	= 0;
$PE2_TO_MEM_SIZE	= 0;
$PE2_TO_PREC		= 0;
$PE2_I_PREC		= 0;
$PE2_O_PREC		= 0;
$PE2_O_SIGN		= 1;
$PE2_OS_MEM_START 	= 0;
$PE2_W_MEM_START  	= 0; 
$PE2_W_MEM_SIZE	= 0;
$PE2_W_PREC		= 0;
$PE2_MAC_OFFSET	= 0;
$PE2_MAC_NLI		= 0;
$PE2_MAC_ROW_NUM	= 0;
$PE2_FFT_NUM		= 0;
$PE2_FFT_I_UNIT_SIZE	= 0;
$PE2_FFT_O_UNIT_SIZE	= 0;
$PE2_SHIFT 		= 0;
$PE2_SHIFT_OS 	= 0;
$PE2_SHIFT_NL	= 0;
#
$PE3_OP		= $OP_MOV;
$PE3_TO_MEM_START	= 0;
$PE3_TO_MEM_SIZE	= 0;
$PE3_TO_PREC		= 0;
$PE3_I_PREC		= 0;
$PE3_O_PREC		= 0;
$PE3_O_SIGN		= 1;
$PE3_OS_MEM_START 	= 0;
$PE3_W_MEM_START  	= 0;
$PE3_W_MEM_SIZE	= 0;
$PE3_W_PREC		= 0;
$PE3_MAC_OFFSET	= 0;
$PE3_MAC_NLI		= 0;
$PE3_MAC_ROW_NUM	= 0;
$PE3_FFT_NUM		= 0;
$PE3_FFT_I_UNIT_SIZE	= 0;
$PE3_FFT_O_UNIT_SIZE	= 0;
$PE3_SHIFT 		= 0;
$PE3_SHIFT_OS 	= 0;
$PE3_SHIFT_NL	= 0;
###################

print 	"\n";
print	sprintf("PE0_I_MEM_START  = %d\n", $PE0_I_MEM_START);
print	sprintf("PE0_I_MEM_END    = %d\n", $PE0_I_MEM_START + $PE0_I_MEM_SIZE - 1);
print	sprintf("PE0_O_MEM_START  = %d\n", $PE0_O_MEM_START);
print	sprintf("PE0_O_MEM_END    = %d\n", $PE0_O_MEM_START + $PE0_O_MEM_SIZE - 1);
#

$PE0_INST_0 = "";
$PE0_INST_1 = "";
$PE0_INST_2 = "";
$PE0_INST_3 = "";
$PE0_INST_4 = "";
$PE0_INST_5 = "";

$PE0_MAC_EN = 1;

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

$PE0_INST_1 = "";
$PE0_INST_1 = join "", sprintf("%14.14b",int($PE0_I_MEM_START/2) & (2**14-1)), $PE0_INST_1;
$PE0_INST_1 = join "", sprintf("%15.15b",$PE0_I_MEM_SIZE), $PE0_INST_1;
$PE0_INST_1 = join "", sprintf("%3.3b",$PE0_I_PREC), $PE0_INST_1;

$PE0_INST_2 = "";
$PE0_INST_2 = join "", sprintf("%15.15b",$PE0_O_MEM_START), $PE0_INST_2;
$PE0_INST_2 = join "", sprintf("%15.15b",$PE0_O_MEM_SIZE), $PE0_INST_2;
$PE0_INST_2 = join "", sprintf("%2.2b",$PE0_O_PREC & 0x3), $PE0_INST_2;

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

$PE0_INST_4 = "";
$PE0_INST_4 = join "", sprintf("%15.15b",$PE0_W_MEM_SIZE), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%3.3b",$PE0_W_PREC), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%1.1b",$PE0_MAC_OFFSET), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%1.1b",$PE0_MAC_NLI), $PE0_INST_4;
$PE0_INST_4 = join "", sprintf("%12.12b",$PE0_MAC_ROW_NUM & (2**12-1)), $PE0_INST_4;

$PE0_INST_5 = "";
$PE0_INST_5 = join "", sprintf("%3.3b",int($PE0_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT_OS), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%5.5b",$PE0_SHIFT_NL), $PE0_INST_5;
$PE0_INST_5 = join "", sprintf("%14.14b",0), $PE0_INST_5;

# GENERATE DATA FOR TESTING MOV
# PE1
#
print 	"\n";
print	sprintf("PE1_I_MEM_START  = %d\n", $PE1_I_MEM_START);
print	sprintf("PE1_I_MEM_END    = %d\n", $PE1_I_MEM_START + $PE1_I_MEM_SIZE - 1);
print	sprintf("PE1_O_MEM_START  = %d\n", $PE1_O_MEM_START);
print	sprintf("PE1_O_MEM_END    = %d\n", $PE1_O_MEM_START + $PE1_O_MEM_SIZE - 1);
#

$PE1_INST_0 = "";
$PE1_INST_1 = "";
$PE1_INST_2 = "";
$PE1_INST_3 = "";
$PE1_INST_4 = "";
$PE1_INST_5 = "";

$PE1_MAC_EN = 1;

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

$PE1_INST_1 = "";
$PE1_INST_1 = join "", sprintf("%14.14b",int($PE1_I_MEM_START/2) & (2**14-1)), $PE1_INST_1;
$PE1_INST_1 = join "", sprintf("%15.15b",$PE1_I_MEM_SIZE), $PE1_INST_1;
$PE1_INST_1 = join "", sprintf("%3.3b",$PE1_I_PREC), $PE1_INST_1;

$PE1_INST_2 = "";
$PE1_INST_2 = join "", sprintf("%15.15b",$PE1_O_MEM_START), $PE1_INST_2;
$PE1_INST_2 = join "", sprintf("%15.15b",$PE1_O_MEM_SIZE), $PE1_INST_2;
$PE1_INST_2 = join "", sprintf("%2.2b",$PE1_O_PREC & 0x3), $PE1_INST_2;

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

$PE1_INST_4 = "";
$PE1_INST_4 = join "", sprintf("%15.15b",$PE1_W_MEM_SIZE), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%3.3b",$PE1_W_PREC), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%1.1b",$PE1_MAC_OFFSET), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%1.1b",$PE1_MAC_NLI), $PE1_INST_4;
$PE1_INST_4 = join "", sprintf("%12.12b",$PE1_MAC_ROW_NUM & (2**12-1)), $PE1_INST_4;

$PE1_INST_5 = "";
$PE1_INST_5 = join "", sprintf("%3.3b",int($PE1_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT_OS), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%5.5b",$PE1_SHIFT_NL), $PE1_INST_5;
$PE1_INST_5 = join "", sprintf("%14.14b",0), $PE1_INST_5;

# GENERATE DATA FOR TESTING MOV
# PE2
#
print 	"\n";
print	sprintf("PE2_I_MEM_START  = %d\n", $PE2_I_MEM_START);
print	sprintf("PE2_I_MEM_END    = %d\n", $PE2_I_MEM_START + $PE2_I_MEM_SIZE - 1);
print	sprintf("PE2_O_MEM_START  = %d\n", $PE2_O_MEM_START);
print	sprintf("PE2_O_MEM_END    = %d\n", $PE2_O_MEM_START + $PE2_O_MEM_SIZE - 1);
#

$PE2_INST_0 = "";
$PE2_INST_1 = "";
$PE2_INST_2 = "";
$PE2_INST_3 = "";
$PE2_INST_4 = "";
$PE2_INST_5 = "";

$PE2_MAC_EN = 1;

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

$PE2_INST_1 = "";
$PE2_INST_1 = join "", sprintf("%14.14b",int($PE2_I_MEM_START/2) & (2**14-1)), $PE2_INST_1;
$PE2_INST_1 = join "", sprintf("%15.15b",$PE2_I_MEM_SIZE), $PE2_INST_1;
$PE2_INST_1 = join "", sprintf("%3.3b",$PE2_I_PREC), $PE2_INST_1;

$PE2_INST_2 = "";
$PE2_INST_2 = join "", sprintf("%15.15b",$PE2_O_MEM_START), $PE2_INST_2;
$PE2_INST_2 = join "", sprintf("%15.15b",$PE2_O_MEM_SIZE), $PE2_INST_2;
$PE2_INST_2 = join "", sprintf("%2.2b",$PE2_O_PREC & 0x3), $PE2_INST_2;

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

$PE2_INST_4 = "";
$PE2_INST_4 = join "", sprintf("%15.15b",$PE2_W_MEM_SIZE), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%3.3b",$PE2_W_PREC), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%1.1b",$PE2_MAC_OFFSET), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%1.1b",$PE2_MAC_NLI), $PE2_INST_4;
$PE2_INST_4 = join "", sprintf("%12.12b",$PE2_MAC_ROW_NUM & (2**12-1)), $PE2_INST_4;

$PE2_INST_5 = "";
$PE2_INST_5 = join "", sprintf("%3.3b",int($PE2_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT_OS), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%5.5b",$PE2_SHIFT_NL), $PE2_INST_5;
$PE2_INST_5 = join "", sprintf("%14.14b",0), $PE2_INST_5;

# GENERATE DATA FOR TESTING MOV
# PE3
#
print 	"\n";
print	sprintf("PE3_I_MEM_START  = %d\n", $PE3_I_MEM_START);
print	sprintf("PE3_I_MEM_END    = %d\n", $PE3_I_MEM_START + $PE3_I_MEM_SIZE - 1);
print	sprintf("PE3_O_MEM_START  = %d\n", $PE3_O_MEM_START);
print	sprintf("PE3_O_MEM_END    = %d\n", $PE3_O_MEM_START + $PE3_O_MEM_SIZE - 1);
#

$PE3_INST_0 = "";
$PE3_INST_1 = "";
$PE3_INST_2 = "";
$PE3_INST_3 = "";
$PE3_INST_4 = "";
$PE3_INST_5 = "";

$PE3_MAC_EN = 1;

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

$PE3_INST_1 = "";
$PE3_INST_1 = join "", sprintf("%14.14b",int($PE3_I_MEM_START/2) & (2**14-1)), $PE3_INST_1;
$PE3_INST_1 = join "", sprintf("%15.15b",$PE3_I_MEM_SIZE), $PE3_INST_1;
$PE3_INST_1 = join "", sprintf("%3.3b",$PE3_I_PREC), $PE3_INST_1;

$PE3_INST_2 = "";
$PE3_INST_2 = join "", sprintf("%15.15b",$PE3_O_MEM_START), $PE3_INST_2;
$PE3_INST_2 = join "", sprintf("%15.15b",$PE3_O_MEM_SIZE), $PE3_INST_2;
$PE3_INST_2 = join "", sprintf("%2.2b",$PE3_O_PREC & 0x3), $PE3_INST_2;

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

$PE3_INST_4 = "";
$PE3_INST_4 = join "", sprintf("%15.15b",$PE3_W_MEM_SIZE), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%3.3b",$PE3_W_PREC), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%1.1b",$PE3_MAC_OFFSET), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%1.1b",$PE3_MAC_NLI), $PE3_INST_4;
$PE3_INST_4 = join "", sprintf("%12.12b",$PE3_MAC_ROW_NUM & (2**12-1)), $PE3_INST_4;

$PE3_INST_5 = "";
$PE3_INST_5 = join "", sprintf("%3.3b",int($PE3_MAC_ROW_NUM/(2**12)) & (2**3-1)), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT_OS), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%5.5b",$PE3_SHIFT_NL), $PE3_INST_5;
$PE3_INST_5 = join "", sprintf("%14.14b",0), $PE3_INST_5;

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
#########
## caogao
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

## input
# PE0
#
$k = 0;
for($i=$PE0_I_MEM_START_BASE; $i<($PE0_I_MEM_START_BASE + $PE0_I_MEM_SIZE); $i++){
	if ($PE0_I == 0){	$PE0_LINE_O[$k] = $PE0_MEM_LINES[$i]; } 
	if ($PE0_I == 1){	$PE0_LINE_O[$k] = $PE1_MEM_LINES[$i]; } 
	if ($PE0_I == 2){	$PE0_LINE_O[$k] = $PE2_MEM_LINES[$i]; } 
	if ($PE0_I == 3){	$PE0_LINE_O[$k] = $PE3_MEM_LINES[$i]; }
	$k++;
}
# PE1
#
$k = 0;
for($i=$PE1_I_MEM_START_BASE; $i<($PE1_I_MEM_START_BASE + $PE1_I_MEM_SIZE); $i++){
	if ($PE1_I == 0){	$PE1_LINE_O[$k] = $PE0_MEM_LINES[$i]; } 
	if ($PE1_I == 1){	$PE1_LINE_O[$k] = $PE1_MEM_LINES[$i]; } 
	if ($PE1_I == 2){	$PE1_LINE_O[$k] = $PE2_MEM_LINES[$i]; } 
	if ($PE1_I == 3){	$PE1_LINE_O[$k] = $PE3_MEM_LINES[$i]; }
	$k++;
}
# PE2
#
$k = 0;
for($i=$PE2_I_MEM_START_BASE; $i<($PE2_I_MEM_START_BASE + $PE2_I_MEM_SIZE); $i++){
	if ($PE2_I == 0){	$PE2_LINE_O[$k] = $PE0_MEM_LINES[$i]; } 
	if ($PE2_I == 1){	$PE2_LINE_O[$k] = $PE1_MEM_LINES[$i]; } 
	if ($PE2_I == 2){	$PE2_LINE_O[$k] = $PE2_MEM_LINES[$i]; } 
	if ($PE2_I == 3){	$PE2_LINE_O[$k] = $PE3_MEM_LINES[$i]; }
	$k++;
}
# PE3
#
$k = 0;
for($i=$PE3_I_MEM_START_BASE; $i<($PE3_I_MEM_START_BASE + $PE3_I_MEM_SIZE); $i++){
	if ($PE3_I == 0){	$PE3_LINE_O[$k] = $PE0_MEM_LINES[$i]; } 
	if ($PE3_I == 1){	$PE3_LINE_O[$k] = $PE1_MEM_LINES[$i]; } 
	if ($PE3_I == 2){	$PE3_LINE_O[$k] = $PE2_MEM_LINES[$i]; } 
	if ($PE3_I == 3){	$PE3_LINE_O[$k] = $PE3_MEM_LINES[$i]; }
	$k++;
}

### output
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
# FFT I/W + O DATA WRITING INTO MEMORY
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

########
close(OUT_DNN_INST);

print ("\n");
print ("EXECUTING   ./V2_MEM.pl\n");

system ($^X,"V2_MEM.pl", $OF0);
system ($^X,"V2_MEM.pl", $OF1);
system ($^X,"V2_MEM.pl", $OF2);
system ($^X,"V2_MEM.pl", $OF3);

if (not (defined $argv[1] and $argv[1] ne "NULL")) {
  $OF0 = join "", $inst, "_MEM_0.goldendata";
  system ($^X,"V2_MEM.pl", $OF0);
  $OF1 = join "", $inst, "_MEM_1.goldendata";
  system ($^X,"V2_MEM.pl", $OF1);
  $OF2 = join "", $inst, "_MEM_2.goldendata";
  system ($^X,"V2_MEM.pl", $OF2);
  $OF3 = join "", $inst, "_MEM_3.goldendata";
  system ($^X,"V2_MEM.pl", $OF3);
}

print ("COMPLETED\n");
print ("\n");
##########
} ## end MOV_sub

