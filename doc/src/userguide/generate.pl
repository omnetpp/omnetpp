#!/usr/bin/perl -- # -*- Perl -*-

# Generates docbook elements from the source code.

sub usage
{
  print STDERR "perl generate chart-properties <dir>\n";
  print STDERR "\tgenerates table of chart properties from java files in <dir>\n\n";
  print STDERR "perl generate processing-operations\n";
  print STDERR "\tgenerates table of processing operations\n";
}

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


sub processPropertyFile
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

sub generateChartProperties
{
  my $javaFileDir = shift;

  # emit docbook
  print "<?xml version=\"1.0\"?>\n";
  print "<!DOCTYPE table PUBLIC \"-//OASIS//DTD DocBook XML V4.5//EN\"\n";
  print "    \"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd\">\n";
  print "<table>\n";
  opendir (DIR, $javaFileDir) || die "can not open directory $javaFileDir\n";
  for (readdir(DIR)) {
    processPropertyFile "$javaFileDir/$_" if (/.*Properties.java/ && -f "$javaFileDir/$_");
  }
  closedir DIR;
  print "</table>\n";
}

sub generateProcessingOps
{
  my $info = `../../../bin/scavetool info`;
  my @ops = ($info =~ /([-\w]+)\(.*?\):\n(.*?\n\n)/gs);
  my @names;
  my @descs;
  my $desc;
  for ($i = 0; $i < @ops; $i += 2) {
    push @names, $ops[$i];
    $desc = $ops[$i+1];
    $desc =~ s/^\s+-.*$//gm; # remove lines starting with '-', they describe parameters
    $desc =~ s/\s+/ /gs;     # normalize whitespaces
    $desc =~ s/^\s+//gs;
    $desc =~ s/\s+$//gs;
    $desc =~ s|\[([ki].*?)\]|<subscript>\1</subscript>|g;
    $desc =~ s|([yt])out|\1<superscript>out</superscript>|g;
    $desc =~ s|([yt]<superscript>out.*)|<emphasis>\1</emphasis>|;
    push @descs, $desc;
  }

  
  print "<?xml version=\"1.0\"?>\n";
  print "<!DOCTYPE table PUBLIC \"-//OASIS//DTD DocBook XML V4.5//EN\"\n";
  print "    \"http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd\">\n";
  print "<table id='processing-operations'>\n";
  print "  <title>Processing operations</title>\n";
  print "  <tgroup cols='2'>\n";
  print "    <colspec colwidth='1.2in'/>\n";
  print "    <colspec colwidth='3*'/>\n";
  print "    <thead><row><entry>Name</entry><entry>Description</entry></row></thead>\n";
  print "    <tbody>\n";
  for ($i = 0; $i < @names; $i++) {
    print "      <row><entry>$names[$i]</entry><entry>$descs[$i]</entry></row>\n";
  }
  print "    </tbody>\n";
  print "  </tgroup>\n";
  print "</table>\n";
}

$cmd = $ARGV[0];
if ($cmd eq "chart-properties") {
  generateChartProperties $ARGV[1];
}
elsif ($cmd eq "processing-operations") {
  generateProcessingOps();
}
else {
  usage();
  exit 1;
}
