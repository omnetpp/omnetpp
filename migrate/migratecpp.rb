#
# Migrate an OMNeT++ 3.x model into 4.0. Written for the INET Fw, but may work
# for other models too, after some tweaking.
#
# Andras
#

require "FileUtils"

File.open("cppfiles.lst").each_line { |filename|

    filename = filename.chomp
    next if filename =~ /quaggasrc/

    contents = File.open(filename).read
    origcontents = contents.dup

    contents.gsub!( /\bcException\b/,    'cRuntimeError')
    contents.gsub!( /\bgetTail\b/,       'pop')
    contents.gsub!( /\bgetString\b/,     'toString')
    contents.gsub!( /\brunningModule\b/, 'activityModule')
    contents.gsub!( /\btail\b/,          'front')
    contents.gsub!( /\bsetFromText\b/,   'parse')
    contents.gsub!( /\bgetAsText\b/,     'toString')
    contents.gsub!( /\bcSimpleChannel\b/,'cBasicChannel')

    contents.gsub!( /\bcPar( +[a-z])/,   'cMsgPar\1')
    contents.gsub!( /\bnew +cPar\b/,     'new cMsgPar')

    contents.gsub!( /g->datarate\(\)->doubleValue\(\)/,  'g->channel()->par("datarate")')
    contents.gsub!( /\? par\("queueLength"\) : 0;/,      '? (int)par("queueLength") : 0;')
    contents.gsub!( /\bparse\(([a-zA-Z0-9_]+), *'\?'\)/, 'parse(\1)')
    contents.gsub!( /\bchan->addPar\(attrAttr\);/,       '; //FIXME remove this "if"')
    contents.gsub!( /\bchan->datarate\(\)/,              'chan->par("datarate").doubleValue()')

    contents.gsub!( /\bgate\(([^,)]+)\)->size\(\)/,      'gateSize(\1)')

    contents.gsub!( /cPolymorphic *\*dup\(\) *const *\{ *return +new +([_A-Za-z0-9]+)/, '\1 *dup() const {return new \1')
    contents.gsub!( /cObject *\*dup\(\) *const *\{ *return +new +([_A-Za-z0-9]+)/, '\1 *dup() const {return new \1')

    if filename =~ /ChannelControl\.cc/
        contents.gsub!( /\bbackgroundDisplayString\b/,  'displayString')
        contents.gsub!( /\bsetTagArg\("p"/,             'setTagArg("bgp"')
        contents.gsub!( /\bsetTagArg\("b"/,             'setTagArg("bgb"')
    end

    FileUtils.rm_f(filename+".new")
    if origcontents!=contents
        puts "modified: #{filename}"
        #File.open(filename+".new", "w").write(contents)
        File.open(filename, "w").write(contents)
    end

}

puts "TODO check all uses of cSimpleChannel!!!! refs to it should not be hardcoded!!!"

