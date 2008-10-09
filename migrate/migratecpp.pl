my $arglessGetters = "accuracyDetectionObject activityModule
    argCount argTypes argVector arrivalGate arrivalGateId arrivalModule
    arrivalModuleId arrivalTime baseName bitLength byteLength category
    cellMax cellMin cellNumber cellSize cellValue channel channelType
    className componentType config context contextClassName
    contextFullPath contextModule contextSimpleModule contextPointer
    contextType controlInfo count creationTime datarate defaultOwner
    defaultValue delay description destinationGate displayString
    distanceToTarget elemTypeName encapsulatedMsg encapsulationId
    encapsulationTreeId error errorCode eventNumber expression
    extraStackForEnvir fileName fromGate fromstat fullName fullPath
    functionPointer gateNames hasher hostObject id index
    insertOrder instance keys kind lastModuleId liveMessageCount
    liveObjectCount liveParImplObjectCount localGate localGateId mathFunc
    mathFunc1Arg mathFunc2Args mathFunc3Args mathFunc4Args mathFuncNoArg
    max maxArgs maxTime mean messageQueue min minArgs module moduleID
    moduleId moduleType name nedTypeName networkType numArgs
    numProperties numRNGs numbersDrawn objectValue overflowCell owner
    ownerModule parList parentModule previousEventNumber priority
    properties remoteGate remoteGateId remoteNode returnType rootGrid
    scale scaleExp scheduler senderGate senderGateId senderModule
    senderModuleId sendingTime shareCount signature sourceGate sqrSum
    sqrSumWeights srcProcId stackSize stackUsage state stateName stddev
    sum systemModule targetNode timestamp toGate totalMessageCount
    totalObjectCount totalParImplObjectCount transientDetectionObject
    treeDepth treeId type underflowCell unit valuesReceived valuesStored
    variance weight weightedSqrSum weightedSum weights";

my $gettersWithArg = "ancestorPar argType basepoint cellPDF
    connectionProperties gateProperties grid
    module moduleByPath moduleByRelativePath numValues paramProperties
    realCellValue sizeofGate sizeofIdent sizeofIndexedSiblingModuleGate
    sizeofParentModuleGate sizeofSiblingModuleGate stringFor submodule
    submoduleProperties treeDepth typeName typeOf valuesVector node
    nodeFor";

$arglessGetters =~ s/\s+/|/g;
$gettersWithArg =~ s/\s+/|/g;

# key: the warning message; value: where (several file+lineno+linecontent lines contatenated)
%warnings = ();

