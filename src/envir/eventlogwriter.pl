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
   elsif ($_ =~ /^([\w]+) +([\w]+)( +: +([\w]+))? *(\/\/.*)?$/)
   {
      $classCode = $1;
      $className = $2;
      $classSuper = $4;
      if ($classSuper eq "")
      {
         $classSuper = "EventLogTokenBasedEntry";
      }
      $classComment = $5;
      $classHasOptField = 0;
      print "$classCode $className\n" if ($verbose);
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      print "{\n" if ($verbose);
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+)( +([^ ]+))? *(\/\/.*)?$/)
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
      elsif ($fieldType eq "int64_t")
      {
         $fieldPrintfType = '%" PRId64 "';
      }
      elsif ($fieldType eq "eventnumber_t")
      {
         $fieldPrintfType = '%" PRId64 "';
      }
      elsif ($fieldType eq "msgid_t")
      {
         $fieldPrintfType = '%" PRId64 "';
      }
      elsif ($fieldType eq "txid_t")
      {
         $fieldPrintfType = '%" PRId64 "';
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
         SUPER => $classSuper,
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
"#ifndef __OMNETPP_ENVIR_EVENTLOGWRITER_H
#define __OMNETPP_ENVIR_EVENTLOGWRITER_H

#include <cstdio>
#include \"envirdefs.h\"
#include \"omnetpp/simtime_t.h\"

namespace omnetpp {
namespace envir {

class EventLogWriter
{
  public:
    static void recordLogLine(FILE *f, const char *prefix, const char *line, int lineLength);
";

foreach $class (@classes)
{
   print H "    static void " . makeMethodDecl($class,0) . ";\n";
   print H "    static void " . makeMethodDecl($class,1) . ";\n" if (getEffectiveHasOpt($class));
}

print H "};

} // namespace envir
}  // namespace omnetpp

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
#include \"common/stringutil.h\"
#include \"omnetpp/cconfigoption.h\"
#include \"omnetpp/csimulation.h\"
#include \"omnetpp/cmodule.h\"
#include \"omnetpp/cexception.h\"

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError(\"Cannot write event log file, disk full?\");

namespace omnetpp {
namespace envir {

using namespace omnetpp::common;

void EventLogWriter::recordLogLine(FILE *f, const char *prefix, const char *line, int lineLength)
{
    CHECK(fprintf(f, \"- %s\", prefix));
    CHECK(fwrite(line, 1, lineLength, f));
}

";

foreach $class (@classes)
{
   print CC makeMethodImpl($class,0);
   print CC makeMethodImpl($class,1) if (getEffectiveHasOpt($class));
}

print CC "
} // namespace envir\n
}  // namespace omnetpp
";

close(CC);

sub makeMethodImpl ()
{
   my $class = shift;
   my $wantOptFields = shift;

   my $txt = "void EventLogWriter::" . makeMethodDecl($class,$wantOptFields) . "\n{\n";
   $txt .= "    ASSERT(f!=nullptr);\n";

   # class code goes into initial fprintf
   my $fmt .= "$class->{CODE}";
   my $args = "";

   foreach $field ( getEffectiveFields($class) )
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
   foreach $field ( getEffectiveFields($class) )
   {
      my $code = ($field->{CODE} eq "#") ? "e" : $field->{CODE};
      $txt .= "_$code" if ($wantOptFields || $field->{DEFAULTVALUE} eq "");
   }
   $txt .= "(FILE *f";
   foreach $field ( getEffectiveFields($class) )
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

sub getEffectiveFields ()
{
   my $class = shift;
   my @fields = ();

   outer: while (true)
   {
      splice(@fields, 0, 0, @{ $class->{FIELDS} });
      if ($class->{SUPER} eq "EventLogTokenBasedEntry")
      {
         last outer;
      }
      else
      {
         inner: foreach $superClass (@classes)
         {
            if ($superClass->{NAME} eq $class->{SUPER})
            {
               $class = $superClass;
               last inner;
            }
         }
      }
   }
   @fields;
}

sub getEffectiveHasOpt ()
{
   my $class = shift;
   my $hasOpt = 0;

   outer: while (true)
   {
      $hasOpt = 1 if ($class->{HASOPT});
      if ($class->{SUPER} eq "EventLogTokenBasedEntry")
      {
         last outer;
      }
      else
      {
         inner: foreach $superClass (@classes)
         {
            if ($superClass->{NAME} eq $class->{SUPER})
            {
               $class = $superClass;
               last inner;
            }
         }
      }
   }
   $hasOpt;
}
