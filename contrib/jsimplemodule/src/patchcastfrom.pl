#!/usr/bin/perl
#
# Implement transfer of ownership into castFrom()
#

$verbose = 0;

$dir = $ARGV[0];
die "no directory specified" if ($dir eq '');

chdir $dir || die "cannot cd to $dir";
foreach $fname (glob("*.java")) {
    #  public static cOutVector castFrom(cPolymorphic obj) {
    #    long cPtr = SimkernelJNI.cOutVector_castFrom(cPolymorphic.getCPtr(obj), obj);
    #    return (cPtr == 0) ? null : new cOutVector(cPtr, false);
    #  }
    #
    #  TO:
    #
    #  public static cOutVector castFrom(cPolymorphic obj) {
    #    long cPtr = SimkernelJNI.cOutVector_castFrom(cPolymorphic.getCPtr(obj), obj);
    #    if (cPtr==0) return null;
    #    boolean memOwn = obj.swigCMemOwn;
    #    obj.swigDisown();
    #    return new cOutVector(cPtr, memOwn);
    #  }

    print "patching castFrom() in $fname...";
    $content = load_file("$fname");

    if ($content =~ s/(\bcastFrom\b.*?)({.*?})/\1__BODY__/gs) {
        $body = $2;

        $body =~ s/^.*\@notnull\@.*/    boolean memOwn = obj.swigCMemOwn;\n    obj.swigDisown();/m;
        $body =~ s/swigSetMemOwn\(false\)/swigSetMemOwn(memOwn)/s;

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

