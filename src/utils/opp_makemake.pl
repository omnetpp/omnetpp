eval '(exit $?0)' && eval 'exec perl -S $0 ${1+"$@"}' && eval 'exec perl -S $0 $argv:q'
  if 0;

#!perl
#line 6
#
# opp_makemake.pl
#
#  Creates an MSVC makefile for a given OMNeT++/OMNEST model.  FIXME
#  Assumes that .ned, .msg, .cc and .h files are in one directory.
#  The name of the program defaults to the name of the directory ('myproject').
#
#  --VA
#

use Cwd;
use Config;

$ARG0 = $0;
$progname = $ARG0;

$arch = $Config{'archname'};
$isCygwin = ($arch =~ /cygwin/i);

$isWindows = ($ENV{OS} =~ /windows/i) ? 1 : 0;

if ($isWindows && $ENV{OS} ne "Windows_NT") {
    error("this program can only be used on Windows NT/2000/XP, but your OS environment variable says '$ENV{OS}'\n");
}

#
# process command line args
#

# input parameters
$isnmake = $isWindows && !isCygwin;
$outfile = cwd;
$outfile =~ s/[\/\\]$//;
$outfile =~ s/.*[\/\\]//;

$recursive=0;
$userif = "ALL";
$type = "exe";
$includes = "";
$libpath = "";
$libs = "";
$linkdirs = "";
$xobjs = "";
@fragments = ();
$doxyconf = "doxy.cfg";
$ccext = "";  # we'll try to autodetect it
$configfile="";
$basedir="";
$linkwithobjects=0;
$ignorened=1;
$force=0;
$tstamp=1;
@tstampdirs = ();
@exceptdirs = ();
$exportdefopt = "";
$importlibs = "";
$fordll = 0;

$makecommand = $nmake ? 'nmake /nologo /f Makefile.vc' : 'make';


