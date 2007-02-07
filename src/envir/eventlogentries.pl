#
# Generates eventlog writer code from eventlogentries.txt
#
# Author: Andras Varga, 2006
#

#FIXME: make use of "default value" column!!

open(FILE, "../eventlog/eventlogentries.txt");


#
# Read input file
#
while (<FILE>)
{
   chomp;
   if ($_ =~ /^ *$/)
   {
      # blank line
   }
   elsif ($_ =~ /^ *\/\//)
   {
      # comment
   }
   elsif ($_ =~ /^([\w]+) +([\w]+) *$/)
   {
      $classCode = $1;
      $className = $2;
      $classHasOptField = 0;
      print "$classCode $className\n";
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      print "{\n";
   }
   elsif ($_ =~ /^ +([\w]+) +([\w]+) +([\w]+)( +([^ ]+))? *$/)
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
      elsif ($fieldType eq "long")
      {
         $fieldPrintfType = "%ld";
      }
      elsif ($fieldType eq "int")
      {
         $fieldPrintfType = "%d";
      }
      elsif ($fieldType eq "simtime_t")
      {
         $fieldPrintfType = "%s";
         $fieldPrintfValue = "SIMTIME_STR($fieldPrintfValue)";
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
      print " $fieldCode $fieldType $fieldName $fieldDefault\n";
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
      print "}\n";
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
#include \"envdefs.h\"
#include \"cexception.h\"

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
#define CHECK(fprintf)    if (fprintf<0) throw new cRuntimeError(\"Cannot write event log file, disk full?\");

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

   my $txt = "void EventLogWriter::" . makeMethodDecl($class,$wantOptFields) . "\n";
   $txt .= "{\n";
   $nl = ($class->{CODE} eq "E") ? "\\n" : "";
   $txt .= "    CHECK(fprintf(f, \"$nl$class->{CODE}";
   foreach $field (@{ $class->{FIELDS} })
   {
      $txt .= " $field->{CODE} $field->{PRINTFTYPE}" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
   }
   $txt .= "\\n\"";
   foreach $field (@{ $class->{FIELDS} })
   {
      $txt .= ", $field->{PRINTFVALUE}" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
   }
   $txt .= "));\n";
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
      $txt .= "_$field->{CODE}" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
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
    my $ufilename = uc(shift);
    return
"//=========================================================================
// $ufilename - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

";
}


