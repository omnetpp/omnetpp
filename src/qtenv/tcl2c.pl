#
#   Utility for embedding Tcl code into C/C++ as a large string literal.
#   Output is somewhat scrambled so that the Tcl code is not directly
#   readable inside the resulting binary.
#

if (@ARGV eq "") {
    print STDERR "usage: tcl2c output-c-file input-tcl-files";
    exit 1;
}

$result = "static unsigned char tcl_code[] = {";

$outfile = shift @ARGV;

my %processedFiles = ();
while (@ARGV)
{
    $arg = shift @ARGV;

    foreach $infile (glob($arg)) {
        if ($processedFiles{$infile} != 1) {
            $processedFiles{$infile} = 1;
            $txt = readTextFile($infile);
            $result .= "\n  /* $infile */\n  ";
            for ($i=0; $i<length($txt); $i++) {
                $a = ord(substr($txt, $i, 1));
                $a = 255 - $a;
                $result .= "$a,";
                $result .= "\n  " if ($i%20 == 19);
            }
        }
    }
}

$result .= "0\n};\n\n";

$result .=
    "/* decoding part */\n" .
    "{\n" .
    "  int i;\n" .
    "  unsigned char *s = tcl_code;\n" .
    "  for (i=0; i<(int)sizeof(tcl_code)-1; i++,s++) {\n" .
    "    *s = 255-(*s);\n" .
    "  }\n" .
    "}\n";


open(OUT, ">$outfile");
print OUT $result || die "cannot write $outfile";
close OUT;

exit 0;

sub readTextFile($)
{
    my($file) = @_;
    open(INFILE, $file) || die "cannot open $file";
    defined read(INFILE, $content, 1000000) || die "cannot read $file";
    $content =~ s/\r$//mg;
    return $content;
}

