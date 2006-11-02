# FIXME Levy pls add comment at top of file: what is this perl script, what does it do?
# FIXME parsing: choose different strategy, current code cannot detect unrecognized field names in the trace file!
# FIXME parsing: does it support optional fields in the trace file?

open(FILE, "eventlogentries.txt");

##########################################
# Read input file

while (<FILE>)
{
   if ($_ =~ /^ *\/\//)
   {
      # comment
   }
   elsif ($_ =~ /^([\w]+) +([\w]+) *$/)
   {
      $classSign = $1;
      $className = $2;
      print "$classSign $className\n";
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      print "{\n";
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+) *$/)
   {
      $fieldSign = $1;
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
         $fieldPrintfType = "%.*g";
      }

      $fieldCType = $fieldType;
      $fieldCType =~ s/string/const char */;
      $field = {
         SIGN => $fieldSign,
         TYPE => $fieldType,
         CTYPE => $fieldCType,
         PRINTFTYPE => $fieldPrintfType,
         NAME => $fieldName,
      };
      push(@fields, $field);
      print " $fieldSign $fieldType $fieldName\n";
   }
   elsif ($_ =~ /^ *} *$/)
   {
      $class = {
         SIGN => $classSign,
         NAME => $className,
         FIELDS => [ @fields ],
      };
      push(@classes, $class);
      @fields = ();
      print "}\n";
   }
}

close(FILE);

##########################################
# Write eventlogentries header file

open(ENTRIES_H_FILE, ">eventlogentries.h");

print ENTRIES_H_FILE "
#ifndef __EVENTLOGENTRIES_H_
#define __EVENTLOGENTRIES_H_

#include \"defs.h\"
#include \"eventlogentry.h\"

class Event;
";

$index = 1;

foreach $class (@classes)
{
   print ENTRIES_H_FILE "
class $class->{NAME} : public EventLogTokenBasedEntry
{
   public:
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
      virtual const char *getClassName() { return \"$class->{NAME}\"; }
};
";

   $index++;
}

print ENTRIES_H_FILE "
#endif
";

close(ENTRIES_H_FILE);

##########################################
# Write eventlogentries cc file

open(ENTRIES_CC_FILE, ">eventlogentries.cc");

print ENTRIES_CC_FILE "
#include \"event.h\"
#include \"eventlogentries.h\"
";

foreach $class (@classes)
{
   $className = $class->{NAME};

   # constructor
   print ENTRIES_CC_FILE "
$className\::$className(Event *event)
{
   this->event = event;";
   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "string")
      {
      print ENTRIES_CC_FILE "
   $field->{NAME} = NULL;"; 
      }
      else
      {
      print ENTRIES_CC_FILE "
   $field->{NAME} = -1;"; 
      }
   }
   print ENTRIES_CC_FILE "
}
";

   # parse
   print ENTRIES_CC_FILE "
void $className\::parse(char **tokens, int numTokens)
{";
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
      print ENTRIES_CC_FILE "
   $field->{NAME} = $parserFunction(tokens, numTokens, \"$field->{SIGN}\");";
   }

   print ENTRIES_CC_FILE "
}
";
   # print
   print ENTRIES_CC_FILE "
void $className\::print(FILE *fout)
{";
   if ($class->{NAME} eq "EventEntry")
   {
      print ENTRIES_CC_FILE "
   ::fprintf(fout, \"\\n\");";
   }

   print ENTRIES_CC_FILE "
   ::fprintf(fout, \"$class->{SIGN}\");";

   foreach $field (@{ $class->{FIELDS} })
   {
      if ($field->{TYPE} eq "string")
      {
         print ENTRIES_CC_FILE "
   if ($field->{NAME} != NULL)
   {";
         print ENTRIES_CC_FILE "
      if (strchr($field->{NAME}, ' '))
         ::fprintf(fout, \" $field->{SIGN} \\\"$field->{PRINTFTYPE}\\\"\", $field->{NAME});
      else
         ::fprintf(fout, \" $field->{SIGN} $field->{PRINTFTYPE}\", $field->{NAME});
   }";
      }
      elsif ($field->{TYPE} eq "simtime_t")
      {
         print ENTRIES_CC_FILE "
   if ($field->{NAME} != -1)
      ::fprintf(fout, \" $field->{SIGN} $field->{PRINTFTYPE}\", 12, $field->{NAME});";
      }
      else
      {
         print ENTRIES_CC_FILE "
   if ($field->{NAME} != -1)
      ::fprintf(fout, \" $field->{SIGN} $field->{PRINTFTYPE}\", $field->{NAME});";
      }
   }

   print ENTRIES_CC_FILE "
   ::fprintf(fout, \"\\n\");
   ::fflush(fout);
} 
";
}

close(ENTRIES_CC_FILE);

##########################################
# Write eventlogentryfactory cc file

open(FACTORY_CC_FILE, ">eventlogentryfactory.cc");

print FACTORY_CC_FILE "
#include \"event.h\"
#include \"eventlogentryfactory.h\"

EventLogTokenBasedEntry * EventLogEntryFactory::parseEntry(Event *event, char **tokens, int numTokens)
{
   if (numTokens < 1)
      return NULL;

   char *sign = tokens[0];
   EventLogTokenBasedEntry *entry;

   if (false);
";

foreach $class (@classes)
{
   print FACTORY_CC_FILE "
   else if (!strcmp(sign, \"$class->{SIGN}\"))
      entry = new $class->{NAME}(event);
";
   }

print FACTORY_CC_FILE "
   else
      return NULL;

   entry->parse(tokens, numTokens);
   return entry;
}
";

close(FACTORY_CC_FILE);
