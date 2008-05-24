#
# Extracts grammar from a bison source (.y) in a readable form.
#
# perl extractgrammar.pl ../../src/nedxml/ned2.y
#

$verbose = 1;

$yfile = $ARGV[0];
die "no .y file specified" if ($yfile eq '');
$txt = readfile($yfile);

# keep only the middle part (the rules)
$txt =~ s/^.*%%(.*)%%.*$/$1/s;


# remove curly brace blocks
$txt =~ s/'{'/'<<<'/s;
$txt =~ s/'}'/'>>>'/s;
for ($i=0; $i<10; $i++) {
    $txt =~ s/{[^{}]*}//gs;
}
$txt =~ s/'<<<'/'{'/s;
$txt =~ s/'>>>'/'}'/s;

# remove comments
$txt =~ s|/\*.*?\*/||gs;

# remove error recovery rules
$txt =~ s/^.*\berror\b.*$//mg;

# remove mid-rule blank lines
for ($i=0; $i<10; $i++) {
    $txt =~ s/\n\s*(\n\s+)/$1/gs;
}

print $txt;

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