# process arg vector
while (@ARGV)
{
    $arg = shift @ARGV;
    if ($arg eq '-h' || $arg eq '--help')
    {
        print '
FIXME merge help text and options!
$progname: create MSVC makefile for an OMNeT++/OMNEST model, based on
source files in current directory

$progname [-h] [-f] [-e ext] [-o make-target] [-n] [-u user-interface]
              [-w] [-x] [-M] [-Idir] [-Ldir] [-llibrary] [-c configdir]
              [-i makefile-fragment-file]
              [directories, library and object files]...
    -h, --help            This help text
    -f, --force           Force overwriting existing Makefile
    -e ext, --ext ext     C++ source file extension, usually "cc" or "cpp".
                          By default, this is determined by looking at
                          existing files in the directory.
    -o filename, --outputfile filename
                          Name of simulation executable/library
    -r, --recurse         Call make recursively in all subdirectories. If you
                          need to maintain a specific order, declare dependen-
                          cies in the makefrag.vc file.
    -X directory, -Xdirectory, --except directory
                          With -r (recurse) option: ignore the given directory
    -b directory, --basedir directory
                          Project base (root) directory; all absolute paths
                          (-I, -L, object file names, etc.) which point into
                          this directory will be converted to relative, to
                          ease compiling the project in a different directory.
    -n, --nolink          Produce object files but do not create executable or
                          library. Useful for models with parts in several
                          directories. With this option, -u and -l have
                          no effect.
    -s, --make-so         Build a DLL. Useful if you want to load the
                          model dynamically (via the load-libs= omnetpp.ini or
                          the -l Cmdenv/Tkenv command-line option).
    -t library, --importlib library
                          With -t (build DLL) option: specifies an import
                          library for the DLL.
    -d, --fordll          Compile C++ files for use in DLLs (i.e. with the
                          WIN32_DLL symbol defined). The -s (build DLL) option
                          implies this one.
    -w, --withobjects     Link with all object files found in -I directories,
                          or include them if library is created. Ignored when
                          -n option is present. Dependencies between directo-
                          ries have be handled in high Makefiles (see -r
                          option).
    -x, --notstamp        Do not require a .tstamp file to be present in the
                          link directories and (if -w option is present)
                          -I directories after this option.
    -u name, --userinterface name
                          Use all, Cmdenv or Tkenv. Defaults to all.
    -Idir                 Additional NED and C++ include directory
    -Ldir                 Add a directory to the library path
    -llibrary             Additional library to link against
                          (e.g. -lmylibrary.lib)
    -P symbol, -Psymbol   -P option to be passed to opp_msgc
    -c filename, --configfile filename
                          Included config file (default:"../../configuser.vc")
    -i filename, --includefragment filename
                          Append file to near end of Makefile. The file
                          makefrag.vc (if exists) is appended automatically
                          if no -i options are given. This option is useful
                          if a source file (.ned, .msg or .cc) is to be
                          generated from other files.
    directory             Link with all object files in that directory.
                          Dependencies between directories have to be added
                          manually. See also -w option.
    library or object     Link with that file

Default output is Makefile.vc, which you can invoke by typing
  nmake -f Makefile.vc
With the -n and -s options, -u and -l have no effect. makefrag.vc (and the
-i option) is useful when a source file (.ned, .msg or .cc) is to be generated
from other files, or when you want to add extra dependencies that opp_makemake
could not figure out.
';
        exit(1);
    }
    elsif ($arg eq '-f' || $arg eq '--force')
    {
        $force = 1;
    }
    elsif ($arg eq '-e' || $arg eq '--ext')
    {
        $ccext = shift @ARGV;
    }
    elsif ($arg eq '-o' || $arg eq '--outputfile')
    {
        $outfile = shift @ARGV;
        $outfile = abs2rel($outfile,$basedir);
    }
    elsif ($arg eq '-N' || $arg eq '--ignore-ned')
    {
        print STDERR "$progname: $arg: obsolete option, please remove (dynamic NED loading is now the default)";
    }
    elsif ($arg eq '-r' || $arg eq '--recurse')
    {
        $recursive = 1;
    }
    elsif ($arg eq '-X' || $arg eq '--except')
    {
        $dir = shift @ARGV;
        push(@exceptdirs, $dir);
    }
    elsif ($arg =~ /^-X/)
    {
        $dir = $arg;
        $dir =~ s/^-X//;
        push(@exceptdirs, $dir);
    }
    elsif ($arg eq '-b' || $arg eq '--basedir')
    {
        $basedir = shift @ARGV;
    }
    elsif ($arg eq '-c' || $arg eq '--configfile')
    {
        $configfile = shift @ARGV;
        $configfile = abs2rel($configfile,$basedir);
    }
    elsif ($arg eq '-n' || $arg eq '--nolink')
    {
        $type = "o";
    }
    elsif ($arg eq '-s' || $arg eq '--make-so')
    {
        $fordll = 1;
        $type = "so";
    }
    elsif ($arg eq '-t' || $arg eq '--importlib')
    {
        $importlibs .= ' ' . shift @ARGV;
    }
    elsif ($arg eq '-d' || $arg eq '--fordll')
    {
        $fordll = 1;
    }
    elsif ($arg eq '-w' || $arg eq '--withobjects')
    {
        $linkwithobjects = 1;
    }
    elsif ($arg eq '-x' || $arg eq '--notstamp')
    {
        $tstamp = 0;
    }
    elsif ($arg eq '-u' || $arg eq '--userinterface')
    {
        $userif = shift @ARGV;
        $userif = uc($userif);
        if ($userif ne "ALL" && $userif ne "CMDENV" && $userif ne "TKENV")
        {
            print STDERR "$progname: -u: specify all, Cmdenv or Tkenv";
            exit(1);
        }
    }
    elsif ($arg eq '-i' || $arg eq '--includefragment')
    {
        $frag = shift @ARGV;
        $frag = abs2rel($frag,$basedir);
        push(@fragments, $frag);
    }
    elsif ($arg =~ /^-I/)
    {
        $dir = $arg;
        $dir =~ s/^-I//;
        $dir = abs2rel($dir, $basedir);
        $includes .= " -I".quote($dir);
        if ($tstamp eq 1 && $dir ne ".")
        {
            push(@tstampdirs, $dir);
        }
    }
    elsif ($arg =~ /^-L/)
    {
        $dir = $arg;
        $dir =~ s/^-L//;
        $dir = abs2rel($dir, $basedir);
        $libpath .= " /libpath:".quote($dir);
    }
    elsif ($arg =~ /^-l/)
    {
        $arg =~ s/^-l//;
        $libs .= " $arg";
    }
    elsif ($arg eq '-P')
    {
        $exportdefopt = '-P' . shift @ARGV;
    }
    elsif ($arg =~ /^-P/)
    {
        $exportdefopt = $arg;
    }
    else
    {
        $arg =~ s|/|\\|g;
        if (-d $arg)
        {
            $arg = abs2rel($arg, $basedir);
            push(@linkdirs, $arg);
            if ($tstamp)
            {
                push(@tstampdirs, $arg);
            }
        }
        elsif (-f $arg)
        {
            $arg = abs2rel($arg, $basedir);
            push(@xobjs, $arg);
        }
        else
        {
            print STDERR "$progname: $arg is neither an existing file/dir nor a valid option";
            exit(1);
        }
    }
}

