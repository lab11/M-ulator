#!/usr/bin/perl -w

#Creates FLASH Memory Content for 2Mb Flash
#Possible Commands

open (IN, "$ARGV[0]") || die "Input File is Broken.\n";
open (OUT, ">$ARGV[0].2mb");

@line = <IN>;
$line_size = @line;
$prog_size = $line_size/4;
print $prog_size;
print "\n";
$prog_size_hex = sprintf("%04x",$prog_size);
print $prog_size_hex;
print "\n";

#Start address 0
print OUT "\@0\n";
#Header 6AB0C3CB
print OUT "C3CB\n";
print OUT "6AB0\n";
#MEM WR
print OUT "$prog_size_hex\n";
print OUT "1200\n";
#Destination Layer ID
print OUT "0001\n";
print OUT "0000\n";
#Destination Start Address
print OUT "0000\n";
print OUT "0000\n";
#Data(s)
@parity = (0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,0,
		   0,0,0,1);
for($i=0; $i<$line_size; $i=$i+4){
	chomp($line[$i+3]);
	chomp($line[$i+2]);
	chomp($line[$i+1]);
	chomp($line[$i+0]);
	$string0 = "$line[$i+1]$line[$i+0]";
	$string1 = "$line[$i+3]$line[$i+2]";
	print OUT "$string0\n";
	print OUT "$string1\n";
	$string_h = "$string1$string0";
	$string_d = hex($string_h);
	$string_b = sprintf("%032b",$string_d);
	@string_split = split(//,$string_b);
	print "Line Number: $i\n";
	print "string split\t: @string_split\n";
	for($j=0; $j<32; $j++){
		$parity[$j] = $parity[$j]^$string_split[$j];
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
print OUT "$parity[4]$parity[5]$parity[6]$parity[7]\n";
print OUT "$parity[0]$parity[1]$parity[2]$parity[3]\n";
#REG WR
print OUT "0001\n";
print OUT "1100\n";
#Write to RUN_CPU
#Address
print OUT "0001\n";
print OUT "0000\n";
#Data
print OUT "0001\n";
print OUT "0800\n";
#Parity
print OUT "0000\n";
print OUT "0800\n";

#TAIL0: Go To Idle
#1111_0000_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX
print OUT "0000\n";
print OUT "F000\n";

#TAIL0: Turn Off Flash
#1111_1111_XXXX_XXXX_XXXX_XXXX_XXXX_XXXX
#print OUT "0000\n";
#print OUT "FF00\n";
