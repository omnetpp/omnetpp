#
# Generates eventlogentry classes from a textual description.
#
# Author: Levente Meszaros, 2006
#

# FIXME parsing: choose different strategy,
# current code cannot detect unrecognized field names in the trace file!

$target = @ARGV[0];
$verbose = 0;

open(FILE, "eventlogentries.txt");


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
      foreach $class (@classes)
      {
         if ($classCode ne "abstract" && $classCode eq $class->{CODE})
         {
            die "Class code $classCode used twice";
         }
      }
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
      print "$fieldCode $fieldType $fieldName" if ($verbose);

      if ($fieldType eq "string")
      {
         $fieldPrintfType = "%s";
         $fieldCDefault = "nullptr";
         $fieldJavaDefault = "null";
      }
      elsif ($fieldType eq "bool")
      {
         $fieldPrintfType = "%d";
         $fieldCDefault = "false";
         $fieldJavaDefault = "false";
      }
      elsif ($fieldType eq "int")
      {
         $fieldPrintfType = "%d";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "short")
      {
         $fieldPrintfType = "%d";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "long")
      {
         $fieldPrintfType = "%ld";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "int64_t")
      {
         $fieldPrintfType = "%\" PRId64 \"";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "eventnumber_t")
      {
         $fieldPrintfType = "%\" EVENTNUMBER_PRINTF_FORMAT \"";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "msgid_t")
      {
         $fieldPrintfType = "%\" MSGID_PRINTF_FORMAT \"";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "txid_t")
      {
         $fieldPrintfType = "%\" TXID_PRINTF_FORMAT \"";
         $fieldCDefault = "-1";
         $fieldJavaDefault = "-1";
      }
      elsif ($fieldType eq "simtime_t")
      {
         $fieldPrintfType = "%s";
         $fieldCDefault = "simtime_nil";
         $fieldJavaDefault = "null";
      }

      if ($5 ne "") {
         $classHasOptField = 1;
         $fieldCDefault = $5;
         $fieldJavaDefault = $5;
         $fieldJavaDefault =~ s/NULL/null/;
         if ($fieldType eq "simtime_t") {
             $fieldJavaDefault = "new BigDecimal($fieldJavaDefault)"
         }
         $mandatory = 0;
      }
      else {
         $mandatory = 1;
      }

      $comment = $7;

      $fieldCType = $fieldType;
      $fieldCType =~ s/string/const char */;

      $fieldJavaType = $fieldType;
      $fieldJavaType =~ s/string/String/;
      $fieldJavaType =~ s/bool/boolean/;
      $fieldJavaType =~ s/int64_t/long/;
      $fieldJavaType =~ s/eventnumber_t/long/;
      $fieldJavaType =~ s/msgid_t/long/;
      $fieldJavaType =~ s/txid_t/long/;
      $fieldJavaType =~ s/simtime_t/BigDecimal/;

      $field = {
         CODE => $fieldCode,
         TYPE => $fieldType,
         CTYPE => $fieldCType,
         JAVATYPE => $fieldJavaType,
         PRINTFTYPE => $fieldPrintfType,
         NAME => $fieldName,
         MANDATORY => $mandatory,
         CDEFAULTVALUE => $fieldCDefault,
         JAVADEFAULTVALUE => $fieldJavaDefault,
         COMMENT => $comment,
      };

      push(@fields, $field);
      print " $fieldCode $fieldType $fieldName $fieldCDefault\n" if ($verbose);
   }
   elsif ($_ =~ /^ *} *$/)
   {
      $class = {
         CODE => $classCode,
         NAME => $className,
         SUPER => $classSuper,
         HASOPT => $classHasOptField,
         FIELDS => [ @fields ],
         COMMENT => $classComment,
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
# Write eventlogentries header file
#

open(ENTRIES_H_FILE, ">eventlogentries.h");

print ENTRIES_H_FILE
"//=========================================================================
//  EVENTLOGENTRIES.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#ifndef __OMNETPP_EVENTLOG_EVENTLOGENTRIES_H
#define __OMNETPP_EVENTLOG_EVENTLOGENTRIES_H

#include <cstdio>
#include \"eventlogdefs.h\"
#include \"eventlogentry.h\"

namespace omnetpp {
namespace eventlog {

class Event;

";

$index = 1;

foreach $class (@classes)
{
   print ENTRIES_H_FILE "
class EVENTLOG_API $class->{NAME} : public $class->{SUPER}
{
   public:
      $class->{NAME}();
      $class->{NAME}(IChunk *chunk, int entryIndex);

   public:";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_H_FILE "
      $field->{CTYPE} $field->{NAME};";
   }
   print ENTRIES_H_FILE "

   public:
      virtual void parse(char **tokens, int numTokens) override;
      virtual void print(FILE *file) override;
      virtual int getClassIndex() override { return $index; }
      virtual const char *getAsString() const override { return \"$class->{CODE}\"; }
      virtual const std::vector<const char *> getAttributeNames() const override;
      virtual const char *getAsString(const char *attribute) const override; // BEWARE: Returns pointer into a static buffer which gets overwritten with each call!
      virtual const char *getClassName() override { return \"$class->{NAME}\"; }
};
";

   $index++;
}

print ENTRIES_H_FILE "
} // namespace eventlog
} // namespace omnetpp

#endif
";

close(ENTRIES_H_FILE);



#
# Write eventlogentries.cc file
#

open(ENTRIES_CC_FILE, ">eventlogentries.cc");

print ENTRIES_CC_FILE
"//=========================================================================
//  EVENTLOGENTRIES.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#include <cstdio>
#include \"event.h\"
#include \"eventlogentries.h\"
#include \"common/stringutil.h\"

namespace omnetpp {
namespace eventlog {

using namespace omnetpp::common;

";

foreach $class (@classes)
{
   $className = $class->{NAME};

   # constructors
   print ENTRIES_CC_FILE "$className\::$className()\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    : $class->{SUPER}()\n";
   }
   print ENTRIES_CC_FILE "{\n";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    this->chunk = nullptr;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{CDEFAULTVALUE};\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   print ENTRIES_CC_FILE "$className\::$className(IChunk *chunk, int entryIndex)\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    : $class->{SUPER}(chunk, entryIndex)\n";
   }
   print ENTRIES_CC_FILE "{\n";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    this->chunk = chunk;\n";
      print ENTRIES_CC_FILE "    this->entryIndex = entryIndex;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{CDEFAULTVALUE};\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   # parse
   print ENTRIES_CC_FILE "void $className\::parse(char **tokens, int numTokens)\n";
   print ENTRIES_CC_FILE "{\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    $class->{SUPER}::parse(tokens, numTokens);\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "int")
      {
        $parserFunction = "getIntToken";
      }
      elsif ($field->{TYPE} eq "short")
      {
        $parserFunction = "getShortToken";
      }
      elsif ($field->{TYPE} eq "long")
      {
        $parserFunction = "getLongToken";
      }
      elsif ($field->{TYPE} eq "int64_t")
      {
        $parserFunction = "getInt64Token";
      }
      elsif ($field->{TYPE} eq "string")
      {
        $parserFunction = "getStringToken";
      }
      elsif ($field->{TYPE} eq "eventnumber_t")
      {
        $parserFunction = "getEventNumberToken";
      }
      elsif ($field->{TYPE} eq "msgid_t")
      {
        $parserFunction = "getInt64Token";  # khmm
      }
      elsif ($field->{TYPE} eq "txid_t")
      {
        $parserFunction = "getInt64Token";  # khmm
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
        $parserFunction = "getSimtimeToken";
      }
      elsif ($field->{TYPE} eq "bool")
      {
        $parserFunction = "getBoolToken";
      }
      else
      {
         die "Unknown type: $field->{TYPE}";
      }
      if ($field->{MANDATORY})
      {
         $mandatory = "true";
      }
      else
      {
         $mandatory = "false";
      }
      print ENTRIES_CC_FILE "    $field->{NAME} = $parserFunction(tokens, numTokens, \"$field->{CODE}\", $mandatory, $field->{NAME});\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   # print
   print ENTRIES_CC_FILE "void $className\::print(FILE *fout)\n";
   print ENTRIES_CC_FILE "{\n";
   if ($class->{CODE} ne "abstract")
   {
      print ENTRIES_CC_FILE "    fprintf(fout, \"$class->{CODE}\");\n";
   }
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    $class->{SUPER}::print(fout);\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "string")
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRIES_CC_FILE "    if ($field->{NAME})\n    ";
         }
         print ENTRIES_CC_FILE "    fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", QUOTE($field->{NAME}));\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{CDEFAULTVALUE})\n    ";
         }
         print ENTRIES_CC_FILE "    fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{NAME}.str(buffer));\n";
      }
      else
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{CDEFAULTVALUE})\n    ";
         }
         print ENTRIES_CC_FILE "    fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{NAME});\n";
      }
   }
   if ($class->{CODE} ne "abstract")
   {
      print ENTRIES_CC_FILE "    fprintf(fout, \"\\n\");\n";
   }
   print ENTRIES_CC_FILE "    fflush(fout);\n";
   print ENTRIES_CC_FILE "}\n\n";


   # getAttributeNames
   print ENTRIES_CC_FILE "const std::vector<const char *> $className\::getAttributeNames() const\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    std::vector<const char *> names;\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {

      print ENTRIES_CC_FILE "    std::vector<const char *> superNames = $class->{SUPER}::getAttributeNames();\n";
      print ENTRIES_CC_FILE "    names.insert(names.begin(), superNames.begin(), superNames.end());\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    names.push_back(\"$field->{CODE}\");\n";
   }
   print ENTRIES_CC_FILE "    return names;\n";
   print ENTRIES_CC_FILE "}\n\n";

   # getAsString()
   print ENTRIES_CC_FILE "const char *$className\::getAsString(const char *attribute) const\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    if (false);\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    else if (!strcmp(attribute, \"$field->{CODE}\"))\n";
      if ($field->{TYPE} eq "string")
      {
         print ENTRIES_CC_FILE "        return $field->{NAME};\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         print ENTRIES_CC_FILE "        return $field->{NAME}.str(buffer);\n";
      }
      else
      {
         print ENTRIES_CC_FILE "    {\n";
         print ENTRIES_CC_FILE "        snprintf(buffer, sizeof(buffer), \"$field->{PRINTFTYPE}\", $field->{NAME});\n";
         print ENTRIES_CC_FILE "        return buffer;\n";
         print ENTRIES_CC_FILE "    }\n";
      }
   }

   print ENTRIES_CC_FILE "    else\n";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "        return nullptr;\n";
   }
   else
   {
      print ENTRIES_CC_FILE "        return $class->{SUPER}::getAsString(attribute);\n";
   }
   print ENTRIES_CC_FILE "}\n\n";
}

