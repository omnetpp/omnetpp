#
# Migrates 3.x ini files to 4.x format. This consists of the following:
#
# - rename the sections [Parameters], [Cmdenv], [Tkenv], [OutVectors],
#   [Partitioning] to [General]
#
# - merge the resulting multiple occurrences of [General] into one
#
# - prefix [Cmdenv]/[Tkenv] config names with "cmdenv-" and "tkenv-"
#   (unless already begins with that)
#
# - rename sections [Run 1], [Run 2] etc to [Config config1], etc.
#
# - rename output vector configuration sections "**.interval=" to
#   "**.record-interval="
#
# - rename "**.use-default" to "**.apply-default"
#

$verbose = 1;

$listfile = $ARGV[0];
die "no listfile specified" if ($listfile eq '');

# parse listfile
print "reading $listfile...\n" if ($verbose);
open(INFILE, $listfile) || die "cannot open $listfile";
@fnames = ();
while (<INFILE>) {
    chomp;
    s/\r$//; # cygwin/mingw perl does not do CR/LF translation
    push(@fnames,$_);
}
#print join(' ', @fnames);

@obsoleteSections = ("Parameters", "Cmdenv", "Tkenv", "OutVectors", "Partitioning", "DisplayStrings");

@cmdenvNames = ("autoflush", "event-banner-details", "event-banners", "express-mode",
                "message-trace", "module-messages", "output-file", "performance-display",
                "runs-to-execute", "status-frequency");

@tkenvNames =  ("default-run", "image-path", "plugin-path");

@removeNames = ("preload-ned-files", "anim-methodcalls", "animation-enabled", "animation-msgclassnames",
               "animation-msgcolors", "animation-msgnames", "animation-speed",
               "expressmode-autoupdate", "methodcalls-delay", "next-event-markers",
               "penguin-mode", "print-banners", "senddirect-arrows", "show-bubbles",
               "show-layouting", "slowexec-delay", "update-freq-express",
               "update-freq-fast", "use-mainwindow", "use-new-layouter");

#note: these should be the NEW option names!
@commentOutNames = ("tkenv-default-run", "cmdenv-runs-to-execute");


foreach $fname (@fnames)
{
    print "reading $fname...\n" if ($verbose);
    $txt = readfile($fname);

    # rename Cmdenv keys
    $e = join("|", @cmdenvNames);
    $txt =~ s/^(\s*[;#]?\s*)($e)\b/$1cmdenv-$2/mg;

    # rename Tkenv keys
    $e = join("|", @tkenvNames);
    $txt =~ s/^(\s*[;#]?\s*)($e)\b/$1tkenv-$2/mg;

    # extra-stack-kb: we don't know whether it refers to Cmdenv or Tkenv, so we duplicate it
    $txt =~ s/^(\s*[;#]?\s*)(extra-stack-kb\s*=.*)$/$1cmdenv-$2\n$1tkenv-$2/mg;

    # remove obsolete keys
    $e = join("|", @removeNames);
    $txt =~ s/\n[ \t]*[;#]?[ \t]*($e)[ \t]*=[^\n]*//sg;

    # comment out some keys
    $e = join("|", @commentOutNames);
    $txt =~ s/^\s*(($e)\b)/# $1/mg;

    # rename per-object keys
    $txt =~ s/\.use-default\s*=/.apply-default =/mg;
    $txt =~ s/\.ev-output\s*=/.cmdenv-ev-output =/mg;
    $txt =~ s/\.enabled\s*=/.vector-recording =/mg;
    $txt =~ s/\.interval\s*=/.vector-recording-interval =/mg;
    $txt =~ s/\.pingApp\.vector-recording =/.pingApp.enabled =/mg; #change back false hit
    $txt =~ s/\.pingApp\.vector-recording-interval =/.pingApp.interval =/mg; #change back false hit
    $txt =~ s/\.akaroa\s*=/.with-akaroa =/mg;

    # convert yes/no and on/off to true/false
    $txt =~ s/=\s*yes\b/= true/mg;
    $txt =~ s/=\s*no\b/= false/mg;
    $txt =~ s/=\s*on\b/= true/mg;
    $txt =~ s/=\s*off\b/= false/mg;

    # rename sections to [General]
    $e = join("|", @obsoleteSections);
    $txt =~ s/^(\s*[;#]?\s*)\[($e)\]/\1\[General\]/mg;

    # rename [Run X] sections to [Config configX]
    $txt =~ s/^(\s*[;#]?\s*)\[Run ([0-9]+)\]/\1\[Config config\2\]/mg;

    # make exactly one space on both sides of "=" sign (optional, just cosmetics)
    $txt =~ s/^([^#;=]*?) *= */\1 = /mg;

    # cut off superfluous trailing ";" (optional, just cosmetics)
    $txt =~ s/^([^#;]*?);$/\1/mg;

    # replace ";" with "#" as comment mark, ignoring it inside string constants
    $txt =~ s/^([^#;"]*?);/\1#/mg;
    $txt =~ s/^([^#]*);([^"]*)$/\1#\2/mg;

    # merge several [General] sections into one
    $currentSection = "";
    $tmp = "";
    foreach $line (split("\n", $txt)) {
        $line .= "\n";
        if ($line =~ /^\s*\[(.*?)\]/) {
            $thisSection = $1;
            $line = "" if ($thisSection eq $currentSection);
            $currentSection = $thisSection;
        }
        $line = "" if ($line =~ /^\s*[#;]\s*\[(.*?)\]/);
        $tmp .= $line;
    }
    $txt = $tmp;

    writefile($fname, $txt);
    #writefile("$fname.new", $txt);
}

print "

Some OMNeT++ 3.x ini file features do not map 100% to 4.0 ones -- please revise
the converted ini files:

 1. Runs like [Run 1], [Run 2] have been converted into named configurations
    (that is, [Config config1], [Config config2], etc). Please revise the
    generated configuration names, and give them meaningful names.

 2. If you had lots of runs which iterate over a parameter range, consider
    replacing those runs with a single named configuration that contains
    an iteration. For example: **.par = ${1..5,10,20,50,100}

 3. The meaning of the cmdenv-runs-to-execute= and tkenv-default-run= options
    have changed. (In 3.x they referred to [Run] sections, in 4.x they mean
    the run number WITHIN the selected named configuration, by default
    [General].) These options have been commented out.

 4. cmdenv-express-mode= option default value changed from false to true --
    the ini file was NOT modified in this respect.

 5. Some Tkenv options (animation-enabled=, animation-speed=, etc) have been
    obsoleted, they got removed from the ini files.

Conversion done. You may safely re-run this script any time you want.
";

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




