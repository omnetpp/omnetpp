$listfname = $ARGV[0];
open(LISTFILE, $listfname) || die "cannot open $listfname";
while (<LISTFILE>)
{
    chomp;
    $fname = $_;
    print "processing $fname...\n";

    open(INFILE, $fname) || die "cannot open $fname";
    read(INFILE, $txt, 1000000) || die "cannot read $fname";
    close INFILE;

    # INSERT CODE TO PROCESS $txt HERE

    open(OUTFILE, ">$fname") || die "cannot open $fname for write";
    print OUTFILE $txt || die "cannot write $fname";
    close OUTFILE;
}


























































