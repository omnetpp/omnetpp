sub testEventLogIndex
{
   my($fileName, $numberOfOffsetLookups) = @_;

   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//results\//;

   if (system("eventlogindextest $fileName $numberOfOffsetLookups > $resultFileName") == 0)
   {
      print("PASS: Indexing on $fileName\n\n");
   }
   else
   {
      print("FAIL: Indexing on $fileName\n\n");
   }
}

mkdir("results");

testEventLogIndex("elog/empty.elog", 1);
testEventLogIndex("elog/one-event.elog", 5);
testEventLogIndex("elog/two-events.elog", 10);
testEventLogIndex("generated/stress.elog", 1000);
testEventLogIndex("filtered/stress-lines.elog", 1000);
testEventLogIndex("filtered/stress-events.elog", 1000);
