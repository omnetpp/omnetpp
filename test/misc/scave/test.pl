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

sub testReader
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

sub testExport
{
  my($fileName, $format) = @_;

  print("Testing $fileName...\n");
  $resultFileName = $fileName;
  $resultFileName =~ s/^(.*)\.(.*)/\1\.$format/;
  $expectedResultFileName = $resultFileName;
  $resultFileName =~ s/^(.*)\//result\//;
  $expectedResultFileName =~ s/^(.*)\//expected\//;
  $command = "vector";
  $command = "scalar" if ($fileName =~ /(.*)\.sca$/);

  if (system("../../bin/opp_scavetool.exe $command -O $resultFileName -F $format $fileName") == 0 && matchFiles($resultFileName, $expectedResultFileName))
  {
    print("PASS: Exporting $fileName in $format\n");
  }
  else
  {
    print("FAIL: Exporting $fileName in $format\n");
  }
}

sub generateVectorFile
{
  my($fileName,$numOfVectors,$numOfLines,$linesPerBlock) = @_;
  local ($simtime,$vectorId,$lineCount,$value);

  if (-e $fileName) {
    return;
  }

  open (OUT, ">$fileName") || die "Can not open file '$fileName' for output";

  $simtime = 0.0;
  $vectorId = 1;
  $lineCount = 0;

  HEADER:
  for ($id = 1; $id <= $numOfVectors; $id++) {
    print OUT "vector $id\t\"module\"\t\"vector $id\"\t\"TV\"\n";
  }

  DATA:
  while (1) {
    for ($id = 1; $id <= $numOfVectors; $id++) {
      for ($j = 0; $j < $linesPerBlock; $j++) {
        last DATA if ($lineCount >= $numOfLines);
        $value = rand();
        print OUT "$id\t$simtime\t$value\n";
        $lineCount++;
        $simtime += 0.1;
      }
    }
  }

  close(OUT);
}

sub testIndexer
{
  my($fileName) = @_;
  local($indexFileName);

  $indexFileName = $fileName;
  $indexFileName =~ s/^(.*)\.(.*)$/\1\.vci/;

  if (-e $indexFileName) {
    unlink $indexFileName;
  }

  print("Testing indexer on $fileName...\n");
  $expectedResultFileName = $indexFileName;
  $expectedResultFileName =~ s/^(.*)\//expected\//;

  if (system("test indexer $fileName") == 0  &&
      (!(-e $expectedResultFileName) || matchFiles($resultFileName, $expectedResultFileName)))
  {
     print("PASS: Indexer test on $fileName\n\n");
  }
  else
  {
     print("FAIL: Indexer test on $fileName\n\n");
  }
}

sub testReader2
{
  my($readerName, $fileName, $vectors) = @_;

  print("Testing $readerName on $fileName...\n");

  if (system("test $readerName $fileName $vectors") == 0)
  {
     print("PASS: $readerName test on $fileName\n\n");
  }
  else
  {
     print("FAIL: $readerName test on $fileName\n\n");
  }

}




mkdir("result");

testReader("reader-builder", "testfiles/omnetpp1.vec");
testReader("reader-builder", "testfiles/simtime_test.vec");

testReader("reader-writer", "testfiles/omnetpp1.vec");
testReader("reader-writer", "testfiles/simtime_test.vec");

testExport("testfiles/scalars.sca", "matlab");
testExport("testfiles/scalars.sca", "octave");
testExport("testfiles/scalars.sca", "csv");
testExport("testfiles/vectors.vec", "matlab");
testExport("testfiles/vectors.vec", "octave");
testExport("testfiles/vectors.vec", "csv");

generateVectorFile("testfiles/big.vec", 1000, 10000000, 10000);
testIndexer("testfiles/big.vec");
testReader2("indexedvectorfilereader", "testfiles/big.vec", "100,200,300,400,500,600,700,800,900,1000");
testReader2("vectorfilereader", "testfiles/big.vec", "100,200,300,400,500,600,700,800,900,1000");

