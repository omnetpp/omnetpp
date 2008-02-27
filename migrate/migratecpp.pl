$listfname = $ARGV[0];
open(LISTFILE, $listfname) || die "cannot open $listfname";
while (<LISTFILE>)
{
    chomp;
    $fname = $_;
    print "processing $fname...\n";

    open(INFILE, $fname) || die "cannot open $fname";
    read(INFILE, $txt, 1000000) || die "cannot read $fname";
    close INFILE;

    # INSERT CODE TO PROCESS $txt HERE
#    contents.gsub!( /\bcException\b/,    'cRuntimeError')
#    contents.gsub!( /\bgetTail\b/,       'pop')
#    contents.gsub!( /\bgetString\b/,     'toString')
#    contents.gsub!( /\brunningModule\b/, 'activityModule')
#    contents.gsub!( /\btail\b/,          'front')
#    contents.gsub!( /\bsetFromText\b/,   'parse')
#    contents.gsub!( /\bgetAsText\b/,     'toString')
#    contents.gsub!( /\bcSimpleChannel\b/,'cBasicChannel')

#    contents.gsub!( /\bcPar( +[a-z])/,   'cMsgPar\1')
#    contents.gsub!( /\bnew +cPar\b/,     'new cMsgPar')

#    contents.gsub!( /g->datarate\(\)->doubleValue\(\)/,  'g->channel()->par("datarate")')
#    contents.gsub!( /\? par\("queueLength"\) : 0;/,      '? (int)par("queueLength") : 0;')
#    contents.gsub!( /\bparse\(([a-zA-Z0-9_]+), *'\?'\)/, 'parse(\1)')
#    contents.gsub!( /\bchan->addPar\(attrAttr\);/,       '; //FIXME remove this "if"')
#    contents.gsub!( /\bchan->datarate\(\)/,              'chan->par("datarate").doubleValue()')

#    contents.gsub!( /\bgate\(([^,)]+)\)->size\(\)/,      'gateSize(\1)')

#    contents.gsub!( /cPolymorphic *\*dup\(\) *const *\{ *return +new +([_A-Za-z0-9]+)/, '\1 *dup() const {return new \1')
#    contents.gsub!( /cObject *\*dup\(\) *const *\{ *return +new +([_A-Za-z0-9]+)/, '\1 *dup() const {return new \1')

#    if filename =~ /ChannelControl\.cc/
#        contents.gsub!( /\bbackgroundDisplayString\b/,  'displayString')
#        contents.gsub!( /\bsetTagArg\("p"/,             'setTagArg("bgp"')
#        contents.gsub!( /\bsetTagArg\("b"/,             'setTagArg("bgb"')
#    end

    open(OUTFILE, ">$fname") || die "cannot open $fname for write";
    print OUTFILE $txt || die "cannot write $fname";
    close OUTFILE;
}


























































