# detect MS Visual C version and return in the exit code

$prog = $ARGV[0];
$logo=`$prog 2>&1`;

# note: the regex below must match for internationalized versions of MSVC as well,
# so e.g. it cannot contain the words "bit" (as in "32-bit") or "version".
if ($logo =~ /Microsoft.*\s([0-9]+)\./) { 
  exit($1);
} else {
  exit(-1);
}
