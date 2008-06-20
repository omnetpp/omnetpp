#
# Extract chart properties from java files
# and generates a docbook document.
#
# Call with the file names of the java files.
#
sub wordsplit
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
  @props = ($content =~ /properties\.Property\((.*?)\)\s*\n.*?get(.*?)\(\)/gcs);
  for ($i=0; $i < @props; $i += 2) {
    ($cat)  = ($props[$i] =~ /category="(.*?)"/s);
    ($name) = ($props[$i] =~ /displayName="(.*?)"/s);
    ($desc) = ($props[$i] =~ /description="([^"]*?)"/s);
    $name = wordsplit $props[$i+1] unless ($name);

    my $found = 0;
    foreach ( @cats ) {
      if( /$cat/ ) { $found = 1; last }
    }    
    push @cats, $cat unless $found;
    push @{ $names{$cat} }, $name;
    push @{ $descs{$cat} }, $desc;
  }

  # emit docbook
  print "  <section id=\"$file\">\n";
  foreach $cat (@cats) {
    print "    <variablelist>\n";
    print "      <title>$cat</title>\n";
    foreach $i ( 0 .. $#{ $names{$cat} } ) {
      print "      <varlistentry><term>$names{$cat}[$i]</term>\n";
      print "        <listitem>\n";
      print "          <para>$descs{$cat}[$i]</para>\n";
      print "        </listitem>\n";
      print "      </varlistentry>\n";
    }
    print "    </variablelist>\n";
  }
  print "  </section>\n";
}

# emit docbook
print "<?xml version=\"1.0\"?>\n";
print "<!DOCTYPE chapter PUBLIC \"-//OASIS//DTD DocBook XML V4.5//EN\"\n";
print "    \"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd\">\n";
print "<section>\n";
foreach $file (@ARGV) {
  processFile $file;
}
print "</section>\n";