print ENTRIES_CC_FILE "} // namespace eventlog\n} // namespace omnetpp\n";

close(ENTRIES_CC_FILE);



#
# Write eventlogentryfactory cc file
#

open(FACTORY_CC_FILE, ">eventlogentryfactory.cc");

print FACTORY_CC_FILE
"//=========================================================================
//  EVENTLOGENTRYFACTORY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#include <cstdio>
#include \"event.h\"
#include \"eventlogentryfactory.h\"

namespace omnetpp {
namespace eventlog {

EventLogTokenBasedEntry *EventLogEntryFactory::parseEntry(IChunk *chunk, int entryIndex, char **tokens, int numTokens)
{
    if (numTokens < 1)
        return nullptr;

    char *code = tokens[0];
    EventLogTokenBasedEntry *entry;

    if (false)
        ;
";

foreach $class (@classes)
{
   if ($class->{CODE} ne "abstract")
   {
      #print FACTORY_CC_FILE "    else if (!strcmp(code, \"$class->{CODE}\"))\n";
      print FACTORY_CC_FILE "    else if (";
      $i=0;
      foreach $c (split(//, $class->{CODE})) {
          print FACTORY_CC_FILE "code\[$i\]=='$c' && ";
          $i++;
      }
      print FACTORY_CC_FILE "code[$i]==0)  // $class->{CODE}\n";

      print FACTORY_CC_FILE "        entry = new $class->{NAME}(chunk, entryIndex);\n";
   }
}
print FACTORY_CC_FILE "    else\n";
print FACTORY_CC_FILE "        return nullptr;\n\n";
print FACTORY_CC_FILE "    entry->parse(tokens, numTokens);\n";
print FACTORY_CC_FILE "    return entry;\n";
print FACTORY_CC_FILE "}\n\n";
print FACTORY_CC_FILE "} // namespace eventlog\n} // namespace omnetpp\n";

close(FACTORY_CC_FILE);


#
# Write eventlogentries csv file
#

open(ENTRIES_CSV_FILE, ">eventlogentries.csv");

print ENTRIES_CSV_FILE "Name\tType\tDescription\n";

foreach $class (@classes)
{
   print ENTRIES_CSV_FILE "$class->{CODE}\t\t$class->{COMMENT}\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "bool")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\tboolean\t$field->{COMMENT}\n";
      }
      elsif (($field->{TYPE} eq "int") || $field->{TYPE} eq "long")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\tinteger\t$field->{COMMENT}\n";
      }
      elsif ($field->{TYPE} eq "eventnumber_t")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\tevent number\t$field->{COMMENT}\n";
      }
      elsif ($field->{TYPE} eq "msgid_t")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\tmsg id\t$field->{COMMENT}\n";
      }
      elsif ($field->{TYPE} eq "txid_t")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\ttx id\t$field->{COMMENT}\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\tsimulation time\t$field->{COMMENT}\n";
      }
      else
      {
         print ENTRIES_CSV_FILE "$field->{CODE}\t$field->{TYPE}\t$field->{COMMENT}\n";
      }
   }
}

