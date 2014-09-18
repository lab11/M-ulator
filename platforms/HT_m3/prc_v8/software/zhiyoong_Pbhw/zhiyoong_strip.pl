open (OUT, ">cdc_data_out.txt");
open (IN, "cdc_data_in.txt");
my $numlines = 0;
my $V_count = 0;

while ($line = <IN>)
{
    if($line =~ /V1P2/)
    {
	while ($V_count < 22){
	    print OUT $line;
	    $V_count = $V_count + 1;
	    $line = <IN>;
	}
	$V_count = 0;
    }
    if($line =~ /ADDR 0x50    DATA 0x00 0/)
    {
	@nlvars = split(' ',$line);
	if(@nlvars == 7) {
	    if ($numlines < 51){
		$numlines = $numlines + 1;
		print OUT $line;
	    }
	}
    }
    elsif($line =~ /Run/){
	while(($numlines<52) && ($numlines != 0)){
	    print OUT "N/A \n";
	    $numlines = $numlines + 1;
	}
	if ($numlines == 52){
	    $numlines = 0;
	}
    }
}
close IN;
print OUT "\n";

close OUT;
