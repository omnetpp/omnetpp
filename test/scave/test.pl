# compares the content of text file
sub matchFiles
{
   my($fileName1, $fileName2) = @_;
   local($line1, $line2);

   open(FILE1, $fileName1) || return 0;
   open(FILE2, $fileName2) || return 0;

   while (<FILE1>)
   {
      $line1 = $_;
      $line2 = <FILE2>;

      if (!($line1 eq $line2))
      {
         return 0;
      }
   }

   close(FILE1);
   close(FILE2);

   return 1;
}

sub test
{
   my($testname, $fileName) = @_;
   local($suffix, $resultFileName,$expectedResultFileName);

   print("Testing $fileName...\n");
   $suffix = "v";
   if ($testname eq "reader-builder") {
     $suffix = "a";
   }
   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\.(.*)/\1_$suffix\.\2/;
   $expectedResultFileName = $resultFileName;
   $resultFileName =~ s/^(.*)\//result\//;
   $expectedResultFileName =~ s/^(.*)\//expected\//;
   if (system("test $testname $fileName $resultFileName") == 0  && matchFiles($resultFileName, $expectedResultFileName))
   {
      print("PASS: Reader test on $fileName\n\n");
   }
   else
   {
      print("FAIL: Reader test on $fileName\n\n");
   }
}


mkdir("result");

test("reader-builder", "testfiles/omnetpp1.vec");
test("reader-builder", "testfiles/simtime_test.vec");

test("reader-writer", "testfiles/omnetpp1.vec");
test("reader-writer", "testfiles/simtime_test.vec");

