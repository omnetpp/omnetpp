use File::Find;

$userguideDir = "C:\\Workspace\\Repository\\trunk\\omnetpp\\doc\\src\\userguide";

sub icons {
   $name = $_;
   $file = $File::Find::name;
   $file =~ s|/|\\|g;

   if (($name =~ /\.png$/ || $name =~ /\.gif$/) && ($file =~ /icons/ || $file =~ /images/))
   {
      system("C:\\Progra~1\\ImageMagick-6.4.1-Q16\\convert.exe -frame 1 $userguideDir\\$file $userguideDir\\icons\\$name");
   }
}

find(\&icons, "..\\..\\..\\ui");