$makefile = $nmake ? "Makefile.vc" : "Makefile";

if (-f $makefile && $force ne 1)
{
    print STDERR "$progname: use -f to force overwriting existing $makefile\n";
    exit(1);
}

#FIXME cfgfile is nmake only!
if ($configfile eq "")
{
    # try to find it
    $progdir = $0;
    $progdir =~ s|\\|/|g;
    $progdir =~ s|/?[^/:]*$||g;
    $progdir = "." if ($progdir eq "");
    $progparentdir = $progdir;
    $progparentdir =~ s|/?[^/:]*/?$||;
    foreach $f ("configuser.vc", "../configuser.vc", "../../configuser.vc",
                "../../../configuser.vc", "../../../../configuser.vc",
                "../../../../../configuser.vc", "$progparentdir/configuser.vc",
                "$progdir/configuser.vc")
    {
        if (-f $f) {
             $configfile = $f;
             last;
        }
    }
    if ($configfile eq "") {
        print STDERR "$progname: warning: configuser.vc file not found -- try -c option or edit generated makefile\n";
    }

}
else
{
    if (! -f $configfile) {
        print STDERR "$progname: error: file $configfile not found\n";
        exit(1);
    }
}

# try to determine if .cc or .cpp files are used
@ccfiles = glob("*.cc");
@cppfiles = glob("*.cpp");
if ($ccext eq '')
{
    @ccfiles = glob("*.cc");
    @cppfiles = glob("*.cpp");
    if (@ccfiles==() && @cppfiles!=()) {
        $ccext = "cpp";
    }
    elsif (@ccfiles!=() && @cppfiles==()) {
        $ccext = "cc";
    }
    elsif (@ccfiles!=() && @cppfiles!=()) {
        print STDERR "$progname: you have both .cc and .cpp files -- ".
                     "use -e cc or -e cpp option to select which set of files to use\n";
        exit(1);
    }
    else {
        # if no files, use .cc extension
        $ccext = "cc";
    }
}
else
{
    if ($ccext eq "cc" && @ccfiles==() && @cppfiles!=()) {
        print STDERR "$progname: warning: you specified -e cc but you have only .cpp files in this directory!\n";
    }
    if ($ccext eq "cpp" && @cppfiles==() && @ccfiles!=()) {
        print STDERR "$progname: warning: you specified -e cpp but you have only .cc files in this directory!\n";
    }
}

