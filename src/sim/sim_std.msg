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
// Usage: opp_msgc -Xnc -Xns sim_std.msg
//

cplusplus
{{
USING_NAMESPACE
//
// Register enums from the sim. kernel
//
typedef cPar::Type   cPar_Type;
typedef cGate::Type  cGate_Type;

Register_Enum(cPar_Type,  (cPar::BOOL, cPar::DOUBLE, cPar::LONG, cPar::STRING, cPar::XML));
Register_Enum(cGate_Type, (cGate::INPUT, cGate::OUTPUT, cGate::INOUT));


//
// Define operator<< for some classes
//
std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.str() << "\"";
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
        @omitGetVerb(true);
        string className @group(base) @hint(C++ class name);
        string fullName @group(base) @hint(if object is part of a module/gate vector: name with index);
        string fullPath @group(base) @hint(hierarchical name, composed of full names of this object and its owners/parents);
        string info @group(base) @hint(text provided by the object's "info" method);
        string detailedInfo @group(base) @hint(text provided by the object's "detailedInfo" method);
}

class cNamedObject extends cObject
{
        @omitGetVerb(true);
        string name @group(base) @editable @hint(an editable string attribute of the object, also used for fullName and fullPath);
}

class cOwnedObject extends cNamedObject
{
        @omitGetVerb(true);
        cObject *owner @group(base) @hint(module or container this object belongs to);
}

class cNoncopyableOwnedObject extends cOwnedObject
{
}

class cDefaultList extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        cOwnedObject *contents[] @getter(defaultListGet) @sizeGetter(defaultListSize) @group(contents) @hint(list of objects directly held by this component);
}

class cPar;
class cComponentType;
class noncobject cDisplayString;

class cProperty extends cObject
{
        @omitGetVerb(true);
        bool isImplicit @group(general);
        string index @group(general);
        string value @getter(info) @hint(name and value);
}

class cProperties
{
        @omitGetVerb(true);
        cProperty *contents[] @getter(get) @sizeGetter(numProperties) @hint(the property list);
}

class cComponent extends cDefaultList
{
        @omitGetVerb(true);
        bool isModule @group(general) @hint(whether this component is a module or a channel);
        cComponentType *componentType @group(general) @hint(the object that represents the type declaration for this component);
        cPar par[] @sizeGetter(params) @label(parameters) @group(parameters, gates) @hint(parameters);
        cProperties *properties @group(parameters, gates) @hint(properties);
}

class cChannel extends cComponent
{
        @omitGetVerb(true);
        cDisplayString displayString @group(general) @hint(controls appearance of this channel in the GUI);
}

class cBasicChannel extends cChannel
{
        @omitGetVerb(true);
        // Note: no need to spell out delay etc, they come from cComponent parameters
        bool isBusy @group(general) @hint(if channel is currently transmitting);
        simtime_t transmissionFinishes @group(general) @hint(if busy: when currently active transmissions will finish);
}

class cGate;

class cModule extends cComponent
{
        @omitGetVerb(true);
        int id @group(general) @hint(unique module ID -- IDs of deleted modules are not issued again);
        bool isSimple @group(general) @hint(whether this is a simple module);
        bool isVector @group(general) @hint(whether this module is part of a module vector);
        int index @group(general) @hint(if vector: index of this module in its module vector);
        int size @group(general) @hint(if vector: size of module vector this module belongs to);
        cDisplayString displayString @group(general) @hint(controls appearance of this module in the GUI);
        cGate *gate[] @sizeGetter(gateCount) @getter(gateByOrdinal) @group(parameters, gates) @label(gates) @hint(module gates);
}

class cSimpleModule extends cModule
{
        @omitGetVerb(true);
        bool isTerminated @group(general) @hint(whether this module has terminated);
        bool usesActivity @group(activity) @hint(whether this simple module was programmed using the "activity" or the "handleMessage" C++ method);
        unsigned int stackSize @group(activity) @hint(if module uses "activity": stack size allocated for this module -- zero means info not available);
        unsigned int stackUsage @group(activity) @hint(if module uses "activity": stack actually used by this module -- zero means info not available);
}

class cCompoundModule extends cModule
{
}

//----

class cComponentType extends cNoncopyableOwnedObject
{
}

class cModuleType extends cComponentType
{
        @omitGetVerb(true);
        bool isNetwork @group(general) @hint(whether this module type can be used as toplevel module);
}

class cChannelType extends cComponentType
{
}

class cRegistrationList extends cNamedObject
{
        @omitGetVerb(true);
        cObject *contents[] @getter(get) @sizeGetter(size) @hint(contained objects);
}

//----

class cGate extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        char type @group(general) @enum(cGate_Type) @hint(gate type);
        int id @group(general) @hint(gate id, unique within this module);
        bool isVector @group(general) @hint(whether this gate is part of a gate vector);
        int index @group(general) @hint(if vector: index of this gate within its gate vector);

        bool isBusy @group(general) @hint(only if the connection has a channel with nonzero datarate);
        simtime_t transmissionFinishes @group(general) @hint(only if the connection has a channel with nonzero datarate);

        cChannel *channel @group(connection) @hint(channel object associated with the connection);
        cGate *toGate @group(connection) @hint(the gate this one is connected to -- for output gates this points out of this module, and for input gates it points inside);
        cGate *fromGate @group(connection) @hint(the gate whose "toGate" is this gate -- note that "from" and "to" correspond to the direction of messages);
        cGate *sourceGate @group(connection) @hint(the last gate in the "fromGate->fromGate->fromGate..." chain);
        cGate *destinationGate @group(connection) @hint(the last gate in the "toGate->toGate->toGate..." chain);
        bool isConnectedOutside @group(connection) @hint(for an output gate this checks toGate, and fromGate for an input gate);
        bool isConnectedInside @group(connection) @hint(for an input gate this checks toGate, and fromGate for an output gate);
        bool isConnected @group(connection) @hint(whether the gate is fully connected -- both inside and outside for a compound module gate, and outside for a simple module gate);
        bool isPathOK @group(connection) @hint(the chain of connections is OK if it starts and ends at a simple module);
}

//----

class cDisplayString
{
        @omitGetVerb(true);
        string value @getter(str) @setter(parse) @editable @hint(the editable display string itself);
}

class cArray extends cOwnedObject
{
        @omitGetVerb(true);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable @hint(whether the array should own the objects inserted into it);
        cOwnedObject *contents[] @getter(get) @sizeGetter(size) @hint(objects contained in the array);
}

class cMessage extends cOwnedObject
{
        @omitGetVerb(true);
        short kind @group(message) @editable @hint(generic user-settable attribute, usually carries "type" or "role" of message);
        short priority @group(message) @editable @hint(controls delivery order of messages with equal timestamps);
        //FIXME the next two fields should be really int64, but opp_msgc doesn't support int64 yet
        long length @group(message) @editable @hint(simulated length of the message in bits, affects transmission time and probability of bit errors when sent through a channel);
        long byteLength @group(message) @editable @hint(length in bytes, i.e. length in bits divided by eight);
        bool hasBitError @group(message) @setter(setBitError) @editable @hint(signals that a bit error occurred when the message was sent through a channel with nonzero bit error rate);
        string displayString @group(message) @hint(controls appearance of the message in GUI);
        cArray parList @group(obsolete) @hint(use of "parList" should be replaced with message subclassing in the few models that still use it);

        cMessage *encapsulatedMsg @hint(used with protocol stacks: stores an encapsulated higher-layer packet);
        cObject *controlInfo @hint(used with protocol stacks: carries extra information when a packet is sent between protocol layers);

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

        //XXX various IDs
}

//----

class cStatistic extends cOwnedObject
{
        @omitGetVerb(true);
        long count @hint(number of observations collected so far);
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
        @omitGetVerb(true);
        bool isTransformed @group(histogram) @hint(if false, the object is still collecting initial observations to be used for laying out histogram cells);
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

class cTransientDetection extends cOwnedObject
{
}

class cAccuracyDetection extends cOwnedObject
{
}

class cTDExpandingWindows extends cTransientDetection
{
}

class cADByStddev extends cAccuracyDetection
{
}

//----

class cExpression extends cObject
{
        @omitGetVerb(true);
        string str;
}

class cFSM extends cOwnedObject
{
        @omitGetVerb(true);
        int state @hint(numeric code of the state the FSM is currently in);
        string stateName @hint(name of the current FSM state);
}

class cMathFunction extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        int numArgs @hint(number of "double" parameters this function takes);
}

class cNEDFunction extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        string signature @hint(argument types and return value);
        int minArgs @hint(number of mandatory arguments);
        int maxArgs @hint(maximum number of arguments);
}

