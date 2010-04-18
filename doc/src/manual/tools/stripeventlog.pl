#
# Generates eventlog appendix for the manual, from src/eventlog/eventlogentries.txt
#

while (<>)
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
   elsif ($_ =~ /^([\w]+) +([\w]+) *(\/\/ *(.*))?$/)
   {
      $classCode = $1;
      $className = $2;
      $classComment = $4;
      $classHasOptField = 0;

      $classDesc = $className;
      $classDesc =~ s/Entry$//;

      print "\n";
      print "\\tbf{$classCode} \\textit{($classDesc)}: $classComment\n\n";
   }
   elsif ($_ =~ /^ *{ *$/)
   {
      # left brace
      print "\\begin{itemize}\n";
      $fieldCount = 0;
   }
   elsif ($_ =~ /^ +([\w#]+) +([\w]+) +([\w]+)( +([^ ]+))? *(\/\/ *(.*))?$/)
   {
      # field spec
      $fieldCode = $1;
      $fieldType = $2;
      $fieldName = $3;
      if ($5 ne "") {
         $classHasOptField = 1;
         $fieldDefault  = $5;
         $mandatory = 0;
      }
      else {
         $mandatory = 1;
      }

      $comment = $7;

      $fieldCodeQuoted = $fieldCode;
      $fieldCodeQuoted =~ s/#/\\#/g;
      $fieldTypeQuoted = $fieldType;
      $fieldTypeQuoted =~ s/_/\\_/g;

      print "  \\item \\tbf{$fieldCodeQuoted} (\\textit{$fieldName}, $fieldTypeQuoted): $comment\n";
      $fieldCount++;
   }
   elsif ($_ =~ /^ *} *$/)
   {
      # right brace
      if ($fieldCount++ == 0) {
          print "  \\item no parameters\n";
      }
      print "\\end{itemize}\n";
   }
   else
   {
       die "unrecognized line \"$_\"";
   }
}