@subdirs = ();
if ($recursive)
{
    foreach $i (glob("*"))
    {
        #FIXME revise list!
        if (-d $i && $i ne "CVS" && $i ne "backups" && $i ne "." && $i ne ".." && !grep(/$i/,@exceptdirs))
        {
            push(@subdirs, $i);
        }
    }
}

@extdirobjs = ();
if ($linkwithobjects)
{
    foreach $i (split(' ',$includes))
    {
        $i =~ s/^-I//;
        if ($i ne ".") {
            push(@extdirobjs, "$i/*.obj");
        }
    }
}

foreach $i (@linkdirs)
{
    push(@extdirobjs, "$i/*.obj");
}

@extdirtstamps = ();
foreach $i (@tstampdirs)
{
    push(@extdirtstamps, quote($i."/.tstamp"));
}

if ($ignorened) {
    $objpatt = "*.msg *.$ccext";
} else {
    $objpatt = "*.ned *.msg *.$ccext";
}
foreach $i (glob($objpatt))
{
    $i =~ s/\*[^ ]*//g;
    $i =~ s/[^ ]*_n\.$ccext//g;
    $i =~ s/[^ ]*_m\.$ccext//g;
    $i =~ s/\.ned/_n.obj/g;
    $i =~ s/\.msg/_m.obj/g;
    $i =~ s/\.$ccext/.obj/g;
    push(@objs, $i) if ($i ne '');
}

foreach $i (glob("*.msg"))
{
    $i =~ s/\.msg/_m.h/g;
    push(@generatedheaders, $i);
}

$makefrags = "FIXME";
$deps = "FIXME";

#
# now the Makefile creation
#
$dir = cwd;
print "Creating $makefile in $dir...\n";

%m = (
    "nmake" => $isnmake,
    "target" => $outfile,  #XXX in Java this is called "target" (???)
    "progname" => $isnmake ? "opp_nmakemake" : "opp_makemake",
    "args" => join(' ', @ARGV),
    "configfile" => $configfile,
    "-L" => $isnmake ? "/libdir:" : "-L",
    "-l" => $isnmake ? "" : "-l",
    ".lib" => $isnmake ? ".lib" : "",
    "-u" => $isnmake ? "/include:" : "-u",
    "_dir" => "_dir",
    "cc" => $ccext,
    "deps" => $deps,
    "exe" => ($type eq 'exe'),
    "so" => ($type eq 'so'),
    "nolink" => ($type eq 'o'),
    "allenv" => ($userif eq 'ALL'),
    "cmdenv" => ($userif eq 'CMDENV'),
    "tkenv" => ($userif eq 'TKENV'),
    "extdirobjs" => join($externaldirobjs),
    "extdirtstamps" => join($externaldirtstamps),
    "extraobjs" => join($externalObjects),
    "includepath" => join($includeDirs),
    "libs" => join($libDirs, $isnmake ? "/libpath:" : "-L") . join($libs) . join($importLibs),
    "link-o" => $isnmake ? "/out:" : "-o",
    "makecommand" => $makecommand,
    "makefile" => $isnmake ? "Makefile.vc" : "Makefile",
    "makefrags" => $makefrags,
    "msgccandhfiles" => $msgccandhfiles,
    "msgfiles" => $msgfiles,
    "objs" => join($objs),
    "hassubdir" => (@subdirs != ()),
    "subdirs" => join($subdirs),
    "subdirtargets" => join($subdirTargets),
    "fordllopt" => $compileForDll ? "/DWIN32_DLL" : "",
    "dllexportmacro" => $exportDefOpt==null ? "" : ("-P" + exportDefOpt),
);

$content = substituteIntoTemplate(template(), \%m, "{", "}");

open(OUT, ">$makefile");
print OUT $content;
close OUT;

print "$makefile created.\n";
print "Please type `nmake -f $makefile depend' NOW to add dependencies!\n";

#----

