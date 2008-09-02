# detect MS Visual C version and return in the exit code

$prog = $ARGV[0];
$logo=`$prog 2>&1`;

if ($logo =~ /Microsoft.*32-bit.*Version ([0-9]+)\./) {
  exit($1);
} else {
  exit(-1);
}
