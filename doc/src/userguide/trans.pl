$txt = "";

while (<>) {
    $txt .= $_;
}
$txt =~ s|``|<literal>|gs;
$txt =~ s|''|</literal>|gs;
$txt =~ s|\(\s*(<.*?>)|($1|gs;
$txt =~ s|(</.*?>)\s*([.)])|$1$2|gs;

print $txt;