class cOutVector extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        bool isEnabled @setter(setEnabled) @editable @hint(if disabled, values passed to this object to record are simply discarded);
        long valuesReceived @hint(number of values passed to this object to record);
        long valuesStored @hint(number of values actually stored on the disk);
}

class cPar extends cOwnedObject  //FIXME this is a lie! it's cObject!
{
        @omitGetVerb(true);
        string value @getter(str) @setter(parse) @editable;
        char type @enum(cPar_Type) @hint(parameter type);
        string unit @getter(unit) @hint(for long and double types only);
        bool isNumeric @group(flags) @hint(whether parameter type is numeric, i.e. double or long);
        bool isVolatile @group(flags) @hint(whether parameter was declared volatile in the NED file);
        bool isExpression@group(flags) @hint(whether parameter value is a constant or an expression);
        bool isShared @group(flags) @hint(whether storage could be optimized by several modules sharing the same parameter value object);
}

class cMsgPar extends cOwnedObject
{
        @omitGetVerb(true);
        string value @getter(str) @setter(parse) @editable @hint(parameter value);
        char type @enum(cPar_Type) @hint(parameter type);
        bool isNumeric @group(flags) @hint(whether parameter is of numeric type);
        bool isConstant @group(flags) @hint(whether the parameter has a constant value);
}