sub substituteIntoTemplate($;$)
{
    my ($template,$mapref) = @_;
    my $startTag = "{";
    my $endTag = "}";
    my %map = %$mapref;

    my $buf = "";
    my $startTagLen = length($startTag);
    my $endTagLen = length($endTag);

    my $current = 0;
    while (true) {
        my $start = index($template, $startTag, $current);
        if ($start == -1) {
            last;
        }
        else {
            my $end = index($template, $endTag, $start);
            if ($end != -1) {
                my $tag = substr2($template, $start, $end + $endTagLen);
                #print("processing $tag\n");
                my $key = substr2($template, $start+$startTagLen, $end);
                if (index($key, "\n") != -1) {
                    die("template error: newline inside \"$tag\" (misplaced start/end tag?)");
                }
                my $isLineStart = $start==0 || substr($template, $start-1, 1) eq "\n";
                my $isNegated = substr($key, 0, 1) eq "~";
                if ($isNegated) {
                    $key = substr($key, 1);  # drop "~"
                }
                my $colonPos = index($key, ":");
                my $substringAfterColon = $colonPos == -1 ? "" : substr($key, $colonPos+1);
                if ($colonPos != -1) {
                    $key = substr2($key, 0, $colonPos); # drop ":..."
                }

                # determine replacement string, and possibly adjust start/end
                my $replacement = "";
                if ($colonPos != -1) {
                    if ($isLineStart && $substringAfterColon eq "") {
                        # replacing a whole line
                        if (getFromMapAsBool(\%map, $key) != $isNegated) {
                            # put line in: all variables OK
                        }
                        else {
                            # omit line
                            my $endLine = index($template, "\n", $end);
                            if ($endLine == -1) {
                                $endLine = length($template);
                            }
                            $replacement = "";
                            $end = $endLine;
                        }
                    }
                    else {
                        # conditional
                        if ($substringAfterColon eq "") {
                            die("template error: found \"$tag\" mid-line, but whole-line conditions should begin at the start of the line");
                        }
                        $replacement = getFromMapAsBool(\%map, $key)!=$isNegated ? $substringAfterColon : "";
                    }
                }
                else {
                    # plain replacement
                    if ($isNegated) {
                        die("template error: wrong syntax \"$tag\" (missing \":\"?)");
                    }
                    $replacement = getFromMapAsString(\%map, $key);
                }

                # do it: replace substring(start, end) with replacement, unless replacement==null
                $buf .= substr2($template, $current, $start);  # template code up to the {...}
                $buf .= $replacement;
                $current = $end + $endTagLen;
            }
        }
    }
    $buf .= substr($template, $current);  # rest of the template
    return $buf;
}

sub substr2($;$;$)
{
    my($string, $startoffset, $endoffset) = @_;
    return substr($string, $startoffset, $endoffset - $startoffset);
}

# for substituteIntoTemplate()
sub getFromMapAsString($;$)
{
    my($mapref,$key) = @_;
    my %map = %$mapref;
    die("template error: undefined template parameter '$key'") if (!defined($map{$key}));
    return $map{$key};
}

# for substituteIntoTemplate()
sub getFromMapAsBool($;$)
{
    my($mapref,$key) = @_;
    my %map = %$mapref;
    die("template error: undefined template parameter '$key'") if (!defined($map{$key}));
    $value = $map{$key};
    die("template error: template parameter '$key' was expected to be a boolean, but it is \"$value\"") if ($value ne '' && $value ne '0' && $value ne '1');
    return $value;
}

