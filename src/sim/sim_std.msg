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
        string className @group(basic) @hint(C++ class name);
        string name @group(basic) @editable @hint(base name);
        string fullName @group(basic) @hint(name with index);
        string fullPath @group(basic) @hint(hierarchical name, follows ownership relations);
        string info @group(basic) @hint(text provided by the object's "info" method);
        string detailedInfo @group(basic) @hint(text provided by the object's "detailedInfo" method);
        cObject *owner @group(owner or parent) @hint(module or container where this object belongs);
}

class cDefaultList extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        cObject *defaultListGet[] @sizeGetter(defaultListItems) @label(contents) @group(contents) @hints(list of objects directly held by this component);
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
        double delay @hint(transmission delay in seconds);
        double error @hint(bit error rate);
        double datarate @hint(datarate in bit/sec);
        bool disabled @hint(channel state);
        bool isBusy @group(state) @hint(if channel is currently transmitting);
        simtime_t transmissionFinishes @group(state) @hint(if busy: when currently active transmissions will finish);
}

class cGate;

class cModule extends cDefaultList
{
    fields:
        @omitGetVerb(true);
        int id @group(general) @hint(unique module ID -- IDs of deleted modules are not issued again);
        bool isSimple @group(general) @hint(whether this is a simple module);
        bool isVector @group(general) @hint(whether this module is part of a module vector);
        int index @group(general) @hint(if vector: index of this module in its module vector);
        int size @group(general) @hint(if vector: size of module vector this module belongs to);
        cDisplayString displayString @group(general) @hint(controls appearance of this module on the GUI);
        cDisplayString backgroundDisplayString @group(general) @hint(controls appearance of the background when this module gets opened on the GUI);
        cPar par[] @sizeGetter(params) @label(parameters) @group(parameters, gates) @hint(module parameters, declared in NED);
        cGate *gate[] @sizeGetter(gates) @group(parameters, gates) @label(gates) @hint(module gates);
}

class cSimpleModule extends cModule
{
    fields:
        @omitGetVerb(true);
        bool usesActivity @group(activity) @hint(whether this simple module was programmed using activity or handleMessage C++ method);
        unsigned int stackSize @group(activity) @hint(with activity: stack size allocated for this module -- zero means info not available);
        unsigned int stackUsage @group(activity) @hint(with activity: stack actually used by this module -- zero means info not available);
}

class cCompoundModule extends cModule
{
}

//----

class cModuleInterface extends cObject
{
    fields:
        @omitGetVerb(true);
        int numParams @group(parameters) @hint(number of parameters declared in NED);
        string paramName[] @sizeGetter(numParams) @group(parameters) @hint(parameter names);
        string paramType[] @sizeGetter(numParams) @group(parameters) @hint(parameter types);
        bool isParamConst[] @sizeGetter(numParams) @group(parameters) @hint(whether parameter was declared const);
        int numGates @group(gates) @hint(number of gates declared in NED);
        string gateName[] @sizeGetter(numParams) @group(gates) @hint(gate names);
        char gateType[] @sizeGetter(numParams) @group(gates) @hint(gate types);
        bool isGateVector[] @sizeGetter(numParams) @group(gates) @hint(whether gate was declared vector);
};

//----

class cGate extends cObject
{
    fields:
        @omitGetVerb(true);
        char type @group(general) @enum(cGate_Type) @hint(gate type);
        int id @group(general) @hint(gate id, unique within this module);
        bool isVector @group(general) @hint(whether this gate is part of a gate vector);
        int index @group(general) @hint(if vector: index of this gate within its gate vector);
        cDisplayString displayString @group(general) @hint(affects appearance of connection arrow on the "to" side);
        bool isBusy @group(state) @hint(only if the connection has a channel with nonzero datarate);
        simtime_t transmissionFinishes @group(state) @hint(only if the connection has a channel with nonzero datarate);
        cGate *fromGate @group(connection);
        cGate *toGate @group(connection);
        cGate *sourceGate @group(connection) @hint(end of 'toGate' chain);
        cGate *destinationGate @group(connection) @hint(beginning of 'fromGate' chain);
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
        string value @getter(getString) @setter(parse) @editable;
}

class cArray extends cObject
{
    fields:
        @omitGetVerb(true);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable;
        cObject *get[] @sizeGetter(items) @label(contents);
}

