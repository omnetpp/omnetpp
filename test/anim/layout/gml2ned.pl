#graph [
#	node [
#		id 0
#		name "Sunnyvale"
# ...
#	edge [
#		simplex 1
#		source 7
#		target 8
#		bandwidth 10G
#		metric 260
#	]
# ...

$inFileName = shift;
$networkName = $inFileName;
$networkName =~ s/.gml//;
$outFileName = "$networkName.ned";

open(IN, "$inFileName");
open(OUT, ">$outFileName");

print OUT "//
// Converted from $inFileName by gml2ned.pl
//

module ${networkName}Node
{
    parameters:
        \@display(\"i=misc/node_s\");
    gates:
        input in[];
        output out[];
    connections allowunconnected:
}

network $networkName
{
    submodules:
";

$connections = 0;

while (<IN>)
{
   $content = $content . $_;

   if ($content =~ s/node \[.*?id ([0-9]+).*name "(.+?)"//s)
   {
      print OUT "        node$1: ${networkName}Node {
            parameters:
                \@display(\"t=$2\");
        };\n";
   }

   if ($content =~ s/edge \[.*?source ([0-9]+).*?target ([0-9]+)//s)
   {
      if ($connections == 0)
      {
        print OUT "    connections allowunconnected:\n";
        $connections = 1;
      }
      print OUT "        node$1.out++ --> node$2.in++;\n";
   }
}

print OUT "}
";

close(IN);
close(OUT);
