sub filterLinesFromLogFile
{
   my($inFileName, $outFileName) = @_;

   open(IN, $inFileName);
   open(OUT, ">$outFileName");

   while (<IN>)
   {
      if (rand() < 0.5 || (/^$/))
      {
          print OUT;
      }
   }

   close(IN);
   close(OUT);
}

sub filterEventsFromLogFile
{
   my($inFileName, $outFileName) = @_;

   open(IN, $inFileName);
   open(OUT, ">$outFileName");

   while (<IN>)
   {
      if (/^E # (.*)$/)
      {
         $filterEvent = rand() < 0.5;
      }

      if (!($filterEvent))
      {
         print OUT;
      }
   }

   close(IN);
   close(OUT);
}

sub filterLogFileWithTool
{
   # TODO: this would call the eventlogtool to filter the file
}

mkdir("elog/filtered");

filterLinesFromLogFile("elog/generated/stress.elog", "elog/filtered/stress-lines.elog");
filterEventsFromLogFile("elog/generated/stress.elog", "elog/filtered/stress-events.elog");
