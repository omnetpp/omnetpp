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
   my($fileName,$outtype) = @_;
   local($resultFileName,$expectedResultFileName);

   print("Testing $fileName...\n");
   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\.(.*)/\1_$outtype\.\2/;
   $expectedResultFileName = $resultFileName;
   $resultFileName =~ s/^(.*)\//result\//;
   $expectedResultFileName =~ s/^(.*)\//expected\//;
   if (system("vectorfilereadertest $fileName $resultFileName $outtype") == 0  && matchFiles($resultFileName, $expectedResultFileName))
   {
      print("PASS: Reader test on $fileName\n\n");
   }
   else
   {
      print("FAIL: Reader test on $fileName\n\n");
   }
}


mkdir("result");

test("testfiles/omnetpp1.vec", "a");
test("testfiles/simtime_test.vec", "a");

test("testfiles/omnetpp1.vec", "v");
test("testfiles/simtime_test.vec", "v");