close(ENTRIES_CSV_FILE);

if ($target eq "")
{
   exit(0);
}

#
# Write EventLogEntries java file
#

foreach $class (@classes)
{
   $className = $class->{NAME};

   open(ENTRY_JAVA_FILE, ">$class->{NAME}.java");

   print ENTRY_JAVA_FILE
"package org.omnetpp.eventlog.entry;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;

import org.omnetpp.eventlog.IChunk;
";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE
"import org.omnetpp.eventlog.$class->{SUPER};
"
   }

   $hasBigDecimal = "false";
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{JAVATYPE} eq "BigDecimal")
      {
         $hasBigDecimal = "true";
      }
   }
   if ($hasBigDecimal eq "true")
   {
      print ENTRY_JAVA_FILE
"import org.omnetpp.common.util.BigDecimal;

";
   }

   print ENTRY_JAVA_FILE "public class $class->{NAME} extends $class->{SUPER}
{";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRY_JAVA_FILE "
    public $field->{JAVATYPE} $field->{NAME};";
   }
   print ENTRY_JAVA_FILE "

    public $class->{NAME}() {
";

   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "        this.chunk = null;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRY_JAVA_FILE "        $field->{NAME} = $field->{JAVADEFAULTVALUE};\n";
   }

   print ENTRY_JAVA_FILE "    }

    public $class->{NAME}(IChunk chunk, int entryIndex) {
";

   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "        super(chunk, entryIndex);\n";
   }
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "        this.chunk = chunk;\n";
      print ENTRY_JAVA_FILE "        this.entryIndex = entryIndex;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRY_JAVA_FILE "        $field->{NAME} = $field->{JAVADEFAULTVALUE};\n";
   }

   print ENTRY_JAVA_FILE "    }\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      $fname = ucfirst($field->{NAME});
      print ENTRY_JAVA_FILE "
    public $field->{JAVATYPE} get$fname() { return $field->{NAME}; }";
   }

   print ENTRY_JAVA_FILE "

    public void parse(String[] tokens, int numTokens) {
";

   # parse
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "        super.parse(tokens, numTokens);\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "int")
      {
        $parserFunction = "getIntToken";
      }
      elsif ($field->{TYPE} eq "short")
      {
        $parserFunction = "getShortToken";
      }
      elsif ($field->{TYPE} eq "long")
      {
        $parserFunction = "getLongToken";
      }
      elsif ($field->{TYPE} eq "int64_t")
      {
        $parserFunction = "getInt64Token";
      }
      elsif ($field->{TYPE} eq "string")
      {
        $parserFunction = "getStringToken";
      }
      elsif ($field->{TYPE} eq "eventnumber_t")
      {
        $parserFunction = "getEventNumberToken";
      }
      elsif ($field->{TYPE} eq "msgid_t")
      {
        $parserFunction = "getInt64Token"; # khmm
      }
      elsif ($field->{TYPE} eq "txid_t")
      {
        $parserFunction = "getInt64Token"; # khmm
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
        $parserFunction = "getSimtimeToken";
      }
      elsif ($field->{TYPE} eq "bool")
      {
        $parserFunction = "getBoolToken";
      }
      else
      {
         die "Unknown type: $field->{TYPE}";
      }
      if ($field->{MANDATORY})
      {
         $mandatory = "true";
      }
      else
      {
         $mandatory = "false";
      }
      print ENTRY_JAVA_FILE "        $field->{NAME} = $parserFunction(tokens, numTokens, \"$field->{CODE}\", $mandatory, $field->{NAME});\n";
   }

   print ENTRY_JAVA_FILE "    }

    public void print(OutputStream stream) {
        try {
";

   # print
   if ($class->{CODE} ne "abstract")
   {
      print ENTRY_JAVA_FILE "            stream.write((\"$class->{CODE}\").getBytes());\n";
   }
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "            super.print(stream);\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "string")
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRY_JAVA_FILE "            if ($field->{NAME} != null)\n    ";
         }
         print ENTRY_JAVA_FILE "            stream.write((\" $field->{CODE} \" + qoute($field->{NAME})).getBytes());\n";
      }
      elsif ($field->{TYPE} eq "bool")
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRY_JAVA_FILE "            if ($field->{NAME} != $field->{JAVADEFAULTVALUE})\n    ";
         }
         print ENTRY_JAVA_FILE "            stream.write((\" $field->{CODE} \" + ($field->{NAME} ? \"1\" : \"0\")).getBytes());\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRY_JAVA_FILE "            if (!$field->{NAME}.equals($field->{JAVADEFAULTVALUE}))\n    ";
         }
         print ENTRY_JAVA_FILE "            stream.write((\" $field->{CODE} \" + $field->{NAME}.toString()).getBytes());\n";
      }
      else
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRY_JAVA_FILE "            if ($field->{NAME} != $field->{JAVADEFAULTVALUE})\n    ";
         }
         print ENTRY_JAVA_FILE "            stream.write((\" $field->{CODE} \" + String.valueOf($field->{NAME})).getBytes());\n";
      }
   }
   if ($class->{CODE} ne "abstract")
   {
      print ENTRY_JAVA_FILE "            stream.write((\"\\n\").getBytes());\n";
   }
   print ENTRY_JAVA_FILE "            stream.flush();";

    print ENTRY_JAVA_FILE "
        }
        catch (IOException e) {
            throw new RuntimeException (e);
        }
    }

    public int getClassIndex() { return $index; }

    public String getAsString() { return \"$class->{CODE}\"; }

    public ArrayList<String> getAttributeNames() {
";

   # getAttributeNames
   print ENTRY_JAVA_FILE "        ArrayList<String> names = new ArrayList<String>();\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "        names.addAll(super.getAttributeNames());\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRY_JAVA_FILE "        names.add(\"$field->{CODE}\");\n";
   }
   print ENTRY_JAVA_FILE "        return names;";

    print ENTRY_JAVA_FILE "
    }

    \@SuppressWarnings(\"unused\")
    public String getAsString(String attribute) {
";

   # getAsString()
   print ENTRY_JAVA_FILE "        if (false)\n";
   print ENTRY_JAVA_FILE "            return null;\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRY_JAVA_FILE "        else if (attribute.equals(\"$field->{CODE}\"))\n";
      if ($field->{TYPE} eq "string")
      {
         print ENTRY_JAVA_FILE "            return $field->{NAME};\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         print ENTRY_JAVA_FILE "            return $field->{NAME}.toString();\n";
      }
      else
      {
         print ENTRY_JAVA_FILE "            return String.valueOf($field->{NAME});\n";
      }
   }

   print ENTRY_JAVA_FILE "        else\n";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRY_JAVA_FILE "            return null;\n";
   }
   else
   {
      print ENTRY_JAVA_FILE "            return super.getAsString(attribute);\n";
   }

   print ENTRY_JAVA_FILE "
    }

    public String getClassName() { return \"$class->{NAME}\"; }
}

";

   close(ENTRY_JAVA_FILE);
}


