#
#   Utility for embedding contents of a text file into C/C++ as a large string literal.
#

if (@ARGV eq "") {
    print STDERR "usage: text2c inputfile symbol\n";
    exit 1;
}

my $inputfile = shift @ARGV;
my $symbol = shift @ARGV;

$txt = readTextFile($inputfile);
$txt =~ s/(.)/escape($1)/sge;
$txt = "const char *$symbol = \n\t\"$txt\t\";\n";
print $txt;

exit 0;

sub escape($)
{
    my($ch) = @_;
    if ($ch eq "\t") {
        return "\\t";
    }
    if ($ch eq "\n") {
        return "\\n\"\n\t\"";
    }
    elsif (ord($ch)<32 || ord($ch)>=127) {
        return sprintf("\\x%02X", ord($ch));
    }
    else {
        return $ch;
    }
}

sub readTextFile($)
{
    my($file) = @_;
    open(INFILE, $file) || die "cannot open $file";
    read(INFILE, $content, 1000000) || die "cannot read $file";
    $content =~ s/\r$//mg;
    return $content;
}

