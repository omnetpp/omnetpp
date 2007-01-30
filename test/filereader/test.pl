$fileReaderTest = 'filereadertest';

sub test
{
   my($fileName, $numberOfLines, $numberOfSeeks, $numberOfReadLines) = @_;

   print("Testing $fileName...\n");

   if (system("$fileReaderTest $fileName $numberOfLines $numberOfSeeks $numberOfReadLines > result/$fileName") == 0)
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
   if (int(rand(10)) == 0)
   {
      return "";
   }
   else
   {
      @chars = ( "A" .. "Z", "a" .. "z", 0 .. 9);
      return join("", @chars[ map { rand @chars } ( 1 .. int(rand(100))) ]);
   }
}

sub generate
{
   my($fileName, $fileSize) = @_;

   $numberOfLines = 0;
   open(OUT, ">$fileName");

   while ($fileSize > 0)
   {
      print OUT "$numberOfLines ";
      $line = generateContent();
      $fileSize -= length($line);
      $numberOfLines++;
      print OUT "$line\n";
   }

   close(OUT);

   return $numberOfLines;
}

sub generateAndTest
{
   my($fileName, $fileSize, $numberOfSeeks, $numberOfReadLines) = @_;

   test($fileName, generate($fileName, $fileSize), $numberOfSeeks, $numberOfReadLines);
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

generateAndTest("generated/tiny.txt",   1E+2, 100, 100);
generateAndTest("generated/small.txt",  1E+4, 100, 100);
generateAndTest("generated/medium.txt", 1E+6, 1000, 1000);
generateAndTest("generated/large.txt",  1E+8, 1000, 1000);

# uncomment this if you want to test it with GByte files
#generateAndTest("huge.txt",   1E+10, 100, 100);

