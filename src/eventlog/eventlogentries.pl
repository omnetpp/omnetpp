#
# Generates eventlogentry classes from a textual description.
#
# Author: Levente Meszaros, 2006
#

# FIXME parsing: choose different strategy,
# current code cannot detect unrecognized field names in the trace file!

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
         if ($classCode eq $class->{CODE})
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
         $fieldDefault = "nullptr";
      }
      elsif ($fieldType eq "bool")
      {
         $fieldPrintfType = "%d";
         $fieldDefault = "false";
      }
      elsif ($fieldType eq "int")
      {
         $fieldPrintfType = "%d";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "short")
      {
         $fieldPrintfType = "%d";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "long")
      {
         $fieldPrintfType = "%ld";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "int64_t")
      {
         $fieldPrintfType = "%\" PRId64 \"";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "eventnumber_t")
      {
         $fieldPrintfType = "%\" EVENTNUMBER_PRINTF_FORMAT \"d";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "simtime_t")
      {
         $fieldPrintfType = "%s";
         $fieldDefault = "simtime_nil";
      }

      if ($5 ne "") {
         $classHasOptField = 1;
         $fieldDefault  = $5;
         $mandatory = 0;
      }
      else {
         $mandatory = 1;
      }

      $comment = $7;

      $fieldCType = $fieldType;
      $fieldCType =~ s/string/const char */;
      $field = {
         CODE => $fieldCode,
         TYPE => $fieldType,
         CTYPE => $fieldCType,
         PRINTFTYPE => $fieldPrintfType,
         NAME => $fieldName,
         MANDATORY => $mandatory,
         DEFAULTVALUE => $fieldDefault,
         COMMENT => $comment,
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
      $class->{NAME}(Event *event, int entryIndex);

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
      print ENTRIES_CC_FILE "    this->event = nullptr;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{DEFAULTVALUE};\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   print ENTRIES_CC_FILE "$className\::$className(Event *event, int entryIndex)\n";
   if ($class->{SUPER} ne "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    : $class->{SUPER}(event, entryIndex)\n";
   }
   print ENTRIES_CC_FILE "{\n";
   if ($class->{SUPER} eq "EventLogTokenBasedEntry")
   {
      print ENTRIES_CC_FILE "    this->event = event;\n";
      print ENTRIES_CC_FILE "    this->entryIndex = entryIndex;\n";
   }
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{DEFAULTVALUE};\n";
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
            print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{DEFAULTVALUE})\n    ";
         }
         print ENTRIES_CC_FILE "    fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{NAME}.str(buffer));\n";
      }
      else
      {
         if ($field->{MANDATORY} eq 0)
         {
            print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{DEFAULTVALUE})\n    ";
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
         print ENTRIES_CC_FILE "        sprintf(buffer, \"$field->{PRINTFTYPE}\", $field->{NAME});\n";
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

EventLogTokenBasedEntry *EventLogEntryFactory::parseEntry(Event *event, int entryIndex, char **tokens, int numTokens)
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

      print FACTORY_CC_FILE "        entry = new $class->{NAME}(event, entryIndex);\n";
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
