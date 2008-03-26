#
# linkall -- wrapper for the Microsoft Visual C++ Linker LINK.EXE.
#
# Ensures that all symbols from LIB files will make it into the executable.
# This is needed for LIB files that contain simple modules or other
# self-registering extension components.
#
# USAGE: just use this one everywhere the Microsoft linker would get invoked.
#
# Finds LIB files in the linker command line, uses dumpbin to extract the
# list of public symbols from them, and turns them to linker command
# files that contain an "/include:<symbol>" option for each symbol.
#
# See http://www.omnetpp.org/pmwiki/index.php?n=Main.VCOptNorefNotWorking
# and http://forums.microsoft.com/MSDN/ShowPost.aspx?PostID=144087&SiteID=1&mode=1
#

$verbose = 0;

$DUMPBIN = "dumpbin";
$LINKER = "link";
$DUMPFILE = "dumpbin.out";
$tempfilename ="linkall.tmp";

@knownlibs = (
    "sim_std.lib", "envir.lib", "cmdenv.lib", "tkenv.lib",
    "common.lib", "nedxml.lib", "layout.lib",
    "sim_stdd.lib", "envird.lib", "cmdenvd.lib", "tkenvd.lib",
    "commond.lib", "nedxmld.lib", "layoutd.lib",
    "tcl84.lib", "tk84.lib", "blt24.lib", "zdll.lib",
    "libxml2.lib", "iconv.lib", "wsock32.lib"
);
foreach $i (@knownlibs) {$knownlibs{$i} = 1;}

# collect linker path
@libpath = split(';', $ENV{LIB});
foreach $arg (@ARGV) {
    if ($arg =~ /[-\/]libpath:(.*)/i) {
        $dir = $1;
        print("adding dir to lib path: $dir\n") if $verbose;
        push(@libpath, $dir);
    }
}

# remove temp files when killed by Ctrl+C, by catching the INT signal
$SIG{'INT'} = 'removeTempFiles';

# process libs
$content = "";
foreach $arg (@ARGV) {
    $content .= $arg."\n";
    if ($arg =~ /.*\.lib$/i) {
        $lib = $arg;

        # ignore known libs
        if ($knownlibs{$lib}) {
            print("skipping known lib $lib\n") if $verbose;
            next;
        }

        print("processing lib $lib...\n") if $verbose;

        # find library in linker path
        if (! -r $lib) {
            foreach $libdir (@libpath) {
                if (-r "$libdir/$lib") {
                    $lib = "$libdir/$lib";
                    break;
                }
            }
            if (! -r $lib) {
                error("cannot find file '$lib' (searched lib path: ". join(';', @libpath) . ")");
            }
        }

        # invoke dumpbin and redirect output into file
        unlink($DUMPFILE) || error("cannot remove existing $DUMPFILE") if -r $DUMPFILE;
        $prog = "$DUMPBIN /linkermember:1 $lib >$DUMPFILE";
        system($ENV{COMSPEC}, "/c", $prog)==0 || error("error invoking dumpbin (check contents of $DUMPFILE)");

        # read dumpbin output
        open(IN, $DUMPFILE) || error("cannot open $DUMPFILE");
        read(IN, $txt, 100000000) || error("cannot read $DUMPFILE");
        close(IN);
        unlink($DUMPFILE) || error("cannot remove temp file $DUMPFILE");
        $txt =~ s/\r$//mg; # cygwin/mingw perl does not do CR/LF translation

        # parse out useful part
        if (!($txt =~ /^Microsoft.*\n *File Type: *LIBRARY\n.*\n *[0-9]+ public symbols\n(.*)\n +Summary\n.*/s)) {
            error("unexpected dumpbin output, see $DUMPFILE");
        }
        $txt = $1;

        # prefix each symbol with "/include:", and write option file.
        # (actually, we only select symbols that come from EXECUTE_ON_STARTUP
        # macros -- adding all symbols would send the Microsoft linker to the
        # floor)
        foreach $line (split("\n", $txt)) {
            if ($line =~ /^ +[0-9A-Fa-f]+ +(.*__onstartup_func_.*)$/) {
                 $content .= "/include:$1\n"
            }
        }
    }
    
}
open(OUT, ">".$tempfilename) || error("cannot open $tempfilename for write");
print OUT $content."\n" || error("cannot write $tempfilename"); # do NOT remove "\n" -- would cause error for empty files
close(OUT) || error("cannot close $tempfilename");

# put together linker command line and invoke linker
@cmdline = ($LINKER);
push(@cmdline, "@".$tempfilename);
print("invoking linker: ".join(' ', @cmdline) . "\n") if $verbose;
system($ENV{COMSPEC}, ("/c", join(' ', @cmdline)))==0 || error("error invoking linker"); # join is needed for mingw/perl

# remove temporary linker command files
removeTempFiles();
exit(0);

sub error {
    my $msg = shift;
    print STDERR "linkall: $msg\n";
    removeTempFiles();
    exit(1);
}

sub removeTempFiles {
        unlink $tempfilename || print STDERR "linkall: cannot remove temp file $tempfilename";
}