#
# Converts absolute path $abs to relative path (relative to the current
# directory $cur), provided that both $abs and $cur are under a
# "project base directory" $base. Otherwise it returns the original path.
# All "\" are converted to "/".
#
sub abs2rel($;$;$;)
{
    my($abs, $base, $cur) = @_;

    if ($base eq '') {
        return $abs;
    }
    if ($cur eq '') {
        $cur = cwd;
    }

    # some normalization
    $abs  =~ s|\\|/|g;
    $cur  =~ s|\\|/|g;
    $base =~ s|\\|/|g;

    $abs  =~ s|/\./|/|g;
    $cur  =~ s|/\./|/|g;
    $base =~ s|/\./|/|g;

    $abs  =~ s|//+|/|g;
    $cur  =~ s|//+|/|g;
    $base =~ s|//+|/|g;

    $cur  =~ s|/*$|/|;
    $base =~ s|/*$|/|;

    if (!($abs =~ /^\Q$base\E/i && $cur =~ /^\Q$base\E/i)) {
        return $abs;
    }

    while (1)
    {
       # keep cutting off common prefixes until we can
       $abs =~ m|^(.*?/)|;
       my $prefix = $1;
       last if ($prefix eq '');
       if ($cur =~ /^\Q$prefix\E/i) {
           $abs =~ s/^\Q$prefix\E//i;
           $cur =~ s/^\Q$prefix\E//i;
       } else {
           last;
       }
    }


    # assemble relative path: change every directory name in $cur to "..",
    # then add $abs to it.
    $cur =~ s|[^/]+|..|g;
    my $rel = $cur.$abs;

    return $rel;
}


sub quote($)
{
    my($dir) = @_;
    if ($dir =~ / /) {$dir = "\"$dir\"";}
    return $dir;
}

sub template()
{
    # NOTE: the following template must be kept in sync with the
    # "Makefile.TEMPLATE" file in the org.omnetpp.cdt plug-in!
    return <<'ENDTEMPLATE'
#
# OMNeT++/OMNEST Makefile for {target}
#
# This file was generated with the command:
#  {progname} {args}
#


# Name of target to be created (-o option)
TARGET = {target}

# User interface (uncomment one) (-u option)
{~allenv:#}USERIF_LIBS = $(TKENV_LIBS) $(CMDENV_LIBS)
{~cmdenv:#}USERIF_LIBS = $(CMDENV_LIBS)
{~tkenv:#}USERIF_LIBS = $(TKENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = {includepath}

# misc additional object and library files to link with
EXTRA_OBJS = {extraobjs}

# object files from other directories to link with
EXT_DIR_OBJS = {extdirobjs}

# time stamps of other directories (used as dependency)
EXT_DIR_TSTAMPS = {extdirtstamps}

# Additional libraries (-L, -l, -t options)
LIBS = {libs}

#------------------------------------------------------------------------------

!include {configfile}

# User interface libs
CMDENV_LIBS = {-u}_cmdenv_lib {-l}envir{.lib} {-l}cmdenv{.lib}
TKENV_LIBS = {-u}_tkenv_lib {-l}envir{.lib} {-l}tkenv{.lib} {-l}layout{.lib} $(TK_LIBS) $(ZLIB_LIBS)

# Simulation kernel
KERNEL_LIBS = {-l}common{.lib} {-l}sim_std{.lib}

{nmake:}!if "$(WITH_NETBUILDER)"=="yes"
{~nmake:}ifeq($(WITH_NETBUILDER),yes)
KERNEL_LIBS = $(KERNEL_LIBS) {-l}nedxml{.lib} $(XML_LIBS)
{nmake:!}endif

{nmake:}!if "$(WITH_PARSIM)"=="yes"
{~nmake:}ifeq($(WITH_PARSIM),yes)
KERNEL_LIBS = $(KERNEL_LIBS) $(MPI_LIBS)
{nmake:!}endif

# Simulation kernel and user interface libraries
OMNETPP_LIBS = {-L}$(OMNETPP_LIB_DIR) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) {fordllopt} $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
NEDCOPTS = $(COPTS) $(NEDCFLAGS)
MSGCOPTS = $(INCLUDE_PATH) {dllexportmacro}

#------------------------------------------------------------------------------
# object files in this directory
OBJS = {objs}

# subdirectories to recurse into
SUBDIRS = {subdirs}
{nmake:}SUBDIR_TARGETS = {subdirtargets}

{makefrags}

{exe:}$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) {hassubdir:subdirs} {makefile}
{exe:}	$(LINK) $(LDFLAGS) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS) $(LIBS) $(OMNETPP_LIBS) {link-o}$(TARGET)
{exe:}	echo{nmake:.} >.tstamp
{so:}$(TARGET): $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_TSTAMPS) {hassubdir:subdirs} {makefile}
{so:}	$(SHLIB_LD) -o $(TARGET) $(OBJS) $(EXTRA_OBJS) $(EXT_DIR_OBJS)
{so:}	echo{nmake:.} >.tstamp
{nolink:}$(TARGET): $(OBJS) {hassubdir:subdirs} {makefile} .tstamp
{nolink:}	@{nmake:rem}{~nmake:#} Do nothing
{nolink:}
{nolink:}.tstamp: $(OBJS)
{nolink:}	echo{nmake:.} >.tstamp

subdirs: $(SUBDIR_TARGETS)

{subdirtargets}:
{~nmake:}	cd $@ && {makecommand}
{nmake:}	cd $(@:{_dir}=) && echo [Entering $(@:{_dir}=)] && {makecommand} && echo [Leaving $(@:{_dir}=)]

{nmake:}{msgccandhfiles} :: {msgfiles}
{~nmake:}%_m.{cc} %_m.h: %.msg
	$(MSGC{nmake::/=\}) -s _m.{cc} $(MSGCOPTS) $**

.SUFFIXES: .{cc}

.{cc}.obj:
	$(CXX) -c $(COPTS) {nmake:/Tp} $<

generateheaders: $(GENERATEDHEADERS)
{nmake:}	@if not "$(SUBDIRS)"=="" for %%i in ( $(SUBDIRS) ) do @cd %%i && echo [opp_msgc in %%i] && nmake /nologo /f Makefile.vc generateheaders && cd .. || exit /b 1
{~nmake:}	for i in $(SUBDIRS); do (cd \$\$i && \$(MAKE) generateheaders) || exit 1; done

clean:
{nmake:}	-del *.obj .tstamp *.idb *.pdb *.ilk *.exp $(TARGET) $(TARGET:.exe=.lib) $(TARGET:.dll=.lib) 2>NUL
{nmake:}	-del *_n.{cc} *_n.h *_m.{cc} *_m.h 2>NUL
{nmake:}	-del *.vec *.sca 2>NUL
{nmake:}	-if not "$(SUBDIRS)"=="" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [clean in %%i] && nmake /nologo /f Makefile.vc clean && cd .. || exit /b 1
{~nmake:}	rm -f *.o *_n.{cc} *_n.h *_m.{cc} *_m.h .tstamp
{~nmake:}	rm -f *.vec *.sca
{~nmake:}	for i in $(SUBDIRS); do (cd $$i && $(MAKE) clean); done

depend:
	$(MAKEDEPEND) $(INCLUDE_PATH) -f Makefile.vc -- *.{cc}
{nmake:}	if not "$(SUBDIRS)"=="" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [depend in %%i] && nmake /nologo /f Makefile.vc depend && cd .. || exit /b 1
{~nmake:}	for i in $(SUBDIRS); do (cd $$i && $(MAKE) depend) || exit 1; done

makefiles:
	{progname} {args}
{nmake:}	if not "$(SUBDIRS)"=="" for %%i in ( $(SUBDIRS) ) do cd %%i && echo [makemake in %%i] && nmake /nologo /f Makefile.vc makefiles && cd .. || exit /b 1
{~nmake:}	for i in $(SUBDIRS); do (cd $$i && $(MAKE) makefiles) || exit 1; done

# DO NOT DELETE THIS LINE -- make depend depends on it.
{deps}
ENDTEMPLATE
}
