sub testEventLog
{
   my($fileName, $numberOfRandomReads, $numberOfEventsToScan) = @_;

   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//result\//;

   if (system("./eventlogtest $fileName forward > $resultFileName") == 0)
   {
      print("PASS: Forward read on $fileName\n\n");
   }
   else
   {
      print("FAIL: Forward read on $fileName\n\n");
   }

   if (system("./eventlogtest $fileName backward > $resultFileName") == 0)
   {
      print("PASS: Backward read on $fileName\n\n");
   }
   else
   {
      print("FAIL: Backward read on $fileName\n\n");
   }

   if (system("./eventlogtest $fileName $numberOfRandomReads $numberOfEventsToScan > $resultFileName") == 0)
   {
      print("PASS: Random read on $fileName\n\n");
   }
   else
   {
      print("FAIL: Random read on $fileName\n\n");
   }
}

mkdir("result");

testEventLog("log/empty.log", 5, 2);
testEventLog("log/one-event.log", 5, 2);
testEventLog("log/two-events.log", 10, 2);
testEventLog("generated/stress.log", 1000, 20);

testEventLog("filtered/stress-events.log", 1000, 20);
