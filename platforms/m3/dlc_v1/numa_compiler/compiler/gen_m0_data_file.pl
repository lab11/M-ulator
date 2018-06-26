#!/usr/bin/perl

# Generate dnn_0L4B1.h for writing the whole XXX_INIT_MEM_0/1/2/3.data to PE_MEM
# version: generate the data in a 2-D array format like PE_MEM[1025][4]([0]:address, [1]-[3]: 96-bit data), skip "xxxxxxxxxxxxxxxxxxxxxxxx"

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
my $text;
my $data; 
my $data_1; #bug: perl can't declare using my $data_1, $data_2, $data_3;
my $data_2;
my $data_3;

$p = 4;
$NUM_LINES	= $NUM_LINES_L4;
for ($q = 1; $q <=4; $q++){
	my $array_size=0;
	
	$DF = join "", $result_folder, "dnn_", $PE_num, "L4B", $q, ".h";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	 
	
	$text = "void set_dnn_data() {\n";
	printf DATA $text;
	
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; #bug: must drop the End of Line character
		if($data ne "xxxxxxxxxxxxxxxxxxxxxxxx") {	#bug: perl string comparison can't use == or !=
			$data_1 = substr $data,0,8; #bug: perl extract sub-string
			$data_2 = substr $data,8,8;
			$data_3 = substr $data,16,8;
			$text = join "", "PE_MEM[", $array_size, "][0] = ", $LINES, ";\n"; #bug: must have "", immediately after join
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][1] = 0x", $data_1, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][2] = 0x", $data_2, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][3] = 0x", $data_3, ";\n";
			printf DATA $text;
			$array_size++;
		}
		$LINES++;
	}	
	$text = "}\n\n";
	printf DATA $text;
	
	#to prevent PE_MEM[] to be size zero add one dummy elements in the end
	$array_size++;

	$text = join "", "uint32_t array_size=", $array_size, ";";
	printf DATA $text;
	
	close (DATA);
	
	
	#Insert PE_MEM array declaration at the top of the file
	open(M, "<$DF");
	@m=<M>;
	close(M);
	open(M, ">$DF");
	$text = join "", "uint32_t PE_MEM[", $array_size, "][4];\n\n";
	print M $text; 
	print M @m; #bug: Can't use printf!
	close(M);
}


	#gen .h file for L3
	$NUM_LINES	= $NUM_LINES_L3*4;
	$array_size=0;
	$DF = join "", $result_folder, "dnn_", $PE_num, "L3.h";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	$text = "void set_dnn_data() {\n";
	printf DATA $text;
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; 
		if($data ne "xxxxxxxxxxxxxxxxxxxxxxxx") {	
			$data_1 = substr $data,0,8; 
			$data_2 = substr $data,8,8;
			$data_3 = substr $data,16,8;
			$text = join "", "PE_MEM[", $array_size, "][0] = ", $LINES, ";\n"; 
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][1] = 0x", $data_1, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][2] = 0x", $data_2, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][3] = 0x", $data_3, ";\n";
			printf DATA $text;
			$array_size++;
		}
		$LINES++;
	}	
	$text = "}\n\n";
	printf DATA $text;
	$array_size++;
	$text = join "", "uint32_t array_size=", $array_size, ";";
	printf DATA $text;
	close (DATA);
	#Insert PE_MEM array declaration at the top of the file
	open(M, "<$DF");
	@m=<M>;
	close(M);
	open(M, ">$DF");
	$text = join "", "uint32_t PE_MEM[", $array_size, "][4];\n\n";
	print M $text; 
	print M @m;
	close(M);

	
	
	#gen .h file for L1L2
	$NUM_LINES	= $NUM_LINES_L1*4 + $NUM_LINES_L2*4;
	$array_size=0;
	$DF = join "", $result_folder, "dnn_", $PE_num, "L1L2.h";
	open (DATA, ">$DF")
     or die "cannot open <", $DF, ": $!";
	$text = "void set_dnn_data() {\n";
	printf DATA $text;
	for ($r = 0; $r < $NUM_LINES; $r++){
		($data) = $LINES_DATA[$LINES] =~ /(.*)/; 
		if($data ne "xxxxxxxxxxxxxxxxxxxxxxxx") {	
			$data_1 = substr $data,0,8; 
			$data_2 = substr $data,8,8;
			$data_3 = substr $data,16,8;
			$text = join "", "PE_MEM[", $array_size, "][0] = ", $LINES, ";\n"; 
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][1] = 0x", $data_1, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][2] = 0x", $data_2, ";\n";
			printf DATA $text;
			$text = join "", "PE_MEM[", $array_size, "][3] = 0x", $data_3, ";\n";
			printf DATA $text;
			$array_size++;
		}
		$LINES++;
	}	
	$text = "}\n\n";
	printf DATA $text;
	$array_size++;
	$text = join "", "uint32_t array_size=", $array_size, ";";
	printf DATA $text;
	close (DATA);
	#Insert PE_MEM array declaration at the top of the file
	open(M, "<$DF");
	@m=<M>;
	close(M);
	open(M, ">$DF");
	$text = join "", "uint32_t PE_MEM[", $array_size, "][4];\n\n";
	print M $text; 
	print M @m;
	close(M);


close (DATA0);

	

