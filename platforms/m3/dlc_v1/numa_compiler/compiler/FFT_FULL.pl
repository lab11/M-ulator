#!/usr/bin/env perl

use warnings;
use POSIX;
use FFT_sub;
use FFT_GEN_WEIGHT;

### Full FFT operations:
### Initial memory footprint: FFT_INIT_*.data
### Final result footprint: FFT_$(iterations)_MEM_*.data
### Intermediate memory footprint: FFT_x_MEM_*.data 

#########
######## configurations
my $input_size = 512;
#$input_size = 128;
#my $i_exp = 16;
my $i_exp = 12;
my $w_exp = 12;
my $o_exp = $i_exp;
#########

## FFT_NUM: number of FFTs that needs to be performed. 
# The less elements each FFT need to work on, the more number of FFTs will be. 
# Input_size = FFT_NUM * per_FFT_SIZE * 2
my $FFT_NUM_START;    
if    ($i_exp == 6)  {$FFT_NUM_START = $input_size / 8 / 2;}
elsif ($i_exp == 8)  {$FFT_NUM_START = $input_size / 4 / 2;}
elsif ($i_exp == 12) {$FFT_NUM_START = $input_size / 4 / 2;}
elsif ($i_exp == 16) {$FFT_NUM_START = $input_size / 2 / 2;}
#
my $I_PREC;    
if    ($i_exp == 6)  {$I_PREC = 0;}
elsif ($i_exp == 8)  {$I_PREC = 1;}
elsif ($i_exp == 12) {$I_PREC = 2;}
elsif ($i_exp == 16) {$I_PREC = 3;}
#
my $W_PREC;    
if    ($w_exp == 6)  {$W_PREC = 0;}
elsif ($w_exp == 8)  {$W_PREC = 1;}
elsif ($w_exp == 12) {$W_PREC = 2;}
elsif ($w_exp == 16) {$W_PREC = 3;}
#
my $O_PREC;    
if    ($o_exp == 6)  {$O_PREC = 0;}
elsif ($o_exp == 8)  {$O_PREC = 1;}
elsif ($o_exp == 12) {$O_PREC = 2;}
elsif ($o_exp == 16) {$O_PREC = 3;}

my $inst = "FFT_INIT";
my $INPUT_MEM_FILE = "NULL";
my $weight_filename = "fft_0.weights";
my $input_filename = "fft.inputs";
my $import_input_file = 1;
my $PE_on = 0b0001;
my $FFT_NUM = $FFT_NUM_START; 
#print $FFT_NUM, "\n";
# my $PE0_I_MEM_START_BASE = 5504;
# my $PE0_O_MEM_START_BASE = 5504;
my $PE0_I_MEM_START_BASE = 128;
my $PE0_O_MEM_START_BASE = 128;
my $PE0_W_MEM_START_BASE = 0;
my @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $import_input_file, $PE_on, $FFT_NUM, $I_PREC, $W_PREC, $O_PREC, $PE0_I_MEM_START_BASE, $PE0_O_MEM_START_BASE, $PE0_W_MEM_START_BASE); 
FFT_GEN_WEIGHT(@argument);

my $iter;
my $iterations = int(log($FFT_NUM) / log(2));
#print $iterations, "\n";
$import_input_file = 0;
$INPUT_MEM_FILE = "FFT_INIT_MEM";
for ($iter = 1; $iter < $iterations + 1; $iter++) { 
  $weight_filename = join "", "fft_", $iter, ".weights";
  if (($w_exp != 8) and ($w_exp != 16)) {
    $PE0_W_MEM_START_BASE += ceil($input_size / $FFT_NUM / 2 * 2 * $w_exp / 96);   ## weight_size in memory
  } else {
    $PE0_W_MEM_START_BASE += ceil($input_size / $FFT_NUM / 2 * 2 * ($w_exp * 3 / 2) / 96);   ## zero padding 
  }
  $FFT_NUM /= 2;
  @argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $import_input_file, $PE_on, $FFT_NUM, $I_PREC, $W_PREC, $O_PREC, $PE0_I_MEM_START_BASE, $PE0_O_MEM_START_BASE, $PE0_W_MEM_START_BASE); 
 FFT_GEN_WEIGHT(@argument);
}

my $ii;
my $original;
my $copy;
system("rm dnn.inst.c");
$FFT_NUM = $FFT_NUM_START; 
$PE0_W_MEM_START_BASE = 0;
$inst = "FFT_0";
$weight_filename = "fft_0.weights";
$input_filename = "fft.inputs";
$output_filename = "fft_0.outputs";
@argument = ($inst, $INPUT_MEM_FILE, $weight_filename, $input_filename, $output_filename, $PE_on, $FFT_NUM, $I_PREC, $W_PREC, $O_PREC, $PE0_I_MEM_START_BASE, $PE0_O_MEM_START_BASE, $PE0_W_MEM_START_BASE); 
FFT_sub(@argument);
for ($iter = 1; $iter < $iterations + 1; $iter++) { 
  $inst = join "", "FFT_", $iter;
  $weight_filename = join "", "fft_", $iter, ".weights";
  if (($w_exp != 8) and ($w_exp != 16)) {
    $PE0_W_MEM_START_BASE += ceil($input_size / $FFT_NUM / 2 * 2 * $w_exp / 96);   ## weight_size in memory
  } else {
    $PE0_W_MEM_START_BASE += ceil($input_size / $FFT_NUM / 2 * 2 * ($w_exp * 3 / 2) / 96);   ## zero padding 
  }
  $input_filename = join "", "fft_", $iter - 1, ".outputs";
  $output_filename = join "", "fft_", $iter, ".outputs";
  $INPUT_MEM_FILE = join "", "FFT_", $iter - 1; 
  $FFT_NUM /= 2;
  @argument = ($inst, $INPUT_MEM_FILE . "_MEM", $weight_filename, $input_filename, $output_filename, $PE_on, $FFT_NUM, $I_PREC, $W_PREC, $O_PREC, $PE0_I_MEM_START_BASE, $PE0_O_MEM_START_BASE, $PE0_W_MEM_START_BASE); 
  print @argument, "\n";
  for ($ii = 0; $ii < 4; $ii++) { 
    $original = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".goldendata";
    $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
    system ("cp", $original, $copy); 
  }
  FFT_sub(@argument);
  for ($ii = 0; $ii < 4; $ii++) { 
    $copy = join "", $INPUT_MEM_FILE, "_MEM_", $ii, ".data";
    system ("rm", $copy); 
  }
}
