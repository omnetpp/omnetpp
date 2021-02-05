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

   if (system("./eventlogtest $fileName backward >> $resultFileName") == 0)
   {
      print("PASS: Backward read on $fileName\n\n");
   }
   else
   {
      print("FAIL: Backward read on $fileName\n\n");
   }

   if (system("./eventlogtest $fileName $numberOfRandomReads $numberOfEventsToScan >> $resultFileName") == 0)
   {
      print("PASS: Random read on $fileName\n\n");
   }
   else
   {
      print("FAIL: Random read on $fileName\n\n");
   }
}

mkdir("result");

testEventLog("elog/predefined/simple/empty.elog", 5, 2);
testEventLog("elog/predefined/simple/one-event.elog", 5, 2);
testEventLog("elog/predefined/simple/two-events.elog", 10, 2);
testEventLog("elog/generated/stress.elog", 1000, 20);
