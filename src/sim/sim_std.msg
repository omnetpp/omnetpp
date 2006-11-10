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

//
// Define operator<< for some classes
//
std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.toString() << "\"";
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

class cNoncopyableObject extends cObject
{
}

class cDefaultList extends cNoncopyableObject
{
    properties:
        omitGetVerb = true;
    fields:
        cObject *defaultListGet[] @sizeGetter(defaultListItems) @label(contents) @group(contents) @hints(list of objects directly held by this component);
}

class cPar;
class cComponentType;
class noncobject cDisplayString;

class cComponent extends cDefaultList
{
    fields:
        @omitGetVerb(true);
        bool isModule @group(general) @hint(whether this is a module or a channel);
        cComponentType *componentType @group(general) @hint(the object that represents the type declaration for this component);
        cPar par[] @sizeGetter(params) @label(parameters) @group(parameters, gates) @hint(parameters, declared in NED);
}

class cChannel extends cComponent
{
}

class cBasicChannel extends cChannel
{
    fields:
        @omitGetVerb(true);
        // Note: no need to spell out delay etc, they come from cComponent parameters
        bool isBusy @group(state) @hint(if channel is currently transmitting);
        simtime_t transmissionFinishes @group(state) @hint(if busy: when currently active transmissions will finish);
}

class cGate;

class cModule extends cComponent
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
        cGate *gate[] @sizeGetter(gates) @group(parameters, gates) @label(gates) @hint(module gates);
}

class cSimpleModule extends cModule
{
    fields:
        @omitGetVerb(true);
        bool isTerminated @group(state) @hint(whether this module has terminated);
        bool usesActivity @group(activity) @hint(whether this simple module was programmed using activity or handleMessage C++ method);
        unsigned int stackSize @group(activity) @hint(with activity: stack size allocated for this module -- zero means info not available);
        unsigned int stackUsage @group(activity) @hint(with activity: stack actually used by this module -- zero means info not available);
}

class cCompoundModule extends cModule
{
}

//----

class cComponentType extends cNoncopyableObject
{
    fields:
        @omitGetVerb(true);
        string description @group(general);
}

class cModuleType extends cComponentType
{
    fields:
        @omitGetVerb(true);
        bool isNetwork @group(general) @hint(whether this module type can be used as toplevel module);
}

class cChannelType extends cComponentType
{
}

//----

class cGate extends cNoncopyableObject
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
        string value @getter(toString) @setter(parse) @editable @hint(the editable display string itself);
}

class cArray extends cObject
{
    fields:
        @omitGetVerb(true);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable @hint(whether the array should own the objects inserted into it);
        cObject *contents[] @getter(get) @sizeGetter(items) @hint(objects contained in the array);
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

        //XXX various IDs
}

class cMessagePar extends cObject
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

class cExpression extends cPolymorphic
{
    fields:
        @omitGetVerb(true);
        string toString;
}

class cFSM extends cObject
{
    fields:
        @omitGetVerb(true);
        int state;
        string stateName;
}

class cMathFunction extends cNoncopyableObject
{
    fields:
        @omitGetVerb(true);
        int numArgs;
}

class cNEDFunction extends cNoncopyableObject
{
    fields:
        @omitGetVerb(true);
        char returnType;
        int numArgs;
        string argTypes;
}

class cOutVector extends cNoncopyableObject
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
        string value @getter(toString) @setter(parse) @editable;
        char type @enum(cPar_Type);
        bool isNumeric @group(flags);
        bool isConstant @group(flags);
        bool isVolatile @group(lags);
        bool isSet @group(flags);
        bool hasDefaultValue @group(flags);
}

class cProperties extends cPolymorphic
{
}

class cProperty extends cPolymorphic
{
    fields:
        @omitGetVerb(true);
        bool isImplicit @group(general);
        string index @group(general);
        //XXX const std::vector<const char *>& keys @group(general);
}

class cLinkedList extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @hint(number of items contained);
}

class cMessageHeap extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @hint(number of items contained);
        cMessage *messages[] @getter(peek) @sizeGetter(length) @hint(self-messages scheduled, and messages in transit);
}

class cQueue extends cObject
{
    fields:
        @omitGetVerb(true);
        int length @hint(number of items in the queue);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable @hint(whether the queue should own the objects inserted into it);
        //FIXME add getter for elements
}

class cSimulation extends cNoncopyableObject
{
    fields:
        @omitGetVerb(true);
        int runNumber @hint(identifies the [Run X] section selected in omnetpp.ini);
        simtime_t simTime @hint(the current simulation time in seconds);
        long eventNumber @hint(the current event number -- counts from 0 up);
        cMessageHeap messageQueue @label(futureEvents) @hint(self-messages scheduled, and messages in transit);
        int lastModuleId @group(modules) @hint(the largest module ID issued so far);
        cModule *systemModule @group(modules) @hint(the top-level module);
        cModule *module[] @sizeGetter(lastModuleId) @group(modules) @label(modules) @hint(list of modules, indexed with module ID);
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



