$inputFile = shift;

$firstEventNumber = `grep "E #" omnetpp-nclients-tiny.log | head -n 1`;
$firstEventNumber =~ s/E # (.*?) .*\n/$1/;
$lastEventNumber = `grep "E #" omnetpp-nclients-tiny.log | tail -n 1`;
$lastEventNumber =~ s/E # (.*?) .*\n/$1/;

print("Testing log file $inputFile from event number $firstEventNumber to event number $lastEventNumber\n");

sub test1()
{
  print("Testing readlines on $inputFile\n");

  system("eventlogtool readlines $inputFile") == 0
    or die("*** Testing readlines on $inputFile failed");
}

sub test2()
{
  print("Testing loadevents on $inputFile\n");

  system("eventlogtool loadevents $inputFile") == 0
    or die("*** Testing loadevents on $inputFile failed");
}

sub test3()
{
  print("Testing echo on $inputFile\n");

  system("eventlogtool echo -o tmp.log $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

  system("diff tmp.log $inputFile > tmp.diff");
  unlink("tmp.log");

  if ((stat("tmp.diff"))[7] != 0)
  {
     die("*** Echo returned different content for $inputFile");
  }
  unlink("tmp.diff");
}

sub test4()
{
  print("Testing echo on $inputFile\n");
  system("eventlogtool echo -o tmp1.log -te 100 $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

  system("eventlogtool echo -oi -o tmp2.log -fe 101 $inputFile") == 0
    or die("*** Testing echo on $inputFile failed");

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

sub test5()
{
  print("Testing offsets on $inputFile\n");

  system("eventlogtool offsets -e \"0 1 2 3\" -o tmp.log $inputFile") == 0
    or die("*** Testing offsets on $inputFile failed");
  unlink("tmp.log");

  `eventlogtool offsets -e 0 $inputFile` != -1
    or die("*** First event not found in $inputFile");

  `eventlogtool offsets -e 100 $inputFile` != -1
    or die("*** First event not found in $inputFile");

  `eventlogtool offsets -e -1 $inputFile` == -1
    or die("*** Invalid event number found in $inputFile");

  `eventlogtool offsets -e 10000000 $inputFile` == -1
    or die("*** Invalid event number found in $inputFile");
}

sub test6()
{
  print("Testing filter on $inputFile\n");

  for ($i = 0; $i <= $lastEventNumber; $i++)
  {
    print("  Tracing event number $i in $inputFile");

    system("eventlogtool filter -oi -e $i -o tmp.log $inputFile") == 0
      or die("*** Testing filter on $inputFile failed");

    $length = (stat("tmp.log"))[7];
    print(" resulted in $length bytes\n");

    `eventlogtool offsets -e $i tmp.log` != -1
      or die("*** Traced event not found in filter result for $inputFile");
  }

  unlink("tmp.log");
}

test1;
test2;
test3;
test4;
test5;
test6;
