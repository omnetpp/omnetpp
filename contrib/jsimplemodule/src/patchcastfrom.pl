#!/usr/bin/perl
#
# Implement transfer of ownership into cast()
#

$verbose = 0;

$dir = $ARGV[0];
die "no directory specified" if ($dir eq '');

chdir $dir || die "cannot cd to $dir";
foreach $fname (glob("*.java")) {
    print "patching cast() in $fname...";
    $content = load_file("$fname");

    #
    # IMPORTANT: Java files are only patched if they contain the @CASTFROM-OWNERSHIP@ string!
    #
    if (!($content =~ /\@CASTFROM-OWNERSHIP\@/s)) {
        print "disabled\n";
    } elsif ($content =~ s/(\bcastFrom\b.*?)({.*?})/\1__BODY__/gs) {
        $body = $2;

        $body =~ s/^.*\@notnull\@.*/    boolean memOwn = obj.swigCMemOwn;\n    obj.swigDisown();/m;
        $body =~ s/false\);/memOwn);/s;

        $content =~ s/__BODY__/$body/gs;
        print "OK\n";
    } else {
        print "none\n";
    }

    save_file("$fname", $content);
}

sub load_file()
{
    my $fname = shift;
    my $content;
    open FILE, "$fname" || die "cannot open $fname";
    read(FILE, $content, 1000000);
    close FILE;
    $content;
}

sub save_file()
{
    my $fname = shift;
    my $content = shift;
    open FILE, ">$fname" || die "cannot write $fname";
    print FILE $content;
    close FILE;
}

