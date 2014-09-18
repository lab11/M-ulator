#!/usr/bin/perl
my $testname	= $ARGV[0];

open IN, "$testname.hex";
open OUT, ">$testname.v";
my $i = 0;
my $j = 0;

while ($line = <IN>){
    if (($i%4) == 0){
	@data_0 = split(' ',$line);
    }
    elsif (($i%4) == 1){
	@data_1 = split(' ',$line);
    }
    elsif (($i%4) == 2){
	@data_2 = split(' ',$line);
    }
    elsif (($i%4) == 3){
	@data_3 = split(' ',$line);
    }
    if (($i%4) == 3){
    	print OUT "memory[$j] = 32'h";
	print OUT $data_3[0];
	print OUT $data_2[0];
	print OUT $data_1[0];
	print OUT $data_0[0];
	print OUT ";\n";
	$j=$j+1;
    }
    $i=$i+1;
}

close(IN);
close(OUT);


