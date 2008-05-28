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
$txt =~ s/'{'/'<<<'/gs;
$txt =~ s/'}'/'>>>'/gs;
for ($i=0; $i<10; $i++) {
    $txt =~ s/{[^{}]*}//gs;
}
$txt =~ s/'<<<'/'{'/gs;
$txt =~ s/'>>>'/'}'/gs;

# remove comments
$txt =~ s|/\*.*?\*/||gs;

# remove error recovery rules
$txt =~ s/^.*\berror\b.*$//mg;

# remove '%prec' stuff
$txt =~ s/\%prec\s+[a-zA-Z]+//mg;

# remove mid-rule blank lines
for ($i=0; $i<10; $i++) {
    $txt =~ s/\n\s*(\n\s+)/$1/gs;
}

# remove trailing "_" from some names
$txt =~ s/([A-Z]+)_\b/$1/gs;

# substitute some terminals
%terminals = (
   "RIGHTARROW"  =>     "'-->'",
   "LEFTARROW"  =>      "'<--'",
   "DBLARROW"  =>       "'<-->'",
   "TO"  =>             "'..'",
   "DOUBLEASTERISK"  => "'**'",
   "PLUSPLUS"  =>       "'++'",
   "OR"  =>             "'||'",
   "AND"  =>            "'&&'",
   "XOR"  =>            "'##'",
   "NOT"  =>            "'!'",
   "BIN_OR"  =>         "'|'",
   "BIN_AND"  =>        "'&'",
   "BIN_XOR"  =>        "'#'",
   "BIN_COMPL"  =>      "'~'",
   "SHIFT_LEFT"  =>     "'<<'",
   "SHIFT_RIGHT"  =>    "'>>'",
   "EQ"  =>             "'=='",
   "NE"  =>             "'!='",
   "LE"  =>             "'<='",
   "GE"  =>             "'>='",
   "CPLUSPLUSBODY" =>   "'{{' ... '}}'",
   "DOUBLETYPE" =>      "DOUBLE",
   "CHARTYPE" =>        "CHAR",
   "SHORTTYPE" =>       "SHORT",
   "INTTYPE" =>         "INT",
   "LONGTYPE" =>        "LONG",
   "STRINGTYPE" =>      "STRING",
   "BOOLTYPE" =>        "BOOL",
   "XMLTYPE" =>         "XML",
);

while (($key, $value) = each(%terminals)){
    $txt =~ s/\b$key\b/$value/gs;
}

#
# LaTeX escaping and formatting follows
#
$txt =~ s/{/\\{/gs;
$txt =~ s/}/\\}/gs;
$txt =~ s/_/\\_/gs;


# make terminals/nonterminals bold/italic (does not look good)
#$txt =~ s/\b([a-z_][a-z0-9_\\]*)\b/\\textit{$1}/gs;
#$txt =~ s/\b([A-Z_][A-Z0-9_\\]*)\b/"\\textbf{".lc($1)."}"/gse;
#$txt =~ s/^  +/\\hspace{1in}/gm;
#$txt =~ s/^(.+)$/$1 \\\\/gm;


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




