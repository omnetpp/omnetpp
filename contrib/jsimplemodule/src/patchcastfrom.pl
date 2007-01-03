#!/usr/bin/perl
#
# Implement transfer of ownership into convertFrom()
#

$verbose = 0;

$dir = $ARGV[0];
die "no directory specified" if ($dir eq '');

chdir $dir || die "cannot cd to $dir";
foreach $fname (glob("*.java")) {
    #  public static cOutVector convertFrom(cPolymorphic obj) {
    #    long cPtr = SimkernelJNI.cOutVector_convertFrom(cPolymorphic.getCPtr(obj), obj);
    #    return (cPtr == 0) ? null : new cOutVector(cPtr, false);
    #  }
    #
    #  TO:
    #
    #  public static cOutVector convertFrom(cPolymorphic obj) {
    #    long cPtr = SimkernelJNI.cOutVector_convertFrom(cPolymorphic.getCPtr(obj), obj);
    #    if (cPtr==0) return null;
    #    boolean memOwn = obj.swigCMemOwn;
    #    obj.swigDisown();
    #    return new cOutVector(cPtr, memOwn);
    #  }

    print "patching convertFrom() in $fname...";
    $content = load_file("$fname");

    if ($content =~ s/(\bconvertFrom\b.*?)({.*?})/\1__BODY__/gs) {
        $body = $2;

        $body =~ s/( +)return\b/\1if (cPtr==0) return null;\n\1boolean memOwn = obj.swigCMemOwn;\n\1obj.swigDisown();\n\1return/s;
        $body =~ s/false\);/memOwn);/s;
        $body =~ s/\(cPtr == 0\) \? null : //s;

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

