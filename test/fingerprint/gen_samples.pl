#!/usr/bin/env perl


@the_skiplist = <<END_OF_SKIPLIST =~ m/(\S.*\S)/g;
/examples/emulation/extclient/omnetpp.ini  General                       # ext interface tests are not supported as they require pcap drivers and external events
/examples/emulation/extserver/omnetpp.ini  Uplink_Traffic                # ext interface tests are not supported as they require pcap drivers and external events
/examples/emulation/extserver/omnetpp.ini  Downlink_Traffic              # ext interface tests are not supported as they require pcap drivers and external events
/examples/emulation/extserver/omnetpp.ini  Uplink_and_Downlink_Traffic   # ext interface tests are not supported as they require pcap drivers and external events
/examples/emulation/traceroute/omnetpp.ini  General                      # ext interface tests are not supported as they require pcap drivers and external events
/examples/ethernet/lans/defaults.ini  General                            # <!> Error: Network `' or `inet.examples.ethernet.lans.' not found, check .ini and .ned files. # The defaults.ini file included from other ini files
END_OF_SKIPLIST


# next line DISABLE previous skiplist:
#@the_skiplist = ();


foreach $i ( @the_skiplist )
{
    ($x) = $i =~ m/\#\s*(.*)$/;
    $i =~ s/\s*\#.*//;
    $skiplist{$i} = '# '.$x;
}
#print "-",join("-\n-",keys(%skiplist)),"-\n";
#die();

$DN = `dirname $0`;
chomp $DN;
#print "DN='$DN'\n";

chdir "$DN/../..";

$OMNETROOT=`pwd`;
chomp $OMNETROOT;
#print "OMNETROOT='$OMNETROOT'\n";


@inifiles = sort `find samples -name '*.ini'`;

die("Not found ini files\n") if ($#inifiles lt 0);

#print "=====================================\n",@inifiles,"=========================================\n";

@runs = ();

print '# workingdir,                        args,                                          simtimelimit,    fingerprint'."\n";

foreach $fname (@inifiles)
{
    chomp $fname;

    open(INFILE, $fname) || die "cannot open $fname";
    read(INFILE, $txt, 1000000) || die "cannot read $fname";
    close INFILE;

###    next if ($txt =~ /\bfingerprint\s*=/); #skip fingerprint tests

    @configs = ($txt =~ /^\s*(\[Config [a-zA-Z_0-9-]+\].*)$/mg);

#    print "-1- ",$#configs," ---------------->file=$fname, configs={",join(',',@configs),"}\n";

    @configs = ( "[Config General]" ) if ($#configs lt 0);

#    print "-2- ",$#configs," --------------->file=$fname, configs={",join(',',@configs),"}\n";

#    extends = XXX, YYY, ....
    @extends = ();
    @extendslines = ($txt =~ /^\s*extends\s*=\s*([^\#\n]+)\s*(?:\#.*)?$/mg);
    foreach $line (@extendslines)
    {
        $line =~ s/^\s+//;
        $line =~ s/\s+$//;
        #print "-e1- >", $line, "<\n";
        @items = split(/\s*,\s*/, $line);
        foreach $item (@items)
        {
            #print "-e2- >", $item, "<\n";
            $extends{$item} = "# $item extended";
        }
    }
    #print "-e3- >", join(', ',keys(%extends)), "<\n";

    foreach $conf (@configs)
    {
        ($cfg,$comm) = ($conf =~ /^\[Config ([a-zA-Z_0-9-]+)\]\s*(\#.*)?$/g);
        ($dir,$fnameonly) = ($fname =~ /(.*)[\/\\](.*)/);

        $run = "/".$dir.'/'.",";
        $run .= (' 'x(36-length $run)).' ';
        $run .= "-f $fnameonly -c $cfg -r 0".",";
        $xrun = "/$dir/$fnameonly  $cfg";
        $run .= (' 'x(83-length $run)).' ';
        $run .= '---100s'.",";
        $run .= (' 'x(100-length $run)).' ';
        $run .= '0000-0000';

        $x = "$run";

        if ($comm =~ /\b__interactive__\b/i)
        {
            $x = "# $run   # $conf";
        }
        elsif (length($extends{$cfg}))
        {
            $x = "# $run   ".$extends{$cfg};
        }
        elsif (length($skiplist{$xrun}))
        {
            $x = "# $run   ".$skiplist{$xrun};
        }

        print "$x\n";
    }
}
