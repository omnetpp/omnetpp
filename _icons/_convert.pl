#
# Creates the icons.tcl file for Tkenv.
#

$verbose = 1;

$listfile = $ARGV[0];
$tclfile = $ARGV[1];
$varname = $ARGV[2];

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
$tclcode = "#=================================================================
#  $tclfile - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2014 Opensim Ltd.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

";

foreach $fname (@fnames)
{
    print "processing $fname...\n" if ($verbose);

    my $name = $fname;
    $name =~ s|^(.*/)?(.*)\..*|$2|;

    my $txt = `base64 --wrap=64 $fname `;
    $txt =~ s/^/    /mg;  # indent

    $tclcode .= "set $varname($name) [image create photo -data {\n";
    $tclcode .= $txt;
    $tclcode .= "}]\n\n";
}

writefile($tclfile, $tclcode);

unlink($tmpfile);


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




