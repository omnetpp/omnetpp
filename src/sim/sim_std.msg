//==============================================================
//   SIM_STD.MSG  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==============================================================

//--------------------------------------------------------------
// Copyright (C) 1992-2005 Andras Varga
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------


//
// Property descriptors for the simkernel classes.
// Usage: opp_msgc -Xnc -Xns sim_std.ins
//


cplusplus
{{

//
// Register enums from the sim. kernel
//
/*XXX
EXECUTE_ON_STARTUP(cGate_Type,
    cEnum *e = new cEnum("cGate_Type");
    enums.instance()->add(e);
    e->insert(cGate::INPUT,  "INPUT");
    e->insert(cGate::OUTPUT, "OUTPUT");
    e->insert(cGate::INOUT,  "INOUT");
);

EXECUTE_ON_STARTUP(cPar_Type,
    cEnum *e = new cEnum("cPar_Type");
    enums.instance()->add(e);
    e->insert(cPar::BOOL,   "BOOL");
    e->insert(cPar::DOUBLE, "DOUBLE");
    e->insert(cPar::LONG,   "LONG");
    e->insert(cPar::STRING, "STRING");
    e->insert(cPar::XML,    "XML");
);
*/

//
// Define operator<< for some classes
//
std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.getString() << "\"";
}

/*XXX
typedef cDensityEstBase::Cell cDensityEstBase_Cell;
std::ostream& operator<<(std::ostream& os, const cDensityEstBase::Cell& cell) {
    return os << "[" << cell.lower << ", " << cell.upper << ")  ==>  " << cell.value << " (" << cell.relativeFreq << ")";
}
*/

//
// This function is to be referenced from sim. kernel classes, to force the
// linker to include this file in executables
//
void std_sim_descriptor_dummy() {}

}}

enum cPar_Type;
enum cGate_Type;

//XXX struct cDensityEstBase_Cell;


class cObject
{
    fields:
        @omitGetVerb(true);
        string className @group(basic);
        string fullName @group(basic) @label(name) @tooltip(name with index);
        string fullPath @group(basic);
        string info @group(basic);
        string detailedInfo @group(basic);
        cObject *owner @group(owner or parent);
}

class cDefaultList extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        cObject *defaultListGet[] @sizeGetter(defaultListItems) @label(contents) @group(contents);
}

class cPar;
class noncobject cDisplayString;

class cChannel extends cObject
{
}

class cBasicChannel extends cChannel
{
    fields:
        @omitGetVerb(true);
        //XXX add delay etc
        bool isBusy @group(state);
        simtime_t transmissionFinishes @group(state);
}

class cGate;

class cModule extends cDefaultList
{
    fields:
        @omitGetVerb(true);
        int id @group(general);
        bool isSimple @group(general);
        bool isVector @group(general);
        int index @group(general);
        int size @group(general);
        cDisplayString displayString @group(general);
        cDisplayString backgroundDisplayString @group(general);
        cPar par[] @sizeGetter(params) @label(parameters) @group(parameters, gates);
        cGate *gate[] @sizeGetter(gates) @group(parameters, gates) @label(gates);
}

class cSimpleModule extends cModule
{
    fields:
        @omitGetVerb(true);
        //XXX bool isTerminated @group(state);
        bool usesActivity @group(activity);
        unsigned int stackSize @group(activity);
        unsigned int stackUsage @group(activity);
}

class cCompoundModule extends cModule
{
}

//----

//XXX class cComponentType extends cObject
//XXX class cModuleType extends cComponentType
//XXX class cChannelType extends cComponentType

//----

class cGate extends cObject
{
    fields:
        @omitGetVerb(true);
        char type @group(general) @enum(cGate_Type);
        int id @group(general);
        bool isVector @group(general);
        int index @group(general);
        cDisplayString displayString @group(general);
        bool isBusy @group(state);
        simtime_t transmissionFinishes @group(state);
        cGate *fromGate @group(connection);
        cGate *toGate @group(connection);
        cGate *sourceGate @group(connection) @tooltip(end of 'toGate' chain);
        cGate *destinationGate @group(connection) @tooltip(beginning of 'fromGate' chain);
        bool isConnectedOutside @group(connection);
        bool isConnectedInside @group(connection);
        bool isConnected @group(connection);
        bool isRouteOK @group(connection);
        cChannel *channel @group(connection);
}

//----

class cDisplayString
{
    fields:
        @omitGetVerb(true);
        string getString @label(string);
}

class cArray extends cObject
{
    fields:
        @omitGetVerb(true);
        bool takeOwnership @group(general);
        cObject *get[] @sizeGetter(items) @label(contents) @group(general);
}

class cMessage extends cObject
{
    fields:
        @omitGetVerb(true);
        int kind @group(general);
        int priority @group(general);
        long length @group(general);
        long byteLength @group(general);
        bool hasBitError @group(general);
        string displayString @group(general);

