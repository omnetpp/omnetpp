sub testEventLogIndex
{
   my($fileName, $numberOfOffsetLookups) = @_;

   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//result\//;

   if (system("eventlogindextest $fileName $numberOfOffsetLookups > $resultFileName") == 0)
   {
      print("PASS: Indexing on $fileName\n\n");
   }
   else
   {
      print("FAIL: Indexing on $fileName\n\n");
   }
}

mkdir("result");

testEventLogIndex("log/empty.log", 1);
testEventLogIndex("log/one-event.log", 5);
testEventLogIndex("log/two-events.log", 10);
testEventLogIndex("generated/stress.log", 1000);
testEventLogIndex("filtered/stress-lines.log", 1000);
testEventLogIndex("filtered/stress-events.log", 1000);
