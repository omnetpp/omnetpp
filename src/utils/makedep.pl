#
# makedep.pl
#
#       This perl script will update dependencies in a makefile, or
#       print dependencies to the standard output
#
# Usage:
#
#       % makedep.pl [flags] <file_list>
#
# where possible flags are:
#   -Y
#   -I<include-dir>
#   -I <include-dir>
#   --stdout
#   --makefile <makefile-name>
#   --objsuffix <object-file-suffix>
#   --objdir <object-file-directory>
#   --srcdir <source-directory>
#
# <file_list> is the list of C/C++ files for which dependencies must be generated.
#
# -Y causes makedep to ignore (and also leave out from the dependencies)
#    the include files that cannot be found.
#
# -I adds directories to the include search path
#
# --stdout causes dependencies to be written to the standard output,
#    (instead of updating a makefile)
#
# --objsuffix sets the suffix of the object filename. It defaults to ".o".
#
# --objdir sets the directory of the object filename.  Defaults to current dir.
#
# Bug: difference between #include <foo.h> and #include "foo.h" is ignored.
#
# Based on the script at:
#  http://faqchest.dynhost.com/prgm/perlu-l/perl-98/perl-9802/perl-980200/perl98020418_25228.html
#
# Rewritten by Andras Varga, 2001
#


# the #include syntax we are looking for
$Syntax = '^\s*#\s*include\s+["<]([^">]*)[">]' ;

# marker line in the makefile
$Separator = '# DO NOT DELETE THIS LINE -- make depend depends on it.';

# IncludePath is a list of the directories to search for includes
@IncludePath = ();

# CFileList is a list of files that need to be scanned for #include
@CFileList = ();

# Below is the default suffix for the object file printed in the
# dependency list.  This default is overriden with --objsuffix
#
$ObjSuffix = ".o";
$SourceDirectory = "";

# If this flag is set, include files that are not found in the search path are
# ignored. (Otherwise they cause an error.) The -Y option sets the flag to 1.
$IgnoreStdHeaders = 0;

# If set, writes dependencies to standard output instead up updating a makefile
$UseStdout = 0;

# Name of the makefile in which the dependencies should be substituted
$Makefile = "Makefile";


#
#  This first thing this script does is parse the command line
#  for Include Path specifications, options, and files.
#
while (@ARGV)
{
    $arg = shift @ARGV;

    if ($arg eq "-I")
    {
        $dir_name = shift @ARGV;
        push(@IncludePath, $dir_name);
    }
    elsif ($arg =~ /^-I/)
    {
        $arg =~ s/-I//;
        push(@IncludePath, $arg);
    }
    elsif ($arg eq "-Y")
    {
        $IgnoreStdHeaders = 1;
    }
    elsif ($arg eq "--stdout")
    {
        $UseStdout = 1;
    }
    elsif ($arg eq "--makefile")
    {
        $Makefile = shift @ARGV;
    }
    elsif ($arg eq "--objsuffix")
    {
        $ObjSuffix = shift @ARGV;
    }
    elsif ($arg eq "--objdir")
    {
        $ObjDir = shift @ARGV;
    }
    elsif ($arg eq "--srcdir")
    {
        $SourceDirectory = shift @ARGV;
    }
    elsif ($arg =~ /^-/)
    {
        print STDERR "makedep: ignoring unknown flag $arg\n";
    }
    else
    {
        # otherwise we have a file name that needs to be added to the file list
        push(@CFileList, $arg);
    }
}

#
# for each file in the file list
#    scan it for #includes
#    print out the list of #includes
#
$Deps = "";
for (@CFileList)
{
    @Includes = ();
    &scan( $_ );
    $Deps .= &print_dep( $_, @Includes );
}
if ($UseStdout) {
    print $Deps;
} else {
    update_makefile( $Deps );
}
exit(0);

