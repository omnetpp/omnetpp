#
# Usage:
#
# set ROOT=%~dp0\..\..
# pushd %ROOT%\include && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\sim && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\sim\netbuilder && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\sim\parsim && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\envir && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\cmdenv && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\tkenv && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\nedxml && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\common && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\scave && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\eventlog && perl %~dp0\namespaceize.pl && popd
# pushd %ROOT%\src\layout && perl %~dp0\namespaceize.pl && popd
#


$verbose = 1;

$names = "cOmnetAppRegistration cInspectorFactory cInspectByNameVisitor cCollectObjectsVisitor cCollectChildrenVisitor cFilteredCollectObjectsVisitor cCountChildrenVisitor cCompiledExpressionBase cClassName cAkaroaRNG cWatch cNEDFileLoader cXMLElementList cXMLAttributeMap cRecursiveObjectFinderVisitor cPacket cIniFile cInifile cConfigKey cStringPool cParValueCache cParValueCache2 cConfigurationReader cXMLSAXHandler cXMLDocCache cFileOutputVectorManager cFileOutputScalarManager cFileSnapshotManager cEnvir cGenericReadonlyWatch cGenericAssignableWatch cSnapshotManager cOutputScalarManager cOutputVectorManager cSnapshotManager cArray cBag cModuleInterface cPolymorphic cBasicChannel cBoolPar cChannel cIdealChannel cClassDescriptor cClassFactory cCommBuffer cComponent cComponentType cModuleType cChannelType cCompoundModule cConfigKey cConfiguration cCoroutine cDefaultList cDensityEstBase cTransientDetection cAccuracyDetection cTDExpandingWindows cADByStddev cDisplayString cDoublePar cDynamicExpression cEnum cException cTerminationException cRuntimeError cDeleteModuleException cStackCleanupException cExpression cFSM cMathFunction cGate cHasher cHistogramBase cEqdHistogramBase cLongHistogram cDoubleHistogram cKSplit cLCG32 cLinkedList cLongPar cMersenneTwister cMessage cModule cSubModIterator cMessageHeap cMsgPar cNamedObject cNEDDeclarationBase cNEDFunction cObject cOutVector cOwnedObject cNoncopyableOwnedObject cStaticFlag cPar cParsimCommunications cParValue cProperties cProperty cPSquare cQueue cRNG cScheduler cSequentialScheduler cRealTimeScheduler cSimpleModule extern cSimulation simulation; cSimulation cStatistic cStdDev cWeightedStdDev cStdVectorWatcherBase cStdVectorWatcher cStdPointerVectorWatcher cStdListWatcher cStdPointerListWatcher cStdSetWatcher cStdPointerSetWatcher cStdMapWatcher cStdPointerMapWatcher cStringPar cStringTokenizer cTopology cVarHistogram cVisitor cWatchBase cWatch_bool cWatch_char cWatch_uchar cWatch_stdstring cWatch_cObject cWatch_cObjectPtr cXMLElement cXMLPar cErrorMessages cSymTable cRegistrationList cContextSwitcher cMethodCallContextSwitcher cContextTypeSwitcher cDynamicChannelType cDynamicModuleType cExpressionBuilder cNEDDeclaration cNEDLoader cNEDNetworkBuilder cAdvancedLinkDelayLookahead cCommBufferBase cFileCommunications cFileCommBuffer cIdealSimulationProtocol cISPEventLogger cLinkDelayLookahead cMemCommBuffer cMPICommunications cMPICommBuffer cNamedPipeCommunications cNMPLookahead cNoSynchronization cNullMessageProtocol cParsimPartition cParsimProtocolBase cParsimSynchronizer cPlaceHolderModule cProxyGate cPtrPassingMPICommunications cPtrPassingMPICommBuffer";
$names =~ s/ +/|/g;
$names =~ s/\bc//g;

@hfiles = glob("*.h");
@ccfiles = glob("*.cc");

$fnamesuffix = "";
#$fnamesuffix = ".new";  # for testing

foreach $fname (@hfiles)
{
    print "reading $fname...\n" if ($verbose);
    $txt = readfile($fname);

    $txt =~ s!^(.*\n *# *include .*?\n)(.*)(\n#endif)!$1\nNAMESPACE_BEGIN\n$2\nNAMESPACE_END\n\n$3!s;

    #$txt =~ s/\bc($names)\b/$1/gs;
    #$txt =~ s/\bc($names)s\b/$1s/gs;
    #$txt =~ s/Watch_cObject/Watch_Object/gs;

    writefile($fname . $fnamesuffix, $txt);
}

foreach $fname (@ccfiles)
{
    print "reading $fname...\n" if ($verbose);
    $txt = readfile($fname);

    # add "using namespace" after the last #include or #endif
    $txt =~ s!^([^{]*\n *# *[ei][n][dc][il][fu].*?\n)!$1\nUSING_NAMESPACE\n!s;

    #$txt =~ s/\bc($names)\b/$1/gs;
    #$txt =~ s/\bc($names)s\b/$1s/gs;
    #$txt =~ s/Watch_cObject/Watch_Object/gs;

    writefile($fname . $fnamesuffix, $txt);
}

sub readfile ()
{
    my $fname = shift;
    my $content;
    open FILE, "$fname" || die "cannot open $fname";
    read(FILE, $content, 1000000);
    close FILE;
    $content;
}

sub writefile ()
{
    my $fname = shift;
    my $content = shift;
    open FILE, ">$fname" || die "cannot open $fname for write";
    print FILE $content;
    close FILE;
}




