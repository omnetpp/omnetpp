eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

#
# Tool for testing error-detection capabilities in nedtool
#

$nedtool='nedtool';
$num_fail=0;
$num_unresolved=0;
$num_pass=0;

$verbose=1;

$isWindows = ($ENV{OS} =~ /windows/i) ? 1 : 0;

@filenames = @ARGV;
if (@filenames==()) {
   @filenames = glob("*.ned");
}

foreach $testfile (@filenames)
{
    print "testing $testfile\n" if ($debug);

    $errfile = $testfile.".err";
    unlink($errfile);
    $status = runprog ($nedtool, $testfile, "2>$errfile");
    if ($status eq '0') {
        fail($testfile, "nedtool failed to detect error in NED file (exit code 0)");
        next;
    }
    if (!open(IN,"$errfile")) {
        unresolved($testfile, "cannot read error output file `$errfile'");
        next;
    }
    $txt = '';
    while (<IN>)
    {
        s/[\r\n]*$//;
        $txt.= $_."\n";
    }
    close IN;
    $txt=~s/[\r\n]*$//s;

    if (!open(IN,"$testfile")) {
        unresolved($testfile, "cannot read test NED file");
        next;
    }
    $pattern = "";
    while (<IN>)
    {
        s/[\r\n]*$//;
        if (/^\/\/ *ERROR:(.*)/) {
            $pattern = $1;
            break;
        }
    }
    close IN;
    if ($pattern eq '') {
        unresolved($testfile, "no '//ERROR:' line in NED file");
        next;
    }

    $pattern =~ s/^\s*(.*?)\s*$/\1/s; # trim pattern

    # check contains or not-contains
    if (!($txt =~ /$pattern/s)) {
        fail($testfile, "different error message");
        print "expected output: >>>$pattern<<<\nactual output: >>>$txt<<<\n" if ($verbose);
        next;
    }
    pass($testfile);
}

print "========================================\n";
print "PASS: $num_pass   FAIL: $num_fail   UNRESOLVED: $num_unresolved\n";

if ($num_fail>0 && $verbose) {
    print "FAILED tests: ".join(' ', @failed_tests)."\n";
}
if ($num_unresolved>0 && $verbose) {
    print "UNRESOLVED tests: ".join(' ', @unresolved_tests)."\n";
}

sub runprog()
{
    my @prog = @_;
    print "running: ".join(' ',@prog)."\n" if $debug;
    if ($isWindows) {
        system($ENV{COMSPEC}, "/c", @prog);
    } else {
        system(@prog);
    }
}

sub unresolved()
{
    my $testname = shift;
    my $reason = shift;
    $num_unresolved++;
    push (@unresolved_tests, $testname);
    $result{$testname} = 'UNRESOLVED';
    $reason{$testname} = $reason;
    print "*** $testname: UNRESOLVED ($reason)\n";
}

sub fail()
{
    my $testname = shift;
    my $reason = shift;
    $num_fail++;
    push (@failed_tests, $testname);
    $result{$testname} = 'FAIL';
    $reason{$testname} = $reason;
    print "*** $testname: FAIL ($reason)\n";
}

sub pass()
{
    my $testname = shift;
    $num_pass++;
    $result{$testname} = 'PASS';
    $reason{$testname} = '';
    print "*** $testname: PASS\n";
}

