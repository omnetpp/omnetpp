$fileReaderTest = 'filereadertest';

sub test
{
   my($fileName, $numberOfLines, $numberOfSeeks, $numberOfReadLines) = @_;

   print("Testing $fileName...\n");
   $resultFileName = $fileName;
   $resultFileName =~ s/^(.*)\//result\//;

   if (system("$fileReaderTest $fileName $numberOfLines $numberOfSeeks $numberOfReadLines > $resultFileName") == 0)
   {
      print("PASS: $fileName\n\n");
   }
   else
   {
      print("FAIL: $fileName\n\n");
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
      return join("", @chars[ map { rand @chars } ( 1 .. int(rand($maxLineSize))) ]);
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
      print OUT "$numberOfLines ";
      $line = generateContent($maxLineSize);
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

mkdir("generated");
mkdir("result");

test("text/empty.txt", 0, 1, 10);
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
generateAndTest("generated/medium-big-lines.txt", 1E+7, 32768, 1000, 1000);
generateAndTest("generated/large-big-lines.txt",  1E+8, 32768, 1000, 1000);

# uncomment this if you want to test it with GByte files
#generateAndTest("huge-big-lines.txt",   5E+9, 32768, 100, 100);
