#!/usr/bin/perl;
#use strict;
use warnings;
use POSIX ;
use File::Copy;

my $PREFIX = $ARGV[0];
if ($PREFIX ne "NLI" && $PREFIX ne "MOV" && $PREFIX ne "FFT" && $PREFIX ne "MAC" && $PREFIX ne "MACNLI"){
	die sprintf("WRONG PREFIX = %s", $ARGV[0]);
}
copy "../V1/$PREFIX.inst", "../V2/V2.inst";

my $DF0 = join "", $PREFIX, "_MEM_0.data";
open (DATA0, "../V1/$DF0");
my $DF1 = join "", $PREFIX, "_MEM_1.data";
open (DATA1, "../V1/$DF1");
my $DF2 = join "", $PREFIX, "_MEM_2.data";
open (DATA2, "../V1/$DF2");
my $DF3 = join "", $PREFIX, "_MEM_3.data";
open (DATA3, "../V1/$DF3");
print "$DF0\n";
print "$DF1\n";
print "$DF2\n";
print "$DF3\n";

my $GF0 = join "", $PREFIX, "_MEM_0.goldendata";
open (GOLD0, "../V1/$GF0");
my $GF1 = join "", $PREFIX, "_MEM_1.goldendata";
open (GOLD1, "../V1/$GF1");
my $GF2 = join "", $PREFIX, "_MEM_2.goldendata";
open (GOLD2, "../V1/$GF2");
my $GF3 = join "", $PREFIX, "_MEM_3.goldendata";
open (GOLD3, "../V1/$GF3");
print "$GF0\n";
print "$GF1\n";
print "$GF2\n";
print "$GF3\n";

my $NUM_LINES_L1 = 32; 
my $NUM_LINES_L2 = 128; 
my $NUM_LINES_L3 = 256; 
my $NUM_LINES_L4 = 1024; 

@LINES_DATA0 = <DATA0>;
@LINES_DATA1 = <DATA1>;
@LINES_DATA2 = <DATA2>;
@LINES_DATA3 = <DATA3>;
@LINES_GOLD0 = <GOLD0>;
@LINES_GOLD1 = <GOLD1>;
@LINES_GOLD2 = <GOLD2>;
@LINES_GOLD3 = <GOLD3>;

my $LINES = 0;
my $NUM_LINES = 0;
my $DF;
my $GF;
my $p;
my $q;
my $r;

$LINES = 0;
$PREFIX = "MEM_0_";
for ($p = 4; $p >= 1; $p--){
	if ($p == 4) 	{$NUM_LINES	= $NUM_LINES_L4;  }
	if ($p == 3) 	{$NUM_LINES	= $NUM_LINES_L3;  }
	if ($p == 2) 	{$NUM_LINES	= $NUM_LINES_L2;  }
	if ($p == 1) 	{$NUM_LINES	= $NUM_LINES_L1;  }
	for ($q = 1; $q <=4; $q++){
		$DF = join "", $PREFIX, "L", $p, "B", $q, ".data";	
		$GF = join "", $PREFIX, "L", $p, "B", $q, ".goldendata";
		#print "$DF\n";
		#print "$GF\n";
		open (DATA, ">../V2/$DF");
		open (GOLD, ">../V2/$GF");
		for ($r = 0; $r < $NUM_LINES; $r++){
			printf DATA $LINES_DATA0[$LINES];
			printf GOLD $LINES_GOLD0[$LINES];
			$LINES++;
		}
		close (DATA);
		close (GOLD);
	}
}

$LINES = 0;
$PREFIX = "MEM_1_";
for ($p = 4; $p >= 1; $p--){
	if ($p == 4) 	{$NUM_LINES	= $NUM_LINES_L4;  }
	if ($p == 3) 	{$NUM_LINES	= $NUM_LINES_L3;  }
	if ($p == 2) 	{$NUM_LINES	= $NUM_LINES_L2;  }
	if ($p == 1) 	{$NUM_LINES	= $NUM_LINES_L1;  }
	for ($q = 1; $q <=4; $q++){
		$DF = join "", $PREFIX, "L", $p, "B", $q, ".data";	
		$GF = join "", $PREFIX, "L", $p, "B", $q, ".goldendata";
		#print "$DF\n";
		#print "$GF\n";
		open (DATA, ">../V2/$DF");
		open (GOLD, ">../V2/$GF");
		for ($r = 0; $r < $NUM_LINES; $r++){
			printf DATA $LINES_DATA1[$LINES];
			printf GOLD $LINES_GOLD1[$LINES];
			$LINES++;
		}
		close (DATA);
		close (GOLD);
	}
}

$LINES = 0;
$PREFIX = "MEM_2_";
for ($p = 4; $p >= 1; $p--){
	if ($p == 4) 	{$NUM_LINES	= $NUM_LINES_L4;  }
	if ($p == 3) 	{$NUM_LINES	= $NUM_LINES_L3;  }
	if ($p == 2) 	{$NUM_LINES	= $NUM_LINES_L2;  }
	if ($p == 1) 	{$NUM_LINES	= $NUM_LINES_L1;  }
	for ($q = 1; $q <=4; $q++){
		$DF = join "", $PREFIX, "L", $p, "B", $q, ".data";	
		$GF = join "", $PREFIX, "L", $p, "B", $q, ".goldendata";
		#print "$DF\n";
		#print "$GF\n";
		open (DATA, ">../V2/$DF");
		open (GOLD, ">../V2/$GF");
		for ($r = 0; $r < $NUM_LINES; $r++){
			printf DATA $LINES_DATA2[$LINES];
			printf GOLD $LINES_GOLD2[$LINES];
			$LINES++;
		}
		close (DATA);
		close (GOLD);
	}
}

$LINES = 0;
$PREFIX = "MEM_3_";
for ($p = 4; $p >= 1; $p--){
	if ($p == 4) 	{$NUM_LINES	= $NUM_LINES_L4;  }
	if ($p == 3) 	{$NUM_LINES	= $NUM_LINES_L3;  }
	if ($p == 2) 	{$NUM_LINES	= $NUM_LINES_L2;  }
	if ($p == 1) 	{$NUM_LINES	= $NUM_LINES_L1;  }
	for ($q = 1; $q <=4; $q++){
		$DF = join "", $PREFIX, "L", $p, "B", $q, ".data";	
		$GF = join "", $PREFIX, "L", $p, "B", $q, ".goldendata";
		#print "$DF\n";
		#print "$GF\n";
		open (DATA, ">../V2/$DF");
		open (GOLD, ">../V2/$GF");
		for ($r = 0; $r < $NUM_LINES; $r++){
			printf DATA $LINES_DATA3[$LINES];
			printf GOLD $LINES_GOLD3[$LINES];
			$LINES++;
		}
		close (DATA);
		close (GOLD);
	}
}

close (DATA0);
close (DATA1);
close (DATA2);
close (DATA3);
close (GOLD0);
close (GOLD1);
close (GOLD2);
close (GOLD3);
