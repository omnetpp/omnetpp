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
EXECUTE_ON_STARTUP(cGate_Type,
    cEnum *e = new cEnum("cGate_Type");
    enums.instance()->add(e);
    e->insert('I', "INPUT");
    e->insert('O', "OUTPUT");
);

EXECUTE_ON_STARTUP(cPar_Type,
    cEnum *e = new cEnum("cPar_Type");
    enums.instance()->add(e);
    e->insert('S', "STRING");
    e->insert('B', "BOOL");
    e->insert('L', "LONG");
    e->insert('D', "DOUBLE");
    e->insert('F', "MATH FUNCTION");
    e->insert('T', "DISTRIBUTION");
    e->insert('C', "COMPILED EXPRESSION");
    e->insert('X', "EXPRESSION");
    e->insert('I', "INDIRECTION");
    e->insert('P', "VOID* POINTER");
    e->insert('O', "OBJECT POINTER");
    e->insert('M', "XML");
);

//
// Define operator<< for some classes
//
std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.getString() << "\"";
}

typedef cDensityEstBase::Cell cDensityEstBase_Cell;
std::ostream& operator<<(std::ostream& os, const cDensityEstBase::Cell& cell) {
    return os << "[" << cell.lower << ", " << cell.upper << ")  ==>  " << cell.value << " (" << cell.relativeFreq << ")";
}

//
// This function is to be referenced from sim. kernel classes, to force the
// linker to include this file in executables
//
void std_sim_descriptor_dummy() {}

}}

enum cPar_Type;
enum cGate_Type;

struct cDensityEstBase_Cell;


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
        cObject *get[] @sizeGetter(items) @label(contents);
}

class cMessage extends cObject
{
    fields:
        @omitGetVerb(true);
        int kind @group(message);
        int priority @group(message);
        long length @group(message);
        long byteLength @group(message);
        bool hasBitError @group(message);
        string displayString @group(message);

        bool isScheduled @group(sending);
        bool isSelfMessage @group(sending);
        simtime_t creationTime @group(sending);
        simtime_t sendingTime @group(sending);
        simtime_t arrivalTime @group(sending);
        simtime_t timestamp @group(sending);
        int senderModuleId @group(sending);
        int senderGateId @group(sending);
        cModule *senderModule @group(sending);
        cGate *senderGate @group(sending);
        int arrivalModuleId @group(sending);
        int arrivalGateId @group(sending);
        cModule *arrivalModule @group(sending);
        cGate *arrivalGate @group(sending);

//XXX PARAMETERS???

        cMessage *encapsulatedMsg;
        cPolymorphic *controlInfo;
        //XXX void *contextPointer
}

class cPacket extends cMessage
{
    fields:
        @omitGetVerb(true);
        short protocol @group(packet);
        short pdu @group(packet);
}

//----

class cStatistic extends cObject
{
    fields:
        @omitGetVerb(true);
        long samples;
        double weights;
        double sum;
        double sqrSum;
        double min;
        double max;
        double mean;
        double stddev;
        double variance;
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
        cDensityEstBase_Cell cellInfo[] @sizeGetter(cells) @group(histogram);
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
        int state;
        string stateName;
}

class cFunctionType extends cObject
{
    fields:
        @omitGetVerb(true);
        int argCount;
}

class cOutVector extends cObject
{
    fields:
        @omitGetVerb(true);
        bool isEnabled;
        long valuesReceived;
        long valuesStored;
}

class cPar extends cObject
{
    fields:
        @omitGetVerb(true);
        char type @enum(cPar_Type);
        bool isNumeric;
        bool isInput;
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
        int length;
}

class cMessageHeap extends cObject
{
    fields:
        @omitGetVerb(true);
        int length;
        cMessage *peek[] @sizeGetter(length) @label(messages);
}

class cQueue extends cObject
{
    fields:
        @omitGetVerb(true);
        int length;
        bool takeOwnership @group(general);
        //XXX elements?
}

class cSimulation extends cObject
{
    fields:
        @omitGetVerb(true);
        int runNumber;
        simtime_t simTime;
        long eventNumber;
        cMessageHeap messageQueue @label(futureEvents);
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
        string getTagName @label(tagName);
        string getNodeValue @label(nodeValue);
        string getSourceLocation @label(sourceLocation);
        bool hasAttributes;
        bool hasChildren;
        //XXX const cXMLAttributeMap& getAttributes;
        //XXX cXMLElementList getChildren;
}



