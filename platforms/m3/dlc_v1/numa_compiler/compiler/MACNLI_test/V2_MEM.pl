#!/usr/bin/perl
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

for ($p = 4; $p >= 1; $p--){
	if ($p == 4) 	{$NUM_LINES	= $NUM_LINES_L4;  }
	if ($p == 3) 	{$NUM_LINES	= $NUM_LINES_L3;  }
	if ($p == 2) 	{$NUM_LINES	= $NUM_LINES_L2;  }
	if ($p == 1) 	{$NUM_LINES	= $NUM_LINES_L1;  }
	for ($q = 1; $q <=4; $q++){
		$DF = join "", $result_folder, $filename, "L", $p, "B", $q, $ext;
		open (DATA, ">$DF")
      or die "cannot open <", $DF, ": $!";
		for ($r = 0; $r < $NUM_LINES; $r++){
			printf DATA $LINES_DATA[$LINES];
			$LINES++;
		}
		close (DATA);
	}
}

close (DATA0);
