#!/usr/bin/perl

# Generate dnn_0_L4B1.mem for writing the whole XXX_INIT_MEM_0/1/2/3.data to PE_MEM
# version: generate the data in HEX code format, later used for MBUS memory stream, replace "xxxxxxxxxxxxxxxxxxxxxxxx" with "000000000000000000000000"

#use strict;
use warnings;
use POSIX ;
use File::Copy;

my $FILE = $ARGV[0];

open (DATA0, $FILE)
  or die "cannot open <", $FILE, ": $!";

my ($ext)= $FILE =~ /(\.[^.]+)$/;
my $filename = $FILE;
($filename) =~ s/$ext//ig; 
my ($PE_num) = $filename =~ /([0-9])$/; #bug: must use () for $PE_num and inside /../


my $NUM_LINES_L1 = 32; 
my $NUM_LINES_L2 = 128; 
my $NUM_LINES_L3 = 256; 
my $NUM_LINES_L4 = 1024; 

@LINES_DATA = <DATA0>;

my $LINES = 0;
my $NUM_LINES = 0;
my $DF;
my $p;
my $q;
my $r;
my $result_folder = "memory/";
my $data; 


$p = 4;
$NUM_LINES = $NUM_LINES_L4;
for ($q = 1; $q <=4; $q++){
	
	$DF = join "", $result_folder, "dnn_", $PE_num, "L4B", $q, ".mem";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	 
	
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; #bug: must drop the End of Line character
		if($data eq "xxxxxxxxxxxxxxxxxxxxxxxx") {	#bug: perl string comparison can't use == or !=
			$data = "000000000000000000000000";
		}
		printf DATA $data;
		print DATA "\n";
		$LINES++;
	}	
	
	close (DATA);
}


	#gen .mem file for L3
	$NUM_LINES	= $NUM_LINES_L3*4;
	$DF = join "", $result_folder, "dnn_", $PE_num, "L3.mem";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; #bug: must drop the End of Line character
		if($data eq "xxxxxxxxxxxxxxxxxxxxxxxx") {	#bug: perl string comparison can't use == or !=
			$data = "000000000000000000000000";
		}
		printf DATA $data;
		print DATA "\n";
		$LINES++;
	}	
	close (DATA);

	
	
	#gen .h file for L1L2
	$NUM_LINES	= $NUM_LINES_L1*4 + $NUM_LINES_L2*4;
	$DF = join "", $result_folder, "dnn_", $PE_num, "L1L2.mem";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; #bug: must drop the End of Line character
		if($data eq "xxxxxxxxxxxxxxxxxxxxxxxx") {	#bug: perl string comparison can't use == or !=
			$data = "000000000000000000000000";
		}
		printf DATA $data;
		print DATA "\n";
		$LINES++;
	}	
	close (DATA);


close (DATA0);