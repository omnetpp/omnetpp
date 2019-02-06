my $progdir = "./";

sub matchFiles
{
   my($fileName1, $fileName2) = @_;

   open(FILE1, $fileName1);
   open(FILE2, $fileName2);

   while (<FILE2>)
   {
      if (!($_ eq <FILE1>))
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
   my($fileName, $numberOfLines, $numberOfSeeks, $numberOfReadLines) = @_;

   print("Testing $fileName...\n");
   $resultFileName = $fileName;
   $forwardResultFileName = $fileName;
   $backwardResultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//results\//;
   $forwardResultFileName =~ s/^(.*)\//results\/forward-/;
   $backwardResultFileName =~ s/^(.*)\//results\/backward-/;

   if (system("${progdir}fileechotest $fileName forward > $forwardResultFileName") == 0 && matchFiles($fileName, $forwardResultFileName))
   {
      print("PASS: Forward echoing $fileName\n\n");
   }
   else
   {
      print("FAIL: Forward echoing $fileName\n\n");
   }

   if (system("${progdir}fileechotest $fileName backward > $backwardResultFileName") == 0 && matchFiles($fileName, $forwardResultFileName))
   {
      print("PASS: Backward echoing $fileName\n\n");
   }
   else
   {
      print("FAIL: Backward echoing $fileName\n\n");
   }

   if (system("${progdir}filereadertest $fileName $numberOfLines $numberOfSeeks $numberOfReadLines > $resultFileName") == 0)
   {
      print("PASS: Reader test on $fileName\n\n");
   }
   else
   {
      print("FAIL: Reader test on $fileName\n\n");
   }
}

sub generateContent
{
   my($maxLineSize) = @_;

   if (int(rand(10)) == 0)
   {
      return "";
   }
   else
   {
      @chars = ( "A" .. "Z", "a" .. "z", 0 .. 9);
      return join("", @chars[ map { rand @chars } ( 1 .. int(rand($maxLineSize - 1))) ]);
   }
}

sub generate
{
   my($fileName, $fileSize, $maxLineSize) = @_;

   print("Generating $fileName...\n");
   $numberOfLines = 0;
   open(OUT, ">$fileName");

   while ($fileSize > 0)
   {
      $lineNumber = "$numberOfLines ";
      print OUT $lineNumber;
      $line = generateContent($maxLineSize - length($lineNumber));
      $fileSize -= length($line);
      $numberOfLines++;
      print OUT "$line\n";
   }

   close(OUT);

   return $numberOfLines;
}

sub generateAndTest
{
   my($fileName, $fileSize, $maxLineSize, $numberOfSeeks, $numberOfReadLines) = @_;

   test($fileName, generate($fileName, $fileSize, $maxLineSize), $numberOfSeeks, $numberOfReadLines);
}

sub concurrentTest
{
   my($fileName, $duration, $numberOfLines) = @_;
   
   my $pid = fork();
   if (not defined $pid) {
      print "resources not avilable.\n";
   } elsif ($pid == 0) {
      # child process
      if (system("${progdir}filereaderproducer $fileName $duration $numberOfLines") != 0)
      {
         print("FAIL: Cannot start filereaderproducer with $fileName\n\n");
      }
      exit(0);
   } else {
      # parent process
      if (system("${progdir}filereaderconsumer $fileName $duration") == 0)
      {
         print("PASS: Concurrent reader test on $fileName\n\n");
      }
      else
      {
         print("FAIL: Concurrent reader test on $fileName\n\n");
      }
      
      waitpid($pid,0);
   }
}

mkdir("generated");
mkdir("results");

test("text/empty.txt", 0, 10, 10);
test("text/one-character.txt", 0, 10, 10);
test("text/one-big-line.txt", 0, 10, 10);
test("text/one-big-line-with-new-line-at-the-end.txt", 1, 10, 10);
test("text/one-line.txt", 0, 10, 10);
test("text/one-line-with-new-line-at-the-end.txt", 1, 10, 10);
test("text/ten-new-lines.txt", 10, 10, 20);
test("text/two-big-lines.txt", 1, 10, 10);
test("text/two-big-lines-with-new-line-at-the-end.txt", 2, 10, 10);
test("text/two-lines.txt", 1, 10, 10);
test("text/two-lines-with-new-line-at-the-end.txt", 2, 10, 10);

generateAndTest("generated/tiny-small-lines.txt",   1E+5, 1000, 100, 100);
generateAndTest("generated/small-small-lines.txt",  1E+6, 1000, 100, 100);
generateAndTest("generated/medium-small-lines.txt", 1E+7, 1000, 1000, 1000);
generateAndTest("generated/large-small-lines.txt",  1E+8, 1000, 1000, 1000);

generateAndTest("generated/tiny-big-lines.txt",   1E+5, 32768, 100, 100);
generateAndTest("generated/small-big-lines.txt",  1E+6, 32768, 100, 100);
generateAndTest("generated/medium-big-lines.txt", 1E+7, 32768, 100, 100);
generateAndTest("generated/large-big-lines.txt",  1E+8, 32768, 100, 100);

# uncomment this if you want to test it with GByte files
#generateAndTest("generated/huge-big-lines.txt",   5E+9, 32768, 100, 100);

concurrentTest("results/concurrent_small.txt", 1, 100);
concurrentTest("results/concurrent_large.txt", 10, 10000);
