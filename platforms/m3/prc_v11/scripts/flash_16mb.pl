#!/usr/bin/perl -w

#Creates FLASH Memory Content for 2Mb Flash
#Possible Commands

open (IN, "$ARGV[0]") || die "Input File is Broken.\n";
open (OUT, ">$ARGV[0].16mb");

@line = <IN>;
$line_size = @line;
if ($line_size % 8 != 0){
	push @line, '00','00','00','00';
	$line_size = @line;
}
$prog_size = $line_size/4;
print $prog_size;
print "\n";
$prog_size_hex = sprintf("%04x",$prog_size);
print $prog_size_hex;
print "\n";

#Start address 0
print OUT "\@0\n";
#MEM WR + Header 6AB0C3CB
print OUT "1200";
print OUT "$prog_size_hex";
print OUT "6AB0";
print OUT "C3CB\n";
#Destination Start Address + Destination Layer ID
print OUT "0000";
print OUT "0000";
print OUT "0000";
print OUT "0001\n";
#Data(s)
@parity = (0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,1);
for($i=0; $i<$line_size; $i=$i+8){
	chomp($line[$i+7]);
	chomp($line[$i+6]);
	chomp($line[$i+5]);
	chomp($line[$i+4]);
	chomp($line[$i+3]);
	chomp($line[$i+2]);
	chomp($line[$i+1]);
	chomp($line[$i+0]);
	$string1 = "$line[$i+7]$line[$i+6]$line[$i+5]$line[$i+4]";
	$string0 = "$line[$i+3]$line[$i+2]$line[$i+1]$line[$i+0]";
	$string = "$line[$i+7]$line[$i+6]$line[$i+5]$line[$i+4]$line[$i+3]$line[$i+2]$line[$i+1]$line[$i+0]";
	print OUT "$string\n";
	$string_d1 = hex($string1);
	$string_d0 = hex($string0);
	$string_b1 = sprintf("%032b",$string_d1);
	$string_b0 = sprintf("%032b",$string_d0);
	@string_split1 = split(//,$string_b1);
	@string_split0 = split(//,$string_b0);
	print "Line Number: $i\n";
	print "string split\t: @string_split\n";
	for($j=0; $j<32; $j++){
		$parity[$j] = $parity[$j]^$string_split1[$j];
	}
	for($j=0; $j<32; $j++){
		$parity[$j] = $parity[$j]^$string_split0[$j];
	}
	print "parity\t\t: @parity\n";
	print "\n";
	
}

#Parity
$parity = join('',@parity);
print "parity $parity\n";
$parity = oct("0b".$parity);
print "parity $parity\n";
$parity = sprintf("%x",$parity);
print "parity $parity\n";
@parity = split('',$parity);
print "parity @parity\n";
chomp(@parity);
#REG WR + parity
print OUT "11000001";
print OUT "$parity\n";
#Write to RUN_CPU;
#Data + Address
print OUT "0800000100000001\n";
#Parity + tail go to IDLE
print OUT "F000000008000000\n";
