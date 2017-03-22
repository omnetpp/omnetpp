$command = $ARGV[0];
$fname = $ARGV[1];
$variable = $ARGV[2];
$value = $ARGV[3];

$verbose=0;

open(INFILE, $fname) || die "cannot open $fname;";
defined read(INFILE, $config, 1000000) || die "cannot read $fname;";
close INFILE;

if ($command eq 'commentout')
{
    print "In $fname, commenting out $variable...\n" if $verbose;
    if ($config =~ /^# *$variable *=/m) {
        # OK, already commented out
    } elsif ($config =~ /^ *$variable *=/m) {
        $config =~ s/^ *$variable( *)=/#$variable\1=/mg;
    } else {
        die "$fname doesn't contain $variable;";
    }
}
elsif ($command eq 'uncomment')
{
    print "In $fname, uncommenting $variable...\n" if $verbose;
    if ($config =~ /^ *$variable *=/m) {
        # OK, not commented
    } elsif ($config =~ /^# *$variable *=/m) {
        $config =~ s/^# *$variable( *)=/$variable\1=/mg;
    } else {
        die "$fname doesn't contain $variable;";
    }
}
elsif ($command eq 'set')
{
    print "In $fname, setting $variable=$value...\n" if $verbose;
    if ($config =~ /^ *$variable *=/m) {
        # replace only 1st occurrence, to avoid messing up "CFLAGS=$(CFLAGS) /moreflags ..."
        $config =~ s/^ *$variable( *)=.*$/$variable\1=$value/m;
    } else {
        die "$fname doesn't contain $variable;";
    }
}
elsif ($command eq 'setoradd')
{
    print "In $fname, setting $variable=$value...\n" if $verbose;
    if ($config =~ /^ *$variable *=/m) {
        # replace only 1st occurrence, to avoid messing up "CFLAGS=$(CFLAGS) /moreflags ..."
        $config =~ s/^ *$variable( *)=.*$/$variable\1=$value/m;
    } else {
        $config .= "\n$variable=$value\n"
    }
}
elsif ($command eq 'assert')
{
    print "In $fname, asserting $variable=$value...\n" if $verbose;
    if ($config =~ /^ *$variable *\??= *(.*?) *$/m) {
        if ($1 ne $value) {die "$fname: wrong setting $variable=$1 (should be $value);";}
    } else {
        die "$fname doesn't contain $variable;";
    }
}
elsif ($command eq 'assert_contains')
{
    print "In $fname, asserting $variable=$value...\n" if $verbose;
    if ($config =~ /^ *$variable *\??= *(.*?) *$/m) {
        if (index($1,$value)) {die "$fname: wrong setting $variable=$1 (should be $value);";}
    } else {
        die "$fname doesn't contain $variable;";
    } 
}
else
{
    die "command $command not recognized, valid commands are commentout, uncomment, set, assert;";
}

open(OUTFILE, ">$fname") || die "cannot open $fname for write;";
print OUTFILE $config || die "cannot write $fname;";
close OUTFILE;
