sub testEventLogIndex
{
   my($fileName, $numberOfOffsetLookups) = @_;

   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//result\//;

   if (system("./eventlogindextest $fileName $numberOfOffsetLookups > $resultFileName") == 0)
   {
      print("PASS: Indexing on $fileName\n\n");
   }
   else
   {
      print("FAIL: Indexing on $fileName\n\n");
   }
}

mkdir("result");

testEventLogIndex("elog/predefined/simple/empty.elog", 1);
testEventLogIndex("elog/predefined/simple/one-event.elog", 5);
testEventLogIndex("elog/predefined/simple/two-events.elog", 10);
testEventLogIndex("elog/generated/stress.elog", 1000);
