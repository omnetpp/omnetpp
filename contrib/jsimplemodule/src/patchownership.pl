#!/usr/bin/perl
#
# Patch up the generated Java files with disown() calls.
#

$verbose = 0;

$dir = $ARGV[0];
die "no directory specified" if ($dir eq '');

#
# IMPORTANT: Java files must contain the @DISOWN-METHOD-ARGS@ string to enable the patching!
#
disown("cBasicChannel", "deliver",        "msg");
disown("cChannel",      "deliver",        "msg");
disown("cGate",         "deliver",        "msg");
disown("cMessage",      "encapsulate",    "msg");
disown("cSimpleModule", "send",           "msg");
disown("cSimpleModule", "sendDelayed",    "msg");
disown("cSimpleModule", "scheduleAt",     "msg");
disown("cSimpleModule", "cancelAndDelete","msg");
disown("cArray",        "add",            "obj");
disown("cArray",        "addAt",          "obj");
disown("cArray",        "set",            "obj");
disown("cMessage",      "addObject",      "p");
disown("cMessage",      "setControlInfo", "p");
disown("cPar",          "setObjectValue", "obj");
disown("cQueue",        "insert",         "obj");
disown("cQueue",        "insertBefore",   "obj");
disown("cQueue",        "insertAfter",    "obj");
#XXX cPar::setDoubleValue(cDoubleExpression) too...

sub disown()
{
    my $clazz = shift;
    my $method = shift;
    my $param = shift;
    my $fname = "$dir/$clazz.java";

    print "patching: $fname, $method(), $param...";
    $content = load_file("$fname");

    if (!($content =~ /\@DISOWN-METHOD-ARGS\@/s)) {
        print "disabled\n";
    } elsif ($content =~ s/(public .*\b${method}\b.*)/$1\n    $param.swigDisown();/gm) {
        print "OK\n";
    } else {
        die "ERROR - NOT FOUND!\n";
        exit 1;
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

