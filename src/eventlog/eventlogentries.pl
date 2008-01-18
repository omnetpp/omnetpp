#
# Generates eventlogentry classes from a textual description.
#
# Author: Levente Meszaros, 2006
#

# FIXME parsing: choose different strategy, current code cannot detect
#    unrecognized field names and missing mandatory fields in the trace file!
# FIXME ctors should use default value from the eventlogentries.txt file; also parse() and print() methods

open(FILE, "eventlogentries.txt");


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
   elsif ($_ =~ /^([\w]+) +([\w]+) *(\/\/ *([\w ]+))?$/)
   {
      $classCode = $1;
      $className = $2;
      $classComment = $4;
      $classHasOptField = 0;
      print "$classCode $className\n";
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
      print "{\n";
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+)( +([^ ]+))? *(\/\/ *([\w-+ ]+))?$/)
   {
      $fieldCode = $1;
      $fieldType = $2;
      $fieldName = $3;

      if ($fieldType eq "string")
      {
         $fieldPrintfType = "%s";
         $fieldDefault = "NULL";
      }
      elsif ($fieldType eq "long")
      {
         $fieldPrintfType = "%ld";
         $fieldDefault = "-1";
      }
      elsif ($fieldType eq "int")
      {
         $fieldPrintfType = "%d";
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
      print " $fieldCode $fieldType $fieldName $fieldDefault\n";
   }
   elsif ($_ =~ /^ *} *$/)
   {
      $class = {
         CODE => $classCode,
         NAME => $className,
         HASOPT => $classHasOptField,
         FIELDS => [ @fields ],
         COMMENT => $classComment,
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

#ifndef __EVENTLOGENTRIES_H_
#define __EVENTLOGENTRIES_H_

#include \"eventlogdefs.h\"
#include \"eventlogentry.h\"

NAMESPACE_BEGIN

class Event;

";

$index = 1;

foreach $class (@classes)
{
   print ENTRIES_H_FILE "
class EVENTLOG_API $class->{NAME} : public EventLogTokenBasedEntry
{
   public:
      $class->{NAME}();
      $class->{NAME}(Event *event);

   public:";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_H_FILE "
      $field->{CTYPE} $field->{NAME};";
   }
   print ENTRIES_H_FILE "

   public:
      virtual void parse(char **tokens, int numTokens);
      virtual void print(FILE *file);
      virtual int getClassIndex() { return $index; }
      virtual const char *getDefaultAttribute() const { return \"$class->{CODE}\"; }
      virtual const std::vector<const char *> getAttributeNames() const;
      virtual const char *getAttribute(const char *name) const; // BEWARE: Returns pointer into a static buffer which gets overwritten with each call!
      virtual const char *getClassName() { return \"$class->{NAME}\"; }
};
";

   $index++;
}

print ENTRIES_H_FILE "
NAMESPACE_END

#endif
";

close(ENTRIES_H_FILE);



#
# Write eventlogentries.cc file
#

open(ENTRIES_CC_FILE, ">eventlogentries.cc");

print ENTRIES_CC_FILE "\
//=========================================================================
//  EVENTLOGENTRIES.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#include \"event.h\"
#include \"eventlogentries.h\"
#include \"stringutil.h\"

USING_NAMESPACE
";

foreach $class (@classes)
{
   $className = $class->{NAME};

   # constructors
   print ENTRIES_CC_FILE "$className\::$className()\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    this->event = NULL;\n";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{DEFAULTVALUE};\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   print ENTRIES_CC_FILE "$className\::$className(Event *event)\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    this->event = event;\n";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    $field->{NAME} = $field->{DEFAULTVALUE};\n";
   }
   print ENTRIES_CC_FILE "}\n\n";

   # parse
   print ENTRIES_CC_FILE "void $className\::parse(char **tokens, int numTokens)\n";
   print ENTRIES_CC_FILE "{\n";
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "int")
      {
        $parserFunction = "getIntToken";
      }
      elsif ($field->{TYPE} eq "long")
      {
        $parserFunction = "getLongToken";
      }
      elsif ($field->{TYPE} eq "string")
      {
        $parserFunction = "getStringToken";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
        $parserFunction = "getSimtimeToken";
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
   if ($class->{NAME} eq "EventEntry")
   {
      print ENTRIES_CC_FILE "    fprintf(fout, \"\\n\");\n";
   }
   print ENTRIES_CC_FILE "    fprintf(fout, \"$class->{CODE}\");\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "string")
      {
         print ENTRIES_CC_FILE "    if ($field->{NAME})\n";
         print ENTRIES_CC_FILE "        fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", QUOTE($field->{NAME}));\n";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{DEFAULTVALUE})\n";
         print ENTRIES_CC_FILE "        fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{NAME}.str(buffer));\n";
      }
      else
      {
         print ENTRIES_CC_FILE "    if ($field->{NAME} != $field->{DEFAULTVALUE})\n";
         print ENTRIES_CC_FILE "        fprintf(fout, \" $field->{CODE} $field->{PRINTFTYPE}\", $field->{NAME});\n";
      }
   }

   print ENTRIES_CC_FILE "    fprintf(fout, \"\\n\");\n";
   print ENTRIES_CC_FILE "    fflush(fout);\n";
   print ENTRIES_CC_FILE "}\n\n";


   # getAttributeNames
   print ENTRIES_CC_FILE "const std::vector<const char *> $className\::getAttributeNames() const\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    std::vector<const char *> names;\n";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    names.push_back(\"$field->{CODE}\");\n";
   }
   print ENTRIES_CC_FILE "    return names;\n";
   print ENTRIES_CC_FILE "}\n\n";

   # getAttribute
   print ENTRIES_CC_FILE "const char *$className\::getAttribute(const char *name) const\n";
   print ENTRIES_CC_FILE "{\n";
   print ENTRIES_CC_FILE "    if (false);\n";

   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "    else if (!strcmp(name, \"$field->{CODE}\"))\n";
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
         print ENTRIES_CC_FILE "        sprintf(buffer, \"%ld\", (long)$field->{NAME});\n";
         print ENTRIES_CC_FILE "        return buffer;\n";
         print ENTRIES_CC_FILE "    }\n";
      }
   }

   print ENTRIES_CC_FILE "    else\n";
   print ENTRIES_CC_FILE "        return NULL;\n";
   print ENTRIES_CC_FILE "}\n\n";
}

