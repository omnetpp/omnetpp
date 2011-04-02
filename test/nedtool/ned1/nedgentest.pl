eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

#
# Tool for testing NED generation feaure of nedtool
#

$nedtool='nedtool';

# nedtool has to be tested with both options
$args='-n -y -e';
#$args='-n -y';

$num_fail=0;
$num_unresolved=0;
$num_pass=0;

$verbose=1;
$debug=0;

# are we on Windows?
$isMINGW = defined $ENV{MSYSTEM} && $ENV{MSYSTEM} =~ /mingw/i;
$isWindows = ($ENV{'OS'} =~ /windows/i) ? 1 : 0;
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
    $status = exec_program ("$nedtool $args $testfile", $outfile, $errfile);
    if ($status ne '0') {
        fail($testfile, "nedtool returned exit code $status -- see $outfile and $errfile for details");
        next;
    }
    unlink($outfile);
    unlink($errfile);
    $ned = readfile($testfile);
    if ($ned eq '') {
        unresolved($testfile, "cannot read file $testfile");
        next;
    }

    $newnedfile = $testfile;
    $newnedfile =~ s/\.ned$/_n.ned/;

    $newned = readfile($newnedfile);
    if ($newned eq '') {
        unresolved($testfile, "cannot read file $newnedfile");
        next;
    }
    if ($ned ne $newned) {
        fail($testfile, "$testfile and $newnedfile differ");
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
        $shell = $ENV{'COMSPEC'};
        if ($shell eq "") {
            print STDERR "$arg0: WARNING: no %COMSPEC% environment variable, using cmd.exe\n";
            $shell = "cmd.exe";
        }
        # On the next line, cmd.exe may fail to pass back program exit code.
        # When this happens, use the above (commented out) "system" line.
        if ($isMINGW) {
            $status = system($shell, "/c","$cmd >$outfile 2>$errfile");
        } else {
            $status = system($shell, split(" ", "/c $cmd >$outfile 2>$errfile"));
        }
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

sub readfile()
{
    my $fname = shift;

    if (!open(IN,"$fname")) {
        return '';
    }
    # convert files to Unix style (with LF), discard trailing blank lines and spaces at end of lines
    $txt = '';
    while (<IN>)
    {
        s/[\r\n]*$//;
        s/ +$//;
        $txt.= $_."\n";
    }
    close IN;
    $txt=~s/[\r\n]*$//s;
    return $txt;
}