class cMessage extends cObject
{
    fields:
        @omitGetVerb(true);
        int kind @group(message) @editable;
        int priority @group(message) @editable;
        long length @group(message) @editable;
        long byteLength @group(message) @editable;
        bool hasBitError @group(message) @setter(setBitError) @editable;
        string displayString @group(message);
        cArray parList @group(obsolete);

        cMessage *encapsulatedMsg;
        cPolymorphic *controlInfo;

        bool isScheduled @group(sending);
        bool isSelfMessage @group(sending);
        simtime_t creationTime @group(sending);
        simtime_t sendingTime @group(sending);
        simtime_t arrivalTime @group(sending);
        simtime_t timestamp @group(sending) @editable;
        int senderModuleId @group(sending);
        int senderGateId @group(sending);
        cModule *senderModule @group(sending);
        cGate *senderGate @group(sending);
        int arrivalModuleId @group(sending);
        int arrivalGateId @group(sending);
        cModule *arrivalModule @group(sending);
        cGate *arrivalGate @group(sending);
}

class cPacket extends cMessage
{
    fields:
        @omitGetVerb(true);
        short protocol @group(packet) @editable;
        short pdu @group(packet) @editable;
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
        bool isEnabled @setter(setEnabled) @editable;
        long valuesReceived;
        long valuesStored;
}

class cPar extends cObject
{
    fields:
        @omitGetVerb(true);
        string value @getter(getAsText) @setter(setFromText) @editable;
        char type @enum(cPar_Type);
        string prompt @editable;
        bool isNumeric @group(flags);
        bool isConstant @group(flags);
        bool isInput @setter(setInput) @editable @group(flags);
        bool isRedirected @group(flags);
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
        bool takeOwnership @group(general) @setter(takeOwnership) @editable;
        //Note: elements are available under contents[]
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

cplusplus {{
typedef cTopology::Node cTopology_Node;
typedef cTopology::Link cTopology_Link;
typedef cTopology::LinkIn cTopology_LinkIn;
typedef cTopology::LinkOut cTopology_LinkOut;
}}

class noncobject cTopology_Node;
class noncobject cTopology_Link;
class noncobject cTopology_LinkIn;
class noncobject cTopology_LinkOut;

class cTopology_Node
{
    fields:
        @omitGetVerb(true);
        int moduleId @hint(ID of the module this node corresponds to);
        cModule *module @hint(the module this node corresponds to);
        double weight @hint(node weight -- affects graph algorithms such as shortest path);
        bool enabled @hint(node state -- affects graph algorithms such as shortest path);

        cTopology_LinkIn *inLink[] @getter(in) @sizeGetter(inLinks) @hint(incoming links of this graph node);
        cTopology_LinkIn *outLink[] @getter(out) @sizeGetter(outLinks) @hint(outgoing links of this graph node);

        double distanceToTarget @group(shortest path) @hint(after running a shortest path algorithm: distance to the target node);
        cTopology_LinkOut *path[] @getter(path) @sizeGetter(paths) @group(shortest path) @hint(the next link of the shortest path or paths towards the target node);
}

class cTopology_Link
{
    fields:
        @omitGetVerb(true);
        double weight @hint(link weight -- affects graph algorithms such as shortest path);
        bool enabled @hint(link state -- affects graph algorithms such as shortest path);
}

class cTopology_LinkIn extends cTopology_Link
{
    fields:
        @omitGetVerb(true);
        cTopology_Node *remoteNode @hint(the node at the remote end of this connection);
        cGate *remoteGate @hint(the gate at the remote end of this connection);
        cGate *localGate @hint(the gate at the local end of this connection);
}

class cTopology_LinkOut extends cTopology_Link
{
    fields:
        @omitGetVerb(true);
        cTopology_Node *remoteNode @hint(the node at the remote end of this connection);
        cGate *remoteGate @hint(the gate at the remote end of this connection);
        cGate *localGate @hint(the gate at the local end of this connection);
}

class cTopology extends cObject
{
    fields:
        @omitGetVerb(true);
        int nodes @group(general) @hint(number of nodes in this topology object);
        cTopology_Node *node[] @sizeGetter(nodes) @hint(list of nodes in this topology object);
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
        string detailedInfo @label(contents);
}



