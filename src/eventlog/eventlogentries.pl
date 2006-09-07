open(FILE, "eventlogentries.txt");

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
         $fieldPrintfType = "%x";
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

open(ENTRIES_H_FILE, ">eventlogentries.h");

print ENTRIES_H_FILE "
#ifndef __EVENTLOGENTRIES_H_
#define __EVENTLOGENTRIES_H_

#include \"defs.h\"
#include \"eventlogentry.h\"
";

foreach $class (@classes)
{
   print ENTRIES_H_FILE "
class $class->{NAME} : public EventLogEntry
{
   public:
      $class->{NAME}();

   protected:";
   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_H_FILE "
      $field->{CTYPE} $field->{NAME};";
   }
   print ENTRIES_H_FILE "

   public:
      virtual void parse(const char **tokens, int numTokens);
      virtual void print(FILE *file);
};
";
}

print ENTRIES_H_FILE "
#endif
";

close(ENTRIES_H_FILE);

open(ENTRIES_CC_FILE, ">eventlogentries.cc");

print ENTRIES_CC_FILE "
#include \"eventlogentries.h\"
";

foreach $class (@classes)
{
   $className = $class->{NAME};

   # constructor
   print ENTRIES_CC_FILE "
$className\::$className()
{";
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
void $className\::parse(const char **tokens, int numTokens)
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
   print ENTRIES_CC_FILE "
   ::fprintf(fout, \"$class->{SIGN} \");";

   foreach $field (@{ $class->{FIELDS} })
   {
      print ENTRIES_CC_FILE "
   ::fprintf(fout, \"$field->{SIGN} $field->{PRINTFTYPE} \", $field->{NAME});
   ::fflush(fout);"; 
   }

   print ENTRIES_CC_FILE "
} 
";
}

close(ENTRIES_CC_FILE);
