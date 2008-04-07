#
# Generates eventlog writer code from eventlogentries.txt
#
# Author: Andras Varga, 2006
#

#FIXME: make use of "default value" column!!

$verbose = 0;

open(FILE, "../eventlog/eventlogentries.txt");


#
# Read input file
#
while (<FILE>)
{
   chomp;
   s/\r$//; # cygwin/mingw perl does not do CR/LF translation

   if ($_ =~ /^ *$/)
   {
      # blank line
   }
   elsif ($_ =~ /^ *\/\//)
   {
      # comment
   }
   elsif ($_ =~ /^([\w]+) +([\w]+) *(\/\/ *([\w ]+))?$/)
   {
      $classCode = $1;
      $className = $2;
      $classHasOptField = 0;
      print "$classCode $className\n" if ($verbose);
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      print "{\n" if ($verbose);
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+)( +([^ ]+))? *(\/\/ *([\w-+ ]+))?$/)
   {
      $fieldCode = $1;
      $fieldType = $2;
      $fieldName = $3;
      $fieldDefault  = $5;
      $fieldPrintfValue = $fieldName;

      if ($fieldType eq "string")
      {
         $fieldPrintfType = "%s";
         $fieldPrintfValue = "QUOTE($fieldPrintfValue)";
      }
      elsif ($fieldType eq "bool")
      {
         $fieldPrintfType = "%d";
      }
      elsif ($fieldType eq "int")
      {
         $fieldPrintfType = "%d";
      }
      elsif ($fieldType eq "short")
      {
         $fieldPrintfType = "%d";
      }
      elsif ($fieldType eq "long")
      {
         $fieldPrintfType = "%ld";
      }
      elsif ($fieldType eq "int64")
      {
         $fieldPrintfType = '%"LL"d';
      }
      elsif ($fieldType eq "simtime_t")
      {
         $fieldPrintfType = "%s";
         $fieldPrintfValue = "SIMTIME_STR($fieldPrintfValue)";
      }
      else {
         die "unrecognized type '$fieldType'";
      }

      if ($fieldDefault ne "") {
         $classHasOptField = 1;
      }

      $fieldCType = $fieldType;
      $fieldCType =~ s/string/const char */;
      $field = {
         CODE => $fieldCode,
         TYPE => $fieldType,
         CTYPE => $fieldCType,
         PRINTFTYPE => $fieldPrintfType,
         PRINTFVALUE => $fieldPrintfValue,
         NAME => $fieldName,
         DEFAULTVALUE => $fieldDefault,
      };

      push(@fields, $field);
      print " $fieldCode $fieldType $fieldName $fieldDefault\n" if ($verbose);
   }
   elsif ($_ =~ /^ *} *$/)
   {
      $class = {
         CODE => $classCode,
         NAME => $className,
         HASOPT => $classHasOptField,
         FIELDS => [ @fields ],
      };
      push(@classes, $class);
      @fields = ();
      print "}\n" if ($verbose);
   }
   else
   {
       die "unrecognized line \"$_\"";
   }
}

close(FILE);



#
# Write eventlogwriter.h file
#

open(H, ">eventlogwriter.h");

print H makeFileBanner("eventlogwriter.h");
print H
"#ifndef __EVENTLOGWRITER_H_
#define __EVENTLOGWRITER_H_

#include <stdio.h>
#include \"envirdefs.h\"
#include \"cexception.h\"

USING_NAMESPACE

class EventLogWriter
{
  public:
    static void recordLogLine(FILE *f, const char *s, int n);
";

foreach $class (@classes)
{
   print H "    static void " . makeMethodDecl($class,0) . ";\n";
   print H "    static void " . makeMethodDecl($class,1) . ";\n" if ($class->{HASOPT});
}

print H "};

#endif
";

close(H);



#
# Write eventlogwriter.cc file
#

open(CC, ">eventlogwriter.cc");

print CC makeFileBanner("eventlogwriter.cc");
print CC "
#include \"eventlogwriter.h\"
#include \"stringutil.h\"

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError(\"Cannot write event log file, disk full?\");

#define LL    INT64_PRINTF_FORMAT


void EventLogWriter::recordLogLine(FILE *f, const char *s, int n)
{
    CHECK(fprintf(f, \"- \"));
    CHECK(fwrite(s, 1, n, f));
}

";

foreach $class (@classes)
{
   print CC makeMethodImpl($class,0);
   print CC makeMethodImpl($class,1) if ($class->{HASOPT});
}

close(CC);

sub makeMethodImpl ()
{
   my $class = shift;
   my $wantOptFields = shift;

   my $txt = "void EventLogWriter::" . makeMethodDecl($class,$wantOptFields) . "\n{\n";

   # class code goes into initial fprintf
   my $fmt .= "$class->{CODE}";
   $fmt = "\\n".$fmt if ($class->{CODE} eq "E");
   my $args = "";

   foreach $field (@{ $class->{FIELDS} })
   {
      # if wantOptFields==false, skip optional fields
      next if (!$wantOptFields && $field->{DEFAULTVALUE} ne "");

      if ($field->{DEFAULTVALUE} eq "")
      {
         # mandatory field: append to current fprintf statement
         $fmt .= " $field->{CODE} $field->{PRINTFTYPE}";
         $args .= ", $field->{PRINTFVALUE}";
      }
      else
      {
         # optional field: flush current fprintf statement, and generate a conditional fprintf
         $txt .= "    CHECK(fprintf(f, \"$fmt\"$args));\n" if ($fmt ne "");
         $fmt = "";
         $args = "";
         $txt .= "    if ($field->{NAME}!=$field->{DEFAULTVALUE})\n";
         $txt .= "        CHECK(fprintf(f, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{PRINTFVALUE}));\n";
      }
   }
   # flush final fprintf statement (or at least a newline if $fmt=="")
   $txt .= "    CHECK(fprintf(f, \"$fmt\\n\"$args));\n";

   $txt .= "}\n\n";
   $txt;
}

sub makeMethodDecl ()
{
   my $class = shift;
   my $wantOptFields = shift;

   my $txt = "record$class->{NAME}";
   foreach $field (@{ $class->{FIELDS} })
   {
      my $code = ($field->{CODE} eq "#") ? "e" : $field->{CODE};
      $txt .= "_$code" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
   }
   $txt .= "(FILE *f";
   foreach $field (@{ $class->{FIELDS} })
   {
      $txt .= ", $field->{CTYPE} $field->{NAME}" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
   }
   $txt .= ")";
   $txt;
}

sub makeFileBanner ()
{
    my $ucfilename = uc(shift);
    return
"//=========================================================================
// $ucfilename - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

";
}


