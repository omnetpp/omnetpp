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
        string className @group(base) @hint(C++ class name);
        string name @group(base) @editable @hint(an editable string attribute of the object);
        string fullName @group(base) @hint(if object is part of a module/gate vector: name with index);
        string fullPath @group(base) @hint(hierarchical name, composed of full names of this object and its owners/parents);
        string info @group(base) @hint(text provided by the object's "info" method);
        string detailedInfo @group(base) @hint(text provided by the object's "detailedInfo" method);
        cObject *owner @group(owner or parent) @hint(module or container this object belongs to);
}

class cDefaultList extends cObject
{
    properties:
        omitGetVerb = true;
    fields:
        cObject *contents[] @getter(defaultListGet) @sizeGetter(defaultListItems) @group(contents) @hint(list of objects directly held by this component);
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
        cDisplayString displayString @group(general) @hint(controls appearance of this module in the GUI);
        cDisplayString backgroundDisplayString @group(general) @hint(controls appearance of the background when this module gets opened in the GUI);
        cPar par[] @sizeGetter(params) @label(parameters) @group(parameters, gates) @hint(module parameters);
        cGate *gate[] @sizeGetter(gates) @group(parameters, gates) @label(gates) @hint(module gates);
}

class cSimpleModule extends cModule
{
    fields:
        @omitGetVerb(true);
        bool usesActivity @group(activity) @hint(whether this simple module was programmed using the "activity" or the "handleMessage" C++ method);
        unsigned int stackSize @group(activity) @hint(if module uses "activity": stack size allocated for this module -- zero means info not available);
        unsigned int stackUsage @group(activity) @hint(if module uses "activity": stack actually used by this module -- zero means info not available);
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

        bool isBusy @group(state) @hint(only if the connection has a channel with nonzero datarate: whether there is an ongoing transmission on the channel);
        simtime_t transmissionFinishes @group(state) @hint(only if the connection has a channel with nonzero datarate: if busy: when currently active transmissions will finish);

        cChannel *channel @group(connection) @hint(channel object associated with the connection);

        cGate *toGate @group(connection) @hint(the gate this one is connected to, following the arrow direction -- for input gates, if set, it points inwards into the compound module);
        cGate *fromGate @group(connection) @hint(gate on the opposite side as "toGate" -- "fromGate" of an output gate, if set, usually points to a submodule of this module);
        bool isConnectedOutside @group(connection) @hint(whether if this gate is connected to the parent module or a sibling module);
        bool isConnectedInside @group(connection) @hint(true if this gate is connected to a submodule of this module, or belongs to a "thru" connection inside this compound module);

        cGate *sourceGate @group(connection) @hint(the last gate in the "fromGate->fromGate->fromGate..." chain);
        cGate *destinationGate @group(connection) @hint(the last gate in the "toGate->toGate->toGate..." chain);
        bool isConnected @group(connection) @hint(whether the gate is fully connected -- both inside and outside for a compound module gate, and outside for a simple module gate);
        bool isRouteOK @group(connection) @hint(the chain of connections is OK if it starts and ends at simple modules);
}

//----

class cDisplayString
{
    fields:
        @omitGetVerb(true);
        string value @getter(getString) @setter(parse) @editable @hint(the editable display string itself);
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
        int kind @group(message) @editable @hint(generic user-settable attribute, usually carries "type" or "role" of message);
        int priority @group(message) @editable @hint(controls delivery order of messages with equal timestamps);
        long length @group(message) @editable @hint(simulated length of the message in bits, affects transmission time and probability of bit errors when sent through a channel);
        long byteLength @group(message) @editable @hint(length in bytes, i.e. length in bits divided by eight);
        bool hasBitError @group(message) @setter(setBitError) @editable @hint(signals that a bit error occurred when the message was sent through a channel with nonzero bit error rate);
        string displayString @group(message) @hint(controls appearance of the message in GUI);
        cArray parList @group(obsolete) @hint(use of "parList" should be replaced with message subclassing in the few models that still use it);

        cMessage *encapsulatedMsg @hint(used with protocol stacks: stores an encapsulated higher-layer packet);
        cPolymorphic *controlInfo @hint(used with protocol stacks: carries extra information when a packet is sent between protocol layers);

        bool isScheduled @group(sending) @hint(whether self-message is currently scheduled, or message currently is in transit);
        bool isSelfMessage @group(sending) @hint(whether message is a self-message, scheduled to be delivered back to the same module at a later time);
        simtime_t creationTime @group(sending) @hint(simulation time the message was created);
        simtime_t sendingTime @group(sending) @hint(simulation time of the current or last sending/scheduling of the message);
        simtime_t arrivalTime @group(sending) @hint(simulation time the message will be/was delivered to the destination module);
        simtime_t timestamp @group(sending) @editable @hint(user-settable timestamp for generic use);

        int senderModuleId @group(sending) @hint(ID of the module performing the current or last send/schedule operation);
        int senderGateId @group(sending) @hint(module-local ID of the gate on which the message was sent out -- -1==none for self-messages and after "sendDirect");
        cModule *senderModule @group(sending) @hint(ID of the module performing the current or last send/schedule operation);
        cGate *senderGate @group(sending) @hint(the gate on which the message was sent out -- NULL for self-messages and after "sendDirect");

        int arrivalModuleId @group(sending) @hint(ID of the arrival module of the current or last sending/scheduling);
        int arrivalGateId @group(sending) @hint(module-local ID of arrival gate for current or last sending -- -1==none for self-messages);
        cModule *arrivalModule @group(sending) @hint(arrival module for current or last sending/scheduling);
        cGate *arrivalGate @group(sending) @hint(arrival gate for current or last sending -- NULL for self-messages);
}

class cPacket extends cMessage
{
    fields:
        @omitGetVerb(true);
        short protocol @group(packet) @editable @hint(declared on cPacket, which is an obsolete class);
        short pdu @group(packet) @editable @hint(declared on cPacket, which is an obsolete class);
}

//----

class cStatistic extends cObject
{
    fields:
        @omitGetVerb(true);
        long samples @hint(number of observations collected so far);
        double weights @hint(sum of weights, which equals the number of observations in the unweighted case);
        double sum @hint(sum of observations);
        double sqrSum @hint(sum of squares of the observations);
        double min @hint(minimum of the observations);
        double max @hint(maximum of the observations);
        double mean @hint(mean of the observations);
        double stddev @hint(standard deviation of the observations);
        double variance @hint(variance of the observations);
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
        bool transformed @group(histogram) @hint(if false, the object is still collecting initial observations to be used for laying out histogram cells);
        unsigned long underflowCell @group(histogram) @hint(number of observations below the first histogram cell);
        unsigned long overflowCell @group(histogram) @hint(number of observations above the last histogram cell);
        int cells @group(histogram) @hint(number of cells in the histogram);
        cDensityEstBase_Cell cellInfo[] @sizeGetter(cells) @group(histogram) @hint(histogram cell bounds, observation count in the cell, and density estimate in the cell);
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
        int state @hint(numeric code of the state the FSM is currently in);
        string stateName @hint(name of the current FSM state);
}

class cFunctionType extends cObject
{
    fields:
        @omitGetVerb(true);
        int argCount @hint(number of "double" parameters this function takes);
}

class cOutVector extends cObject
{
    fields:
        @omitGetVerb(true);
        bool isEnabled @setter(setEnabled) @editable @hint(if disabled, values passed to this object to record are simply discarded);
        long valuesReceived @hint(number of values passed to this object to record);
        long valuesStored @hint(number of values actually stored on the disk);
}

class cPar extends cObject
{
    fields:
        @omitGetVerb(true);
        string value @getter(getAsText) @setter(setFromText) @editable @hint(parameter value);
        char type @enum(cPar_Type) @hint(parameter type);
        string prompt @editable @hint(displayed when an input parameter is to be entered by the user);
        bool isNumeric @group(flags) @hint(whether parameter is of numeric type);
        bool isConstant @group(flags) @hint(whether the parameter was declared const);
        bool isInput @setter(setInput) @editable @group(flags) @hint(whether parameter value is to be read from omnetpp.ini or from the user);
        bool isRedirected @group(flags) @hint(whether this parameter is a "link" to another cPar object);
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
        cMessage *messages[] @getter(peek) @sizeGetter(length) @hint(self-messages currently scheduled and messages currently in transit);
}

class cQueue extends cObject
{
    fields:
        @omitGetVerb(true);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable @hint(whether the queue should own the objects inserted into it);
        int length @hint(number of items in the queue);
        cObject *contents[] @getter(get) @sizeGetter(length) @hint(queue contents);
}

class cSimulation extends cObject
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
        string getTagName @label(tagName) @hint(XML element tag);
        string getNodeValue @label(nodeValue) @hint(contents of text node inside this XML element);
        string getSourceLocation @label(sourceLocation) @hint(location this XML element was parsed from);
        bool hasAttributes @hint(whether this XML element has attributes);
        bool hasChildren @hint(whether this XML element has child elements);
        string detailedInfo @label(contents);
}



