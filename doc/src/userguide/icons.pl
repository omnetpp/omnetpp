use File::Find;

sub icons {
   if (/\.png$/ || /\.gif$/) {
      $name = $_;
      $file = $File::Find::name;
      $file =~ s|.*?\\ui/||;
      print "   <icon name=\"$name\" file=\"$file\"/>\n";
   }
}

print "<icons>\n";

find(\&icons, "..\\..\\..\\ui");

print "<\icons>\n";