$listfname = $ARGV[0];
open(LISTFILE, $listfname) || die "cannot open $listfname";
while (<LISTFILE>)
{
    chomp;
    s/\r$//; # cygwin/mingw perl does not do CR/LF translation

    $fname = $_;

    if ($fname =~ /_m\./) {
        print "skipping $fname...\n";
        next;
    }

    print "processing $fname...\n";

    open(INFILE, $fname) || die "cannot open $fname";
    read(INFILE, $txt, 1000000) || die "cannot read $fname";
    close INFILE;

    # process $txt:

    # make dup() use covariant return types
    $txt =~ s/\b(cPolymorphic|cObject|cOwnedOject)\s*\*\s*dup\s*\(\s*\)\s*const\s*\{\s*return\s+new\s+([_A-Za-z0-9]+)/\2 *dup() const {return new \2/sg;

    # message params are no longer cPar but cMsgPar
    $txt =~ s/\bcPar( +[A-Za-z])/cMsgPar\1/mg;
    $txt =~ s/\bnew +cPar\b/new cMsgPar/mg;

    $txt =~ s!^(# *define +EV +.*)$!//$1 ==> EV is now part of <omnetpp.h>!mg;

    # rename cPar methods
    $txt =~ s/\bsetFromText\b/parse/mg;
    $txt =~ s/\bgetAsText\b/str/mg;

    $txt =~ s/\bcException\b/cRuntimeError/mg;
    $txt =~ s/\bcSimpleChannel\b/cDatarateChannel/mg;
    $txt =~ s/\bcBasicChannel\b/cDatarateChannel/mg;

    # message length (careful with length(), as it may easily be string.length(), queue.length() etc)
    $txt =~ s/([a-zA-Z0-9]*([Mm]sg|[Mm]essage|[Pp]k|[Pp]acket|[Ff]rame)[a-zA-Z0-9]*)->length\(\)/$1->getBitLength()/mg;
    $txt =~ s/\bsetLength\(/setBitLength(/mg;
    $txt =~ s/\baddLength\(/addBitLength(/mg;
    #FIXME TODO: if it sees: m->setLength(par("msgLength")) then it should insert a cast to long! otherwise "cannot convert cPar to int64"!

    # cModule
    $txt =~ s/\bgate\(([^,)]+)\)->size\(\)/gateSize(\1)/mg;

    # turn mod->simTime() and simulation.simTime() into just simTime()
    $txt =~ s/[a-zA-Z0-9]+(\.|->)simTime\(\)/simTime()/mg;

    # rename cQueue methods
    $txt =~ s/\bgetTail *\( *\)/pop()/mg;
    $txt =~ s/\btail *\( *\)/front()/mg;

    # cStatistic methods
    $txt =~ s/\bsamples *\( *\)/count()/mg;
    $txt =~ s/\btransformed *\( *\)/isTransformed()/mg;
    $txt =~ s/\brecordScalar *\( *\)/record()/mg;
    $txt =~ s/\. *recordScalar *\( *"/.recordAs("/mg;

    # cDisplayString
    $txt =~ s/\bexistsTag\(/containsTag(/mg;
    $txt =~ s/\bgetString *\( *\)/str()/mg;

    # cSimulation
    $txt =~ s/\brunningModule *\( *\)/activityModule()/mg;

    # cGate
    $txt =~ s/\brouteContains\b/pathContains/mg;
    $txt =~ s/\bisRouteOK *\( *\)/isPathOK()/mg;
    $txt =~ s/->datarate\(\)->doubleValue\(\)/->channel()->par("datarate").doubleValue()/mg;
    $txt =~ s/->delay\(\)->doubleValue\(\)/->channel()->par("delay").doubleValue()/mg;
    $txt =~ s/->error\(\)->doubleValue\(\)/->channel()->par("ber").doubleValue()/mg;

    # cDatarateChannel
    $txt =~ s/\berror\(\)/getBitErrorRate()/mg;
    $txt =~ s/\bsetError\(/setBitErrorRate(/mg;

    # cTopology
    $txt =~ s/\bunweightedSingleShortestPathsTo\(/calculateUnweightedSingleShortestPathsTo(/mg;

    # dynamic module creation
    $txt =~ s/\bfindModuleType\(/cModuleType::get(/mg;

    # add getters (automatic)
    $txt =~ s/\b($arglessGetters) ?\( *\)/"get".ucfirst($1)."()"/mge;
    $txt =~ s/\b($gettersWithArg) ?\(/"get".ucfirst($1)."("/mge;

    # add getters (manual)
    $txt =~ s/\bchanged\(\)/hasChanged()/mg;
    $txt =~ s/\bdisabled\(\)/isDisabled()/mg;
    $txt =~ s/\benabled\(\)/isEnabled()/mg;
    $txt =~ s/\bstackOverflow\(\)/hasStackOverflow()/mg;
    $txt =~ s/\btakeOwnership\(\)/getTakeOwnership()/mg;
    $txt =~ s/\bexiting\(\)/isExiting()/mg;

    $txt =~ s/\bparams\(\)/getNumParams()/mg;
    $txt =~ s/\btransmissionFinishes\(\)/getTransmissionFinishTime()/mg;
    $txt =~ s/\bcells\(\)/getNumCells()/mg;
    $txt =~ s/\bnodes\(\)/getNumNodes()/mg;
    $txt =~ s/\binLinks\(\)/getNumInLinks()/mg;
    $txt =~ s/\boutLinks\(\)/getNumOutLinks()/mg;
    $txt =~ s/\bpaths\(\)/getNumPaths()/mg;
    $txt =~ s/\bostream\(\)/getOStream()/mg;
    $txt =~ s/\binTransientState\(\)/isInTransientState()/mg;

    $txt =~ s/\btakeOwnership\(/setTakeOwnership(/mg;  # since getter is done already
    $txt =~ s/\bcellInfo\(/getCellInfo(/mg;
    $txt =~ s/\bcell\(/getCellValue(/mg;
    $txt =~ s/\brng\(/getRNG(/mg;
    $txt =~ s/\bpdf\(/getPDF(/mg;
    $txt =~ s/\bcdf\(/getCDF(/mg;

    $txt =~ s/(\.|->)in\(/$1getLinkIn(/mg;
    $txt =~ s/(\.|->)out\(/$1getLinkOut(/mg;
    $txt =~ s/(\.|->)path\(/$1getPath(/mg;

    # cGate, cChannel
    $txt =~ s/\bgetFromGate\(\)/getPreviousGate()/mg;
    $txt =~ s/\bgetToGate\(\)/getNextGate()/mg;
    $txt =~ s/\bgetSourceGate\(\)/getPathStartGate()/mg;
    $txt =~ s/\bgetDestinationGate\(\)/getPathEndGate()/mg;
    $txt =~ s/([cC]h[a-zA-Z0-9()]*)->getPreviousGate\(\)/$1->getSourceGate()/mg;  #channel

    # parsim
    $txt =~ s/\bnetPack *\(/parsimPack(/mg;
    $txt =~ s/\bnetUnpack *\(/parsimUnpack(/mg;

    # ancient stuff, from compat.h
    $txt =~ s/\bcKSplitIterator\b/cKSplit::Iterator/mg;
    $txt =~ s/\bsGrid\b/cKSplit::Grid/mg;
    $txt =~ s/\bcLinkedListIterator\b/cLinkedList::Iterator/mg;
    $txt =~ s/\bcMessageHeapIterator\b/cMessageHeap::Iterator/mg;
    $txt =~ s/\bsTopoLink\b/cTopology::Link/mg;
    $txt =~ s/\bsTopoLinkIn\b/cTopology::LinkIn/mg;
    $txt =~ s/\bsTopoLinkOut\b/cTopology::LinkOut/mg;
    $txt =~ s/\bsTopoNode\b/cTopology::Node/mg;
    $txt =~ s/\bcDisplayStringParser\b/cDisplayString/mg;

    # exceptions must be thrown by value
    $txt =~ s/\bthrow +new +cRuntimeError\b/throw cRuntimeError/mg;

    # remove Module_Class_Members(classname, baseclass, 0)
    $txt =~ s/^(\s*)Module_Class_Members\(\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*0\s*\);?//mg;

    # expand Module_Class_Members(classname, baseclass, stacksize) to a constructor definition
    $txt =~ s/^(\s*)Module_Class_Members\(\s*([A-Za-z0-9_]+)\s*,\s*([A-Za-z0-9_]+)\s*,\s*([0-9]+)\s*\);?/$1$2() : $3($4) {}/mg;

    # print warnings
    $lineno = 0;
    foreach $linewithcomment (split ("\n", $txt)) {
       $lineno++;
       $linewithcomment =~ s/^\s+//;
       my $lineinfo = "    $fname:$lineno\t$linewithcomment\n";

       my $line = $linewithcomment;
       $line =~ s|//.*||; # avoid warnings for stuff in comments
       $line =~ s|/\*.*?\*/| |g; # C-style comments
       $line =~ s|/\*.*||g; # C-style comment start
       $line =~ s|.*\*/||g; # C-style comment end
       $line =~ s|^\s*\*.*||; # lines beginning with '*' are likely part of comments as well

       # cObject
       if ($line =~ /\bcObject\b/) {
          $warnings{"cObject got renamed to cOwnedObject, check note at bottom whether you need to change the code"} .= $lineinfo;
       }
       if ($line =~ /\bcPolymorphic\b/) {
          $warnings{"cPolymorphic got renamed to cObject, you may want to change the code (old name is still supported)"} .= $lineinfo;
       }

       # simtime_t
       if ($line =~ /\bdouble +[a-z0-9_]*(time|age|interval|delay|\bt[0-9]*\b)/i) {
          $warnings{"Variable might represent simulation time -- if so, change \"double\" to \"simtime_t\""} .= $lineinfo;
       }
       if ($line =~ /\b(simtimeToStr|strToSimtime)\b/) {
          $warnings{"The simtimeToStr() and strToSimtime() methods are no longer supported. (use SimTime methods, or the SIMTIME_STR(t), SIMTIME_DBL(t), STR_SIMTIME(s), SIMTIME_TTOA(buf,t) macros instead)"} .= $lineinfo;
       }

       # dup()
       if ($line =~ /\b(cPolymorphic|cObject|cOwnedObject)\s*\*\s*dup\s*\(\*\)/) {
          $warnings{"dup() in class X should return an X* pointer"} .= $lineinfo;
       }

       # gates
       if ($line =~ /=.*\bsetGateSize\b/) {
          $warnings{"setGateSize() return type changed to void"} .= $lineinfo;
       }
       if ($line =~ /=.*\baddGate\b/) {
          $warnings{"addGate() return type changed to void, use gate(\"name\") to get the pointer"} .= $lineinfo;
       }
       if ($line =~ /'[IO]'/) {
          $warnings{"if 'I'/'O' refers to a gate type, change it to cGate::INPUT or cGate::OUTPUT"} .= $lineinfo;
       }
       if ($line =~ /\b(setTo|setFrom)\b/) {
          $warnings{"The setTo() and setFrom() methods are no longer supported, use connectTo() instead"} .= $lineinfo;
       }

       # messages
       if ($line =~ /\b(setKind|setPriority)\b/ || $line =~ /\bint\b.*(\bkind\(\)|\bpriority\(\))/) {
          $warnings{"note: message kind and priority changed from int to short, you may want to update the code"} .= $lineinfo;
       }
       if ($line =~ /\b(setLength|setByteLength|addLength|addByteLength)\b/ || $line =~ /\blong\b.*(\blength\(\)|\bbyteLength\(\))/) {
          $warnings{"note: message length changed from long to int64, you may want to update the code"} .= $lineinfo;
       }

       # misc
       if ($line =~ /\bcBag\b/) {
          $warnings{"cBag no longer exists, use std::vector instead"} .= $lineinfo;
       }
       if ($line =~ /\bitems\(\)/) {
          $warnings{"cArray::items() renamed to size()"} .= $lineinfo;
       }

       # dynamic module creation
       if ($line =~ /\bbuildInside\b/) {
          $warnings{"finalizeParameters() must be called (for channels, this reads input params from omnetpp.ini; for modules, also creates gates [since gate vector sizes may depend on parameter values])"} .= $lineinfo;
       }

       # dynamic channel creation
       if ($line =~ /\bnew +cDatarateChannel\b/) {
          $warnings{"channel creation should be done via cChannelType, see manual; direct \"new\" won't work properly"} .= $lineinfo;
       }

       # 'ev' not permitted as variable name
       if ($line =~ /[a-zA-Z0-9_] *[&*]? *\bev\b/) {
          $warnings{"\"ev\" became a macro, so it's no longer suitable as a variable or parameter name -- please rename"} .= $lineinfo;
       }

       # exceptions must be thrown by value
       if ($line =~ /\bthrow +new\b/) {
          $warnings{"Exceptions MUST be thrown by value, please remove the \"new\" keyword"} .= $lineinfo;
       }

       # cModule
       if ($line =~ /\bcSubmodIterator\b/) {
          $warnings{"Deprecated cSubmodIterator. Use cModule::SubmoduleIterator instead"} .= $lineinfo;
       }
       if ($line =~ /\bmoduleState\b/) {
          $warnings{"Deprecated moduleState. "} .= $lineinfo;
       }
       if ($line =~ / end\(\)\b/) {  # note: we may give false warning for STL iterator end() methods
          $warnings{"cSimpleModule::end() -- removed, as there was little value in it. To terminate an activity() module, simply return from the activity() method, or call the new halt() method to end the module while preserving the local variables for inspection"} .= $lineinfo;
       }

       if ($line =~ /\bgates\(\)\b/) {
          $warnings{"cModule's gates() method was removed; to iterate over gates of a module, use: for(cModule::GateIterator i(modp); !i.end(); i++) {cGate *gate=i(); ...}"} .= $lineinfo;
       }

       # display string tags
       if ($line =~ /\bbackgroundDisplayString\b/i) {
          $warnings{"There are no separate backgroundDisplayString and displayString. Use displayString instead. P tag become BGP, B tag become BGB"} .= $lineinfo;
       }

       # ev.rdbuf
       if ($line =~ /\bev\.rdbuf\b/) {
          $warnings{"Cannot reference cEnvir internals (e.g. use ini file options to redirect output)"} .= $lineinfo;
       }
    
    }
    open(OUTFILE, ">$fname") || die "cannot open $fname for write";
    print OUTFILE $txt || die "cannot write $fname";
    close OUTFILE;
}

# print all warnings:
print "\n";
foreach $key (sort keys %warnings) {
     print "WARNING: $key\n$warnings{$key}\n";
}

print "\nConversion done. You may safely re-run this script as many times as you want.\n";

print "cObject note: only change a cObject occurrence to cOwnedObject, if:\n";
print "   (1) cObject is not already the result of a cPolymorphic->cObject conversion, and\n";
print "   (2) you are REALLY sure you need cOwnedObject there, with its name string and other overhead.\n";