class cLinkedList extends cOwnedObject
{
        @omitGetVerb(true);
        int length @hint(number of items contained);
}

class cMessageHeap extends cOwnedObject
{
        @omitGetVerb(true);
        int length @hint(number of items contained);
        cMessage *messages[] @getter(peek) @sizeGetter(length) @hint(self-messages currently scheduled and messages currently in transit);
}

class cQueue extends cOwnedObject
{
        @omitGetVerb(true);
        bool takeOwnership @group(general) @setter(takeOwnership) @editable @hint(whether the queue should own the objects inserted into it);
        int length @hint(number of items in the queue);
        cOwnedObject *contents[] @getter(get) @sizeGetter(length) @hint(queue contents);
}

class noncobject cHasher; //FIXME should be nonpoymorphic or what???

class cHasher
{
       string hash @getter(str) @hint(the current hash value);
}

class cSimulation extends cNoncopyableOwnedObject
{
        @omitGetVerb(true);
        simtime_t simTime @hint(the current simulation time in seconds);
        long eventNumber @hint(the current event number -- counts from 0 up);
        cMessageHeap messageQueue @label(futureEvents) @hint(self-messages scheduled, and messages in transit);
        int lastModuleId @group(modules) @hint(the largest module ID issued so far);
        cModule *systemModule @group(modules) @hint(the top-level module);
        cModule *module[] @sizeGetter(lastModuleId) @group(modules) @label(modules) @hint(list of modules, indexed with module ID);
        cHasher *hasher @hint(used for fingerprint calculation);
}

class cScheduler extends cObject
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
        @omitGetVerb(true);
        double weight @hint(link weight -- affects graph algorithms such as shortest path);
        bool enabled @hint(link state -- affects graph algorithms such as shortest path);
}

class cTopology_LinkIn extends cTopology_Link
{
        @omitGetVerb(true);
        cTopology_Node *remoteNode @hint(the node at the remote end of this connection);
        cGate *remoteGate @hint(the gate at the remote end of this connection);
        cGate *localGate @hint(the gate at the local end of this connection);
}

class cTopology_LinkOut extends cTopology_Link
{
        @omitGetVerb(true);
        cTopology_Node *remoteNode @hint(the node at the remote end of this connection);
        cGate *remoteGate @hint(the gate at the remote end of this connection);
        cGate *localGate @hint(the gate at the local end of this connection);
}

class cTopology extends cOwnedObject
{
        @omitGetVerb(true);
        int nodes @group(general) @hint(number of nodes in this topology object);
        cTopology_Node *node[] @sizeGetter(nodes) @hint(list of nodes in this topology object);
}

class cXMLElement
{
        @omitGetVerb(true);
        string getTagName @label(tagName) @hint(XML element tag);
        string getNodeValue @label(nodeValue) @hint(contents of text node inside this XML element);
        string getSourceLocation @label(sourceLocation) @hint(location this XML element was parsed from);
        bool hasAttributes @hint(whether this XML element has attributes);
        bool hasChildren @hint(whether this XML element has child elements);
        string detailedInfo @label(contents);
}