        bool isScheduled @group(sending);
        bool isSelfMessage @group(sending);
        simtime_t creationTime @group(sending);
        simtime_t sendingTime @group(sending);
        simtime_t arrivalTime @group(sending);
        simtime_t timestamp @group(sending);
        int senderModuleId @group(sending);
        int senderGateId @group(sending);
//XXX        cModule *senderModule @group(sending);
//XXX        cGate *senderGate @group(sending);
        int arrivalModuleId @group(sending);
        int arrivalGateId @group(sending);
//XXX        cModule *arrivalModule @group(sending);
//XXX        cGate *arrivalGate @group(sending);

        cMessage *encapsulatedMsg @group(general);
        cPolymorphic *controlInfo @group(general);
        //XXX void *contextPointer
        //XXX various IDs
}

class cPacket extends cMessage
{
    fields:
        @omitGetVerb(true);
        short protocol @group(general);
        short pdu @group(general);
}

//----

class cStatistic extends cObject
{
    fields:
        @omitGetVerb(true);
        long samples @group(general);
        double weights @group(general);
        double sum @group(general);
        double sqrSum @group(general);
        double min @group(general);
        double max @group(general);
        double mean @group(general);
        double stddev @group(general);
        double variance @group(general);
}

class cStdDev extends cStatistic
{
}

class cWeightedStdDev extends cStdDev
{
}

class cDensityEstBase extends cStdDev
{
    fields:
        @omitGetVerb(true);
        bool transformed @group(histogram);
        unsigned long underflowCell @group(histogram);
        unsigned long overflowCell @group(histogram);
        int cells @group(histogram);
        //XXX cDensityEstBase_Cell cellInfo[] @sizeGetter(cells) @group(histogram);
}

class cHistogramBase extends cDensityEstBase
{
}

class cEqdHistogramBase extends cHistogramBase
{
}

class cVarHistogram extends cHistogramBase
{
}

class cLongHistogram extends cEqdHistogramBase
{
}

class cDoubleHistogram extends cEqdHistogramBase
{
}

class cKSplit extends cDensityEstBase
{
}

class cPSquare extends cDensityEstBase
{
}

class cTransientDetection extends cObject
{
}

class cAccuracyDetection extends cObject
{
}

class cTDExpandingWindows extends cTransientDetection
{
}

class cADByStddev extends cAccuracyDetection
{
}

//----

class cFSM extends cObject
{
    fields:
        @omitGetVerb(true);
        int state @group(general);
        string stateName @group(general);
}

//XXX class cMathFunction extends cObject
//XXX {
//XXX     fields:
//XXX         @omitGetVerb(true);
//XXX         int numArgs @group(general);
//XXX }

class cOutVector extends cObject
{
    fields:
        @omitGetVerb(true);
        bool isEnabled @group(general);
        long valuesReceived @group(general);
        long valuesStored @group(general);
}

class cPar extends cObject
{
    fields:
        @omitGetVerb(true);
        char type @group(general) @enum(cPar_Type);
        bool isNumeric @group(general);
        bool isInput @group(general);
//XXX        bool isVolatile @group(general);
//XXX        bool isConstant @group(general);
//XXX        bool isSet @group(general);
//XXX        bool hasDefaultValue @group(general);
//XXX        string toString @group(general);
}

class cLinkedList extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @group(general);
}

class cMessageHeap extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @group(general);
        cMessage *peek[] @sizeGetter(length) @group(general) @label(messages);
}

class cQueue extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @group(general);
        bool takeOwnership @group(general);
        //XXX elements?
}

class cSimulation extends cObject
{
    fields:
        @omitGetVerb(true);
        int runNumber @group(general);
        simtime_t simTime @group(general);
        long eventNumber @group(general);
        //int contextType @group(general);
//XXX        cMessageHeap messageQueue @label(futureEvents) @group(general);
        int lastModuleId @group(modules);
        cModule *systemModule @group(modules);
        cModule *module[] @sizeGetter(lastModuleId) @group(modules) @label(modules);
}

class cScheduler extends cPolymorphic
{
}

class cSequentialScheduler extends cScheduler
{
}

class cRealTimeScheduler extends cScheduler
{
}

class cTopology extends cObject
{
    fields:
        @omitGetVerb(true);
        int nodes @group(general);
}

class cXMLElement
{
    fields:
        @omitGetVerb(true);
        string getTagName @group(general) @label(tagName);
        string getNodeValue @group(general) @label(nodeValue);
        string getSourceLocation @group(general) @label(sourceLocation);
        bool hasAttributes @group(general);
        bool hasChildren @group(general);
        //XXX const cXMLAttributeMap& getAttributes @group(general);
        //XXX cXMLElementList getChildren @group(general);
}



