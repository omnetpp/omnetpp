#!/usr/bin/perl
#
# part of opp_neddoc -- renders NED comments into HTML
#

$verbose = 0;

foreach $fnamepatt (@ARGV)
{
    foreach $fname (glob($fnamepatt))
    {
        #
        # read file
        #
        print "processing $fname...\n" if ($verbose);
        open(INFILE, $fname) || die "cannot open $fname";
        read(INFILE, $html, 1000000) || die "cannot read $fname";

        #
        # process comments
        #
        while ($html =~ s|\<pre class="(comment)">(.*?)\</pre\>|@bingo@|s ||
               $html =~ s|\<span class="(comment)">(.*?)\</span\>|@bingo@|s ||
               $html =~ s|\<span class="(briefcomment)">(.*?)\</span\>|@bingo@|s)
        {
              # process comment
              $class = $1;
              $comment = $2;

              # add sentries to facilitate processing
              $comment = "\n\n".$comment."\n\n";

              # remove '//' from beginning of lines
              $comment =~ s|^ *// ?||gm;

              # insert blank line (for later processing) in front of lines beginning with '-'
              $comment =~ s|\n( *-)|\n\n\1|gs;

              # if briefcomment, keep only the 1st paragraph
              if ($class eq "briefcomment") {
                 $comment =~ s|(.*?[^\s].*?)\n\n.*|\1\n\n|gs;
              }

              # wrap paragraphs not beginning with '-' into <p></p>.
              # (note: (?=...) and (?<=...) constructs are lookahead and lookbehind assertions,
              # see e.g. http://tlc.perlarchive.com/articles/perl/pm0001_perlretut.shtml).
              $comment =~ s|(?<=\n\n)\s*([^- \t].*?)(?=\n\n)|<p>\1</p>|gs;

              # wrap paragraphs beginning with '-' into <li></li> and <ul></ul>
              $comment =~ s|(?<=\n\n)\s*-\s+(.*?)(?=\n\n)|  <ul><li>\1</li></ul>|gs;
              $comment =~ s|\</li\>\</ul\>\s*\<ul\>\<li\>|</li>\n\n  <li>|gs;

              # wrap paragraphs beginning with '-#' into <li></li> and <ol></ol>
              $comment =~ s|(?<=\n\n)\s*-#\s+(.*?)(?=\n\n)|  <ol><li>\1</li></ol>|gs;
              $comment =~ s|\</li\>\</ol\>\s*\<ol\>\<li\>|</li>\n\n  <li>|gs;

              # now we can trim excess blank lines
              $comment =~ s|^\n+||;
              $comment =~ s|\n+$|\n|;

              # restore " from &quot; (important for attrs of html tags, see below)
              $comment =~ s|&quot;|"|gsi;

              # decode certain HTML tags: <i>,<b>,<br>,...
              $tags="a|b|body|br|center|caption|code|dd|dfn|dl|dt|em|form|hr|h1|h2|h3|i|input|img|li|meta|multicol|ol|p|pre|small|strong|sub|sup|table|td|tr|tt|kbd|ul|var";
              $comment =~ s!&lt;(($tags)( [^\n]*?)?)&gt;!<\1>!gsi;
              $comment =~ s!&lt;(/($tags))&gt;!<\1>!gsi;

              # put comment back
              $html =~ s/@bingo@/$comment/s;
        }


        #
        # save file
        #
        open(FILE,">$fname");
        print FILE $html;
        close FILE;
    }
}
