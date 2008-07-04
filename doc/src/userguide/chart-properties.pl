#!/usr/bin/perl -- # -*- Perl -*-

#
# Extract chart properties from java files
# and generates a docbook document.
#
# Call with the directory of the java files.
#
sub decamelize
{
	my $s = shift;
	my @words = split( /[_\s]+|\b|(?<![A-Z])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])/, $s );
        join (' ', map(lcfirst, @words));
}

sub readFile
{
  my $path = shift;
  my $content;
  open FILE, "$path" || die "cannot open input file $path\n";
  read(FILE, $content, 100000);
  close FILE;
  $content;
}


sub processFile
{
  my $path = shift;
  my ($file) = ($path =~ m|.*/(.*)\.java|);
  my $content = readFile $path;
  my @cats, %names, %descs;

  # parse java file
  @cats = ();
  %names = {};
  %descs = {};
  @props = ($content =~ /Property\((.*?)\)\s*\n\s*?public\s*?(\w+)\s*?get(.*?)\(\)/gcs);
  for ($i=0; $i < @props; $i += 3) {
    ($cat)  = ($props[$i] =~ /category="(.*?)"/s);
    ($name) = ($props[$i] =~ /displayName="(.*?)"/s);
    ($desc) = ($props[$i] =~ /description="([^"]*?)"/s);
    $type = $props[$i+1];
    $name = decamelize $props[$i+2] unless ($name);

    next if ($type =~ /.*IPropertySource.*/);

    my $found = 0;
    foreach ( @cats ) {
      if( /$cat/ ) { $found = 1; last }
    }    
    push @cats, $cat unless $found;
    push @{ $names{$cat} }, $name;
    push @{ $descs{$cat} }, $desc;
  }

  # emit docbook
  print "  <tgroup id='$file' cols='2' rowsep='1'>\n";
  print "    <colspec colname='name' colwidth='1.5in'/>\n";
  print "    <colspec colname='description' colwidth='3*'/>\n";
  print "    <spanspec spanname='category' namest='name' nameend='description' align='left'/>\n";
  print "    <tbody>\n";

  foreach $cat (@cats) {
    print "    <row><entry spanname='category'>$cat</entry></row>\n";
    foreach $i ( 0 .. $#{ $names{$cat} } ) {
      print "      <row>\n";
      print "        <entry>$names{$cat}[$i]</entry>\n";
      print "        <entry>$descs{$cat}[$i]</entry>\n";
      print "      </row>\n";
    }
  }
  print "    </tbody>\n";
  print "  </tgroup>\n";
}

$javaFileDir = $ARGV[0];

# emit docbook
print "<?xml version=\"1.0\"?>\n";
print "<!DOCTYPE table PUBLIC \"-//OASIS//DTD DocBook XML V4.5//EN\"\n";
print "    \"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd\">\n";
print "<table>\n";
opendir (DIR, $javaFileDir) || die "can not open directory $javaFileDir\n";
for (readdir(DIR)) {
  processFile "$javaFileDir/$_" if (/.*Properties.java/ && -f "$javaFileDir/$_");
}
closedir DIR;
print "</table>\n";