#
# scan
#
#       This function is the guts of the dependency generation.
#  It is a recursive function that searches for #include's in
#  the file list.  If it runs into an include file, it then opens that file to
#  search for more #include's.
#
sub scan {
   local( $filename ) = @_ ;
   local( $included, $found, $_ );
   local( @list ) = ();
   local($IncludeDir);
   local($IncludePathName);

   if (!-e $filename)
   {
       $filename = $SourceDirectory . $filename;
   }
   if (!open( INPUT, $filename ))
   {
       print STDERR "makedep: unable to open $filename: $!\n" ;
       exit(2);
   }

   while ( <INPUT> ) {
      if ( /${Syntax}/ ) {
         $included = $1;
         if ( $included !~ /^[.\/]/ ) {

            undef $found;
            # if the include file name doesn't exist
            # in the current directory,
            # then check the include path for the file.
            if (-e $included)
            {
                $found = $included;
            }
            else
            {
                foreach $IncludeDir (@IncludePath)
                {
                    $IncludePathName = "$IncludeDir/$included";
                    if (-e $IncludePathName)
                    {
                        $found = $IncludePathName;
                        last;
                    }
                }
            }

            if ( defined($found) )
            {
                $included = $found;
            }
            elsif ($IgnoreStdHeaders)
            {
                next;
            }
            else
            {
                print STDERR "makedep: file not in include path: $included\n";
                exit(1);
            }

         }

         ## see if its already on the list
         undef $found ;
         for ( $[ .. $#Includes ) {
            $found = $_, last if ( $Includes[$_] eq $included );
         }

         ## nope! put it on
         if ( ! defined( $found ) ) {
            push( @Includes, $included );
            push( @list, $included );
         }
      }
   }
   close( INPUT );

   ## scan through each of the files that turned up
   for ( @list ) {
      &scan( $_ );
   }
}

#
# Return dependencies as a string ready to be inserted into a makefile
#
sub print_dep {
   local( $filename, @includes ) = @_ ;
   local( $_ );
   local( $dep );

   $target = &get_file($filename);

   $target =~ s/\.[^.]*$/$ObjSuffix/ ;

   if (defined($ObjDir))
   {
        $target = $ObjDir . "/" . $target;
   }

   if ( $filename !~ /^[.\/]/ ) {
      $filename = $filename;
   }

   $dep = "${target}: ";
   $dep .= join( " \\\n  ", ("$SourceDirectory$filename", @includes) );
   $dep .= "\n";
   return $dep;
}

#
#       This function takes a string in the form of a directory path
# and returns a string that is just the file part.  Given a string of the
# form "./some/path/witha/filename", "filename" will be returned.
#
sub get_file {
    local($base) = @_;
    local($splitdir);
    local($file);

    @splitdir = split('/', $base);
    $file = $splitdir[$#splitdir];
}

#
# Update makefile with the dependencies.
#
sub update_makefile {
    local($deps) = @_;

    # read makefile
    $makefiletext = "";
    open(INPUT, $Makefile) || die "makedep: cannot open $Makefile: $!";
    while (<INPUT>) {
      $makefiletext .= $_;
    }
    close(INPUT);

    # hack in dependencies
    $makefiletext =~ s/\n${Separator}.*//s;
    $makefiletext .= "\n".$Separator."\n".$deps;

    # write it back into a temp file (overwriting directly the original
    # file is not safe -- what if we're aborted during write?)
    $tmpmakefile = $Makefile.".tmp";
    open(OUT, ">$tmpmakefile") || die "makedep: cannot write $tmpmakefile: $!";
    print OUT $makefiletext;
    close(OUT);

    # replace makefile with temp file.
    # but under windows, rename chokes if target file already exists.
    # thus, try unix way first, and if it fails, do in the windows way
    if (!rename($tmpmakefile, $Makefile)) {
        unlink $Makefile || die "makedep: cannot replace makefile $Makefile: $!";
        rename($tmpmakefile, $Makefile) || die "makedep: cannot replace makefile $Makefile: $!";
    }
}


