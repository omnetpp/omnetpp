$eventLogTool = "../../../bin/opp_eventlogtool";

sub testEchoWhole
{
   my($fileName) = @_;

   print("\nTesting echo on $fileName\n");

   print("  Echoing the whole input file\n");
   system("$eventLogTool echo -o result/tmp.elog $fileName") == 0
      or print("*** FAIL: Testing echo on $fileName\n");

   print("  Diffing output against original input file\n");
   system("diff result/tmp.elog $fileName > result/tmp.diff");
   unlink("result/tmp.elog");

   if ((stat("result/tmp.diff"))[7] != 0)
   {
      print("*** FAIL: Echo returned different content for $fileName\n");
   }
   else
   {
      print("PASS\n");
   }

   unlink("result/tmp.diff");
}

sub testEchoInTwoParts
{
   my($fileName) = @_;

   print("\nTesting echo on $fileName\n");

   print("  Echoing the input file in two parts\n");
   system("$eventLogTool echo -o result/tmp1.elog -te 100 $fileName") == 0
      or print("*** FAIL: Testing echo on $fileName failed\n");

   system("$eventLogTool echo -oi -o result/tmp2.elog -fe 101 $fileName") == 0
      or print("*** FAIL: Testing echo on $fileName failed\n");

   print("  Diffing the concatenation of the outputs against the input file\n");
   system("cat result/tmp1.elog result/tmp2.elog > result/tmp.elog");
   unlink("result/tmp1.elog");
   unlink("result/tmp2.elog");

   system("diff result/tmp.elog $fileName > result/tmp.diff");
   unlink("result/tmp.elog");

   if ((stat("result/tmp.diff"))[7] != 0)
   {
      print("*** FAIL: Echo returned different content for $fileName\n");
   }
   else
   {
      print("PASS\n");
   }

   unlink("result/tmp.diff");
}

sub testOffsets
{
   my($fileName) = @_;

   print("\nTesting offsets on $fileName\n");

   system("$eventLogTool offsets -e \"0 1 2 3\" -o result/tmp.elog $fileName") == 0
      or print("*** FAIL: Testing offsets on $fileName failed\n");
   unlink("result/tmp.elog");

   if ($lastEventNumber >= 0)
   {
      print("  Looking up offset for event 0\n");
      `$eventLogTool offsets -e 0 $fileName` != -1
         or print("*** FAIL: First event not found in $fileName\n");

      print("  Looking up offset for event $lastEventNumber\n");
      `$eventLogTool offsets -e $lastEventNumber $fileName` != -1
         or print("*** FAIL: Last event not found in $fileName\n");
   }

   print("  Looking up offset for an event number not present in the file\n");
   `$eventLogTool offsets -e 1000000000 $fileName` == -1
      or print("*** FAIL: Such a big event number found in $fileName?\n");

   print("PASS\n");
}

sub testEvents
{
   my($fileName, $lastEventNumber) = @_;

   print("\nTesting consistency of offsets and events on $fileName\n");

   for ($i = 0; $i <= $lastEventNumber; $i++)
   {
      print("  Checking for event $i in $fileName\n");
      $offset = `$eventLogTool offsets -e $i $fileName`;
      $offset =~ s/\n//;
      $event = `$eventLogTool events -f $offset $fileName`;

      if ($event !~ /E # $i/)
      {
         print("*** FAIL: Event $i not found for offset $offset in $fileName\n");
      }
   }

   print("PASS\n");
}

sub testFilter
{
   my($fileName, $lastEventNumber) = @_;

   print("\nTesting filter on $fileName\n");

   for ($i = 0; $i <= $lastEventNumber; $i++)
   {
      print("  Tracing event number $i in $fileName\n");

      $end = $i + 1000;
      system("$eventLogTool filter -oi -e $i -te $end -o result/tmp.elog $fileName") == 0
         or print("*** FAIL: Testing filter on $fileName failed\n");

      $length = (stat("result/tmp.elog"))[7];
      print("  resulted in $length bytes\n");

      `$eventLogTool offsets -e $i result/tmp.elog` != -1
         or print("*** FAIL: Traced event not found in filter result for $fileName\n");
   }

   print("PASS\n");
   unlink("result/tmp.elog");
}

sub testEventLogTool
{
   my($fileName) = @_;

   $lastEventNumber = `tail -n 1000 $fileName | grep "E #" | tail -n 1`;
   if (!($lastEventNumber =~ s/E # (.*?) .*\n/$1/))
   {
      $lastEventNumber = -1;
   }

   testEchoWhole($fileName);
   testEchoInTwoParts($fileName);
   testOffsets($fileName);
   testEvents($fileName, $lastEventNumber);
   testFilter($fileName, $lastEventNumber);
}

testEventLogTool("elog/predefined/simple/empty.elog");
testEventLogTool("elog/predefined/simple/one-event.elog");
testEventLogTool("elog/predefined/simple/two-events.elog");
testEventLogTool("elog/generated/stress.elog");