#
# Write EventLogEntryFactory java file
#

open(FACTORY_JAVA_FILE, ">EventLogEntryFactory.java");

print FACTORY_JAVA_FILE
"package org.omnetpp.eventlog;

public class EventLogEntryFactory
{
    \@SuppressWarnings(\"unused\")
    public static EventLogTokenBasedEntry parseEntry(IChunk chunk, int entryIndex, String[] tokens, int numTokens) {
        if (numTokens < 1)
            return null;

        String code = tokens[0];
        EventLogTokenBasedEntry entry = null;

        if (false)
            ;
";

foreach $class (@classes)
{
   if ($class->{CODE} ne "abstract")
   {
      print FACTORY_JAVA_FILE "        else if (";
      $i=0;
      $l=length($class->{CODE});
      print FACTORY_JAVA_FILE "code.length\(\) == $l";
      foreach $c (split(//, $class->{CODE})) {
          print FACTORY_JAVA_FILE " && code.charAt\($i\) == '$c'";
          $i++;
      }
      print FACTORY_JAVA_FILE ") // $class->{CODE}\n";

      print FACTORY_JAVA_FILE "            entry = new $class->{NAME}(chunk, entryIndex);\n";
   }
}

print FACTORY_JAVA_FILE "        else\n";
print FACTORY_JAVA_FILE "            return null;\n\n";
print FACTORY_JAVA_FILE "        entry.parse(tokens, numTokens);\n";
print FACTORY_JAVA_FILE "        return entry;\n";
print FACTORY_JAVA_FILE "    }\n}\n\n";

close(FACTORY_JAVA_FILE);

