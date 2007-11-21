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

print template();


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
$nmake = $isWindows && !isCygwin;
$outfile = cwd;
$outfile  =~ s/[\/\\]$//;
$outfile  =~ s/.*[\/\\]//;

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
$cfgfile="";
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

$args = join(' ',@ARGV);

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
        $cfgfile = shift @ARGV;
        $cfgfile = abs2rel($cfgfile,$basedir);
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
if ($cfgfile eq "")
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
             $cfgfile = $f;
             last;
        }
    }
    if ($cfgfile eq "") {
        print STDERR "$progname: warning: configuser.vc file not found -- try -c option or edit generated makefile\n";
    }

}
else
{
    if (! -f $cfgfile) {
        print STDERR "$progname: error: file $cfgfile not found\n";
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


#
# now the Makefile creation
#
$dir=cwd;
print "Creating $makefile in $dir...\n";

open(OUT, ">$makefile");

print OUT "#
#  Makefile for $outfile
#
#  ** This file was automatically generated by the command:
#  $ARG0 $args
#
";

$suffix = $nmake ? '.exe' : '';
if ($type eq "so")
{
    $suffix = $nmake ? '.dll' : '.so';
}
if ($type eq "o")
{
    $suffix = '';
}

if ($userif eq 'ALL')
{
    $c_all = '';
    $c_cmd = '# ';
    $c_tk = '# ';
}
elsif ($userif eq 'CMDENV')
{
    $c_all = '#';
    $c_cmd = '';
    $c_tk = '# ';
}
elsif ($userif eq 'TKENV')
{
    $c_all = '#';
    $c_cmd = '# ';
    $c_tk = '';
}

$sobjs = join( " \\\n            ", @sobj_list);
$xobjs = join( " \\\n            ", @xobjs);
$extdirobjs = join( " \\\n            ", @extdirobjs);
$extdirtstamps = join( " \\\n            ", @extdirtstamps);

$fordllopt = '';
if ($fordll) {
    $fordllopt = '/DWIN32_DLL';
}

$linkeru = '';

print OUT "

# Name of target to be created (-o option)
TARGET = $outfile$suffix

# User interface (uncomment one) (-u option)
${c_all}USERIF_LIBS=\$(TKENV_LIBS) \$(CMDENV_LIBS)
${c_cmd}USERIF_LIBS=\$(CMDENV_LIBS)
${c_tk}USERIF_LIBS=\$(TKENV_LIBS)

# .ned or .h include paths with -I
INCLUDE_PATH=$includes

# misc additional object and library files to link
EXTRA_OBJS=$xobjs

# object files from other directories to link with
EXT_DIR_OBJS=$extdirobjs

# time stamps of other directories (used as dependency)
EXT_DIR_TSTAMPS=$extdirtstamps

# Additional libraries (-L, -l, -t options)
LIBS=$libpath$libs$importlibs

#------------------------------------------------------------------------------
";

# Makefile
print OUT "

!include \"$cfgfile\"

# User interface libs
CMDENV_LIBS=/include:_cmdenv_lib envir.lib cmdenv.lib
TKENV_LIBS=/include:_tkenv_lib envir.lib tkenv.lib layout.lib \$(TK_LIBS) \$(ZLIB_LIBS)

# Simulation kernel
KERNEL_LIBS=common.lib sim_std.lib

!if \"\$(WITH_NETBUILDER)\"==\"yes\"
KERNEL_LIBS= \$(KERNEL_LIBS) nedxml.lib \$(XML_LIBS)
!endif

!if \"\$(WITH_PARSIM)\"==\"yes\"
KERNEL_LIBS= \$(KERNEL_LIBS) \$(MPI_LIBS)
!endif

# Simulation kernel and user interface libraries
OMNETPP_LIBS=/libpath:\$(OMNETPP_LIB_DIR) \$(USERIF_LIBS) \$(KERNEL_LIBS) \$(SYS_LIBS)

COPTS=\$(CFLAGS) $fordllopt \$(INCLUDE_PATH) -I\$(OMNETPP_INCL_DIR)
NEDCOPTS=\$(COPTS) \$(NEDCFLAGS)
MSGCOPTS=$exportdefopt \$(INCLUDE_PATH)

#------------------------------------------------------------------------------
";

# rules for $(TARGET)
$objs = join(" ", @objs);
print OUT "# object files in this directory\n";
print OUT "OBJS= $objs\n";
print OUT "\n";

$generatedheaders = join(" ", @generatedheaders);
print OUT "# header files generated (from msg files)\n";
print OUT "GENERATEDHEADERS= $generatedheaders\n";
print OUT "\n";

$subdirs = join(" ", @subdirs);
print OUT "# subdirectories to recurse into\n";
print OUT "SUBDIRS= $subdirs\n";
print OUT "SUBDIR_TARGETS= ";
foreach $i (@subdirs) {print OUT " ${i}_dir";}
print OUT "\n\n";

# include external Makefile fragments at top, so that they can
# override the default target if they want
if (@fragments != ())
{
    foreach $frag (@fragments)
    {
        print OUT "# inserted from file '$frag':\n";
        open(FRAG, $frag);
        while(<FRAG>)  {print OUT;}
        close FRAG;
        print OUT "\n\n";
    }
}
else
{
    if (-f "makefrag.vc")
    {
        print OUT "# inserted from file 'makefrag.vc':\n";
        open(FRAG,"makefrag.vc");
        while(<FRAG>)  {print OUT;}
        close FRAG;
        print OUT "\n\n";
    }
}

# note: if "subdirs" were always among the target dependencies, it would cause
# the executable to re-link every time (which is not good).
if ($recursive)
{
    $subdirs_target='subdirs';
}
else
{
    $subdirs_target='';
}

if ($type eq 'exe')
{
    print OUT "\$(TARGET): \$(OBJS) \$(EXTRA_OBJS) \$(EXT_DIR_TSTAMPS) $subdirs_target $makefile\n";
    print OUT "\t\$(LINK) \$(LDFLAGS) \$(OBJS) \$(EXTRA_OBJS) \$(EXT_DIR_OBJS) \$(LIBS) \$(OMNETPP_LIBS) /out:\$(TARGET)\n";
    print OUT "\t\@echo.>.tstamp\n";
}
elsif ($type eq 'so')
{
    print OUT "\$(TARGET): \$(OBJS) \$(EXTRA_OBJS) \$(EXT_DIR_TSTAMPS) $subdirs_target $makefile\n";
    print OUT "\t\$(SHLIB_LD) \$(OBJS) \$(EXTRA_OBJS) \$(EXT_DIR_OBJS) \$(LIBS) /out:\$(TARGET)\n";
    print OUT "\t\@echo.>.tstamp\n";
}
elsif ($type eq 'o')
{
    print OUT "\$(TARGET): \$(OBJS) $makefile $subdirs_target .tstamp\n";
    print OUT "\n";
    print OUT ".tstamp: \$(OBJS)\n";
    print OUT "\t\@echo.>.tstamp\n";
}
print OUT "\n";

if (@objs != ())
{
    print OUT "\$(OBJS) : \$(GENERATEDHEADERS)\n";
    print OUT "\n";
}

if (@extdirtstamps != ()) {
    print OUT "\$(EXT_DIR_TSTAMPS):\n";
    print OUT "\t\@echo Error: \$(\@:/=\\) does not exist.\n";
    print OUT "\t\@echo This means that at least the above dependency directory has not been built.\n";
    print OUT "\t\@echo Maybe you need to do a top-level make?\n";
    print OUT "\t\@echo.\n";
    print OUT "\t\@exit /b 1\n";
    print OUT "\n";
}

# rules for recursive build
# simplified because @#$!? microsoft doesn't know about phony targets
print OUT "subdirs: \$(SUBDIR_TARGETS)\n";
print OUT "\n";

foreach $i (@subdirs)
{
   print OUT "${i}_dir:\n";
   print OUT "\tcd $i && echo [Entering $i] && $makecommand && echo [Leaving $i]\n\n";
}

# rules for ned files
if ($ignorened eq 0)
{
    foreach $i (glob("*.ned"))
    {
        $c = $i;
        $c =~ s/\.ned/_n.$ccext/g;

        print OUT "$c: $i\n";
        print OUT "\t\$(NEDC:/=\\) -s _n.$ccext \$(INCLUDE_PATH) \$<\n";
        print OUT "\n";
    }
}

# rules for msg files (cannot use suffix rule because of "_m"; cannot use "$<" either)
foreach $i (glob("*.msg"))
{
    $c = $i;
    $c =~ s/\.msg/_m.$ccext/g;
    $h = $i;
    $h =~ s/\.msg/_m.h/g;

    print OUT "$c $h: $i\n";
    print OUT "\t\$(MSGC:/=\\) -s _m.$ccext \$(MSGCOPTS) $i\n";
    print OUT "\n";
}

# rules for C++ files (suffix rule)
print OUT ".SUFFIXES: .$ccext\n";
print OUT "\n";
print OUT ".$ccext.obj:\n";
print OUT "\t\$(CXX) -c \$(COPTS) /Tp \$<\n";
print OUT "\n";

# documentation targets
print OUT "
#doc: neddoc doxy

#neddoc:
#\t\@opp_neddoc -a

#doxy: $doxyconf
#\t\@doxygen $doxyconf
";

# prevent accumulating of "-f" options during subsequent remakes
$args1 = " $args ";
$args1 =~ s/ -f / /g;

# rule for clean and other recursive targets
# note funky `if not "$(SUBDIRS)"=="" ...' -- it is necessary because
# cmd.exe thinks that no "for" args is an error (!?!?)
print OUT "
generateheaders: \$(GENERATEDHEADERS)
\t\@if not \"\$(SUBDIRS)\"==\"\" for %%i in ( \$(SUBDIRS) ) do \@cd %%i && echo [opp_msgc in %%i] && $makecommand generateheaders && cd .. || exit /b 1

clean:
";

print OUT "\t-del *.obj .tstamp *.idb *.pdb *.ilk *.exp \$(TARGET) \$(TARGET:.exe=.lib) \$(TARGET:.dll=.lib) 2>NUL
\t-del *_n.$ccext *_n.h *_m.$ccext *_m.h 2>NUL
\t-del *.vec *.sca 2>NUL
\t-for %%i in ( \$(SUBDIRS) ) do cd %%i && echo [clean in %%i] && $makecommand clean && cd .. || exit /b 1

depend:
\t\$(MAKEDEPEND) \$(INCLUDE_PATH) -f $makefile -- *.$ccext
\tif not \"\$(SUBDIRS)\"==\"\" for %%i in ( \$(SUBDIRS) ) do cd %%i && echo [depend in %%i] && $makecommand depend && cd .. || exit /b 1

makefiles:
\t$progname -f $args1
\tif not \"\$(SUBDIRS)\"==\"\" for %%i in ( \$(SUBDIRS) ) do cd %%i && echo [makemake in %%i] && $makecommand makefiles && cd .. || exit /b 1

# \"re-makemake\" is a deprecated, historic name of the above target
re-makemake: makefiles

# DO NOT DELETE THIS LINE -- make depend depends on it.

";

close OUT;

print "$makefile created.\n";
print "Please type `nmake -f $makefile depend' NOW to add dependencies!\n";

#----

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
