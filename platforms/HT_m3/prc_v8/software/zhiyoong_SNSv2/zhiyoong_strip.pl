open (OUT, ">cdc_data_out.txt");
open (IN, "cdc_data_in.txt");
my $numlines = 0;

while ($line = <IN>)
{
    if($line =~ /ADDR 0x50    DATA 0x00 0/)
    {
	@nlvars = split(' ',$line);
	if(@nlvars == 7) {
	    $numlines = $numlines + 1;
	    print OUT $line;
	}
    }
    if ($line =~ /ADDR 0x50    DATA 0x07 00 00 04/){
	while($numlines<51){
#	while($numlines<201){
#	while($numlines<76){
	    print OUT $line;
	    $numlines = $numlines + 1;
	}
	$numlines = 0;
	print OUT $line;
    }
}
close IN;
print OUT "\n";


close OUT;
