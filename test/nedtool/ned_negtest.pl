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
$debug=0;

$isWindows = ($ENV{OS} =~ /windows/i) ? 1 : 0;
$shell='/bin/sh';  # only used on Unix

if (@ARGV == ())
{
   @filenames = glob("*.ned");
}
while (@ARGV)
{
    $arg = shift @ARGV;
    push(@filenames,glob($arg));
}

foreach $testfile (@filenames)
{
    print "testing $testfile\n" if ($debug);

    $outfile = $testfile.".out";
    $errfile = $testfile.".err";
    unlink($outfile);
    unlink($errfile);
    $status = exec_program ("$nedtool $testfile", $outfile, $errfile);
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
    if (!($txt =~ /\Q$pattern\E/s)) {
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

# args: command, stdout-file, stderr-file
# return: exit code, or -1 if program crashed
sub exec_program()
{
    my $cmd = shift;
    my $outfile = shift;
    my $errfile = shift;

    if ($isWindows)
    {
        print "  running \"$cmd >$outfile 2>$errfile\"\n" if ($debug);
        # The following line mysteriously fails to redirect on some Windows configuration.
        # This can be observed together with cvs reporting "editor session fails" -- root cause is common?
        #$status = system ("$cmd >$outfile 2>$errfile");
        $shell=$ENV{'COMSPEC'};
        if ($shell eq "") {
            print STDERR "$arg0: WARNING: no %COMSPEC% environment variable, using cmd.exe\n"; 
            $shell="cmd.exe";
        }
        # On the next line, cmd.exe may fail to pass back program exit code. 
        # When this happens, use the above (commented out) "system" line.
        $status = system($shell, split(" ", "/c $cmd >$outfile 2>$errfile"));
        print "  returned status = $status\n" if ($debug);
        if ($status == 0)
        {
            return 0;
        }
        elsif (256*int($status/256) != $status)
        {
            # this will never happen on Windows: if program doesn't exist, 256 is returned...
            return -1;
        }
        else
        {
            return $status/256;
        }

    }
    else
    {
        print "  running \"$shell -c '$cmd' >$outfile 2>$errfile\"\n" if ($debug);
        $status = system ("$shell -c '$cmd' >$outfile 2>$errfile");
        print "  returned status = $status\n" if ($debug);
        if ($status == 0)
        {
            return 0;
        }
        elsif (256*int($status/256) != $status)
        {
            return -1;
        }
        else
        {
            return $status/256;
        }
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

