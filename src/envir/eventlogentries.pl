#
# Generates eventlog writer code from eventlogentries.txt
#
# Author: Andras Varga, 2006
#

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
      print "$classCode $className\n";
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      print "{\n";
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+)( +opt)? *$/)
   {
      $fieldCode = $1;
      $fieldType = $2;
      $fieldName = $3;

      if ($fieldType eq "string")
      {
         $fieldPrintfType = "%s";
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
      }

      $fieldCType = $fieldType;
      $fieldCType =~ s/string/const char */;
      $field = {
         CODE => $fieldCode,
         TYPE => $fieldType,
         CTYPE => $fieldCType,
         PRINTFTYPE => $fieldPrintfType,
         NAME => $fieldName,
      };
      push(@fields, $field);
      print " $fieldCode $fieldType $fieldName\n";
   }
   elsif ($_ =~ /^ *} *$/)
   {
      $class = {
         CODE => $classCode,
         NAME => $className,
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

print H
"//=========================================================================
//  EVENTLOGWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#ifndef __EVENTLOGWRITER_H_
#define __EVENTLOGWRITER_H_

class EventLogWriter
{
  public:
";

foreach $class (@classes)
{
   print H "    static void record$class->{NAME}(FILE *f";

   foreach $field (@{ $class->{FIELDS} })
   {
      print H ", $field->{CTYPE} $field->{NAME}";
   }
   print H ");\n";
}

print H "
};

#endif
";

close(H);



#
# Write eventlogwriter.cc file
#

open(CC, ">eventlogwriter.cc");

print CC
"//=========================================================================
//  EVENTLOGWRITER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#include \"eventlogwriter.h\"

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw new cRuntimeError(\"Cannot write event log file, disk full?\");


";

#FIXME: use SIMTIME_STR(), quote strings, etc! and change omnetapp.cc to use this

foreach $class (@classes)
{
   print CC "void EventLogWriter::record$class->{NAME}(FILE *f";
   foreach $field (@{ $class->{FIELDS} })
   {
      print CC ", $field->{CTYPE} $field->{NAME}";
   }
   print CC ")\n";
   print CC "{\n";
   print CC "    CHECK(fprintf(f, \"$class->{CODE}";
   foreach $field (@{ $class->{FIELDS} })
   {
      print CC " $field->{CODE} $field->{PRINTFTYPE}";
   }
   print CC "\\n\"";
   foreach $field (@{ $class->{FIELDS} })
   {
      print CC ", $field->{NAME}";
   }
   print CC "));\n";
   print CC "}\n\n";
}

close(CC);


sub makeMethodDecl ()
{
    my $class = shift;
    my @fields = shift;
}
