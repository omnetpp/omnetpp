$inputFile = shift;
$eventLogTool = "../../bin/eventlogtool";

$firstEventNumber = `grep "E #" $inputFile | head -n 1`;
$firstEventNumber =~ s/E # (.*?) .*\n/$1/;
$lastEventNumber = `grep "E #" $inputFile | tail -n 1`;
$lastEventNumber =~ s/E # (.*?) .*\n/$1/;

print("Testing log file $inputFile from event number $firstEventNumber to event number $lastEventNumber\n");

sub testReadLines()
{
  print("Testing readlines on $inputFile\n");

  system("$eventLogTool readlines $inputFile") == 0
    or die("*** Testing readlines on $inputFile failed");
}

sub testLoadEvents()
{
  print("Testing loadevents on $inputFile\n");

  system("$eventLogTool loadevents $inputFile") == 0
    or die("*** Testing loadevents on $inputFile failed");
}

sub testEcho1()
{
  print("Testing echo on $inputFile\n");

  print("  Echoing the whole input file\n");
  system("$eventLogTool echo -o tmp.log $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

  print("  Diffing output against original input file\n");
  system("diff tmp.log $inputFile > tmp.diff");
  unlink("tmp.log");

  if ((stat("tmp.diff"))[7] != 0)
  {
     die("*** Echo returned different content for $inputFile");
  }
  unlink("tmp.diff");
}

sub testEcho2()
{
  print("Testing echo on $inputFile\n");

  print("  Echoing the input file in two parts\n");
  system("$eventLogTool echo -o tmp1.log -te 100 $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

  system("$eventLogTool echo -oi -o tmp2.log -fe 101 $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

  print("  Diffing the concatenation of the outputs against the input file\n");
  system("cat tmp1.log tmp2.log > tmp.log");
  unlink("tmp1.log");
  unlink("tmp2.log");

  system("diff tmp.log $inputFile > tmp.diff");
  unlink("tmp.log");

  if ((stat("tmp.diff"))[7] != 0)
  {
     die("*** Echo returned different content for $inputFile");
  }
  unlink("tmp.diff");
}

sub testOffsets()
{
  print("Testing offsets on $inputFile\n");

  system("$eventLogTool offsets -e \"0 1 2 3\" -o tmp.log $inputFile") == 0
    or die("*** Testing offsets on $inputFile failed");
  unlink("tmp.log");

  print("  Looking up offset for event 0\n");
  `$eventLogTool offsets -e 0 $inputFile` != -1
    or die("*** First event not found in $inputFile");

  print("  Looking up offset for event 100\n");
  `$eventLogTool offsets -e 100 $inputFile` != -1
    or die("*** First event not found in $inputFile");

  print("  Looking up offset for an invalid event number\n");
  `$eventLogTool offsets -e -1 $inputFile` == -1
    or die("*** Invalid event number found in $inputFile");

  print("  Looking up offset for an event number not present in the file\n");
  `$eventLogTool offsets -e 10000000 $inputFile` == -1
    or die("*** Invalid event number found in $inputFile");
}

sub testEvents()
{
  print("Testing consistency of offsets and events on $inputFile\n");

  for ($i = 0; $i <= $lastEventNumber; $i++)
  {
    print("  Checking for event $i\n");
    $offset = `$eventLogTool offsets -e $i $inputFile`;
    $event = `$eventLogTool events -f $offset $inputFile`;

    if ($event !~ /E # $i/)
    {
      die("*** Event $i not found for offset $offset in $inputFile");
    }
  }
}

sub testConsistency()
{
  print("Testing consistency of causes and consequences on $inputFile\n");

  system("$eventLogTool consistency -o tmp.log $inputFile") == 0
    or die("*** Testing consistency on $inputFile failed");

  $length = (stat("tmp.log"))[7];

  if ($length != 0)
  {
    die("*** Causes and consequences are not consistent on $inputFile");
  }
}

sub testFilter()
{
  print("Testing filter on $inputFile\n");

  for ($i = 0; $i <= $lastEventNumber; $i++)
  {
    print("  Tracing event number $i in $inputFile");

    system("$eventLogTool filter -oi -e $i -o tmp.log $inputFile") == 0
      or die("*** Testing filter on $inputFile failed");

    $length = (stat("tmp.log"))[7];
    print(" resulted in $length bytes\n");

    `$eventLogTool offsets -e $i tmp.log` != -1
      or die("*** Traced event not found in filter result for $inputFile");
  }

  unlink("tmp.log");
}

testReadLines;
testLoadEvents;
testEcho1;
testEcho2;
testOffsets;
#testEvents;
testConsistency;
testFilter;
