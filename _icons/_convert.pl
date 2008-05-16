#
# Creates the icons.tcl file for Tkenv.
#

$verbose = 1;

$listfile = "tkenv.lst";

# parse listfile
print "reading $listfile...\n" if ($verbose);
open(INFILE, $listfile) || die "cannot open $listfile";
@fnames = ();
while (<INFILE>) {
    chomp;
    s/\r$//; # cygwin/mingw perl does not do CR/LF translation
    push(@fnames,$_);
}

$tmpfile = 'tmp.out';
$tclcode = '#=================================================================
#  ICONS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license\' for details on this and other legal matters.
#----------------------------------------------------------------#

';

foreach $fname (@fnames)
{
    print "processing $fname...\n" if ($verbose);
    system("cmd", "/c", "base64 <$fname >$tmpfile");

    my $txt = readfile($tmpfile);
    $txt =~ s/^/    /mg;  # indent

    my $name = $fname;
    $name =~ s/\..*//;

    $tclcode .= "set icons($name) [image create photo -data {\n";
    $tclcode .= $txt;
    $tclcode .= "}]\n\n";
}

writefile("icons.tcl", $tclcode);


sub readfile ()
{
    my $fname = shift;
    my $content;
    open FILE, "$fname" || die "cannot open $fname";
    read(FILE, $content, 1000000);
    close FILE;
    $content;
}

sub writefile ()
{
    my $fname = shift;
    my $content = shift;
    open FILE, ">$fname" || die "cannot open $fname for write";
    print FILE $content;
    close FILE;
}




