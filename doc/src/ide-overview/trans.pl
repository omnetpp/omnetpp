# replaces special double qutes with literal tag (just a shortcut)
# deletes the whitespace before and after tags if there is a '(' char before the tag (avoids spaces in (<tag>command</tag>) construct
$txt = "";

while (<>) {
    $txt .= $_;
}
$txt =~ s|``|<literal>|gs;
$txt =~ s|''|</literal>|gs;
$txt =~ s|\(\s*(<.*?>)|($1|gs;
$txt =~ s|(</.*?>)\s*([.)])|$1$2|gs;

print $txt;