close(ENTRIES_CC_FILE);



#
# Write eventlogentryfactory cc file
#

open(FACTORY_CC_FILE, ">eventlogentryfactory.cc");

print FACTORY_CC_FILE "\
//=========================================================================
//  EVENTLOGENTRYFACTORY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  This is a generated file -- do not modify.
//
//=========================================================================

#include \"event.h\"
#include \"eventlogentryfactory.h\"

USING_NAMESPACE

EventLogTokenBasedEntry *EventLogEntryFactory::parseEntry(Event *event, char **tokens, int numTokens)
{
    if (numTokens < 1)
        return NULL;

    char *code = tokens[0];
    EventLogTokenBasedEntry *entry;

    if (false)
        ;
";

foreach $class (@classes)
{
   #print FACTORY_CC_FILE "    else if (!strcmp(code, \"$class->{CODE}\"))\n";
   print FACTORY_CC_FILE "    else if (";
   $i=0;
   foreach $c (split(//, $class->{CODE})) {
       print FACTORY_CC_FILE "code\[$i\]=='$c' && ";
       $i++;
   }
   print FACTORY_CC_FILE "code[$i]==0)  // $class->{CODE}\n";

   print FACTORY_CC_FILE "        entry = new $class->{NAME}(event);\n";
}
print FACTORY_CC_FILE "    else\n";
print FACTORY_CC_FILE "        return NULL;\n\n";
print FACTORY_CC_FILE "    entry->parse(tokens, numTokens);\n";
print FACTORY_CC_FILE "    return entry;\n";
print FACTORY_CC_FILE "}\n";

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
