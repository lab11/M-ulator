open (IN, "PRCv10_zhiyoong.hex");
open (OUT, ">PRCv10_zhiyoong.hex.mem");

@lines = <IN>;
chomp(@lines);
$i = 0;

while ($i < scalar(@lines))
{
	print OUT "$lines[$i+3]$lines[$i+2]$lines[$i+1]$lines[$i]";
	print OUT "\n";
	$i = $i + 4;
}

close OUT;
