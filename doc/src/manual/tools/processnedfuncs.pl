#
# formats output of opp_run -h nedfunctions into latex, for appendix-ned-functions.tex
#

$txt = "";
while (<>) { chomp; s/\r$//; $txt .= $_ . "\n"; }

$txt =~ s/([_\$])/\\$1/sg;
$txt =~ s/^ (Category.*)/\\end{description}\n\n\\section{$1}\n\n\\begin{description}/mg;
$txt =~ s/^  ([^ :]+) *: *(.*)/\\item[$1]: \\ttt{$2} \\\\/mg;

$txt =~ s/^.*?(\\section)/$1/s;
$txt =~ s/End\.\s*//s;
$txt =~ s/\s*$/\n\\end{description}\n/s;

print $txt;
