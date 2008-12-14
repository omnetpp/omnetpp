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

#ifdef _MSC_VER
#  pragma warning(disable:4996)  // deprecation (cLinkedList)
#endif

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
    string className @group(base) @hint(C++ class name);
    string fullName @group(base) @hint(If object is part of a module/gate vector: name with index);
    string fullPath @group(base) @hint(Hierarchical name, composed of full names of this object and its owners/parents);
    string info @getter(info) @group(base) @hint(Text provided by the object's "info" method);
    string detailedInfo @getter(detailedInfo) @group(base) @hint(Text provided by the object's "detailedInfo" method);
}

class cNamedObject extends cObject
{
    string name @group(base) @editable @hint(An editable string attribute of the object, also used for fullName and fullPath);
}

class cOwnedObject extends cNamedObject
{
    cObject *owner @group(base) @hint(Module or container this object belongs to);
}

class cNoncopyableOwnedObject extends cOwnedObject
{
}

class cDefaultList extends cNoncopyableOwnedObject
{
    cOwnedObject *contents[] @getter(defaultListGet) @sizeGetter(defaultListSize) @group(contents) @hint(List of objects directly held by this component);
}

class cDisplayString extends void
{
    string value @getter(str) @setter(parse) @editable @hint(The editable display string itself);
}

class cProperty extends cObject
{
    bool isImplicit @getter(isImplicit) @group(general);
    string index @group(general);
    string value @getter(info) @hint(Name and value);
}

class cProperties
{
    cProperty *contents[] @getter(get) @sizeGetter(getNumProperties) @hint(The property list);
}

class cPar extends cObject;
class cComponentType extends cOwnedObject;

class cComponent extends cDefaultList
{
    bool isModule @getter(isModule) @group(general) @hint(Whether this component is a module or a channel);
    cComponentType *componentType @group(general) @hint(The object that represents the type declaration for this component);
    cPar par[] @getter(par) @sizeGetter(getNumParams) @label(parameters) @group(parameters, gates) @hint(Parameters);
    cProperties *properties @group(parameters, gates) @hint(Properties);
}

class cChannel extends cComponent
{
    cDisplayString displayString @group(general) @hint(Controls appearance of this channel in the GUI);
}

class cIdealChannel extends cChannel
{
    // nothing special
}

class cDelayChannel extends cChannel
{
    // nothing special
}

class cDatarateChannel extends cChannel
{
    // Note: no need to spell out delay etc, they come from cComponent parameters
    bool isBusy @getter(isBusy) @group(general) @hint(If channel is currently transmitting);
    simtime_t transmissionFinishTime @group(general) @hint(If busy: when currently active transmissions will finish);
}

class cGate extends cObject;

class cModule extends cComponent
{
    int id @group(general) @hint(Unique module ID -- IDs of deleted modules are not issued again);
    bool isSimple @getter(isSimple) @group(general) @hint(Whether this is a simple module);
    bool isVector @getter(isVector) @group(general) @hint(Whether this module is part of a module vector);
    int index @group(general) @hint(If vector: index of this module in its module vector);
    int size @getter(size) @group(general) @hint(If vector: size of module vector this module belongs to);
    cDisplayString displayString @group(general) @hint(Controls appearance of this module in the GUI);
    cGate *gate[] @getter(getGateCount) @sizeGetter(gateCount) @getter(gateByOrdinal) @group(parameters, gates) @label(gates) @hint(Module gates);
}

class cSimpleModule extends cModule
{
    bool isTerminated @getter(isTerminated) @group(general) @hint(Whether this module has terminated);
    bool usesActivity @getter(usesActivity) @group(activity) @hint(Whether this simple module was programmed using the "activity" or the "handleMessage" C++ method);
    unsigned int stackSize @group(activity) @hint(If module uses "activity": stack size allocated for this module -- zero means info not available);
    unsigned int stackUsage @group(activity) @hint(If module uses "activity": stack actually used by this module -- zero means info not available);
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
    bool isNetwork @getter(isNetwork) @group(general) @hint(Whether this module type can be used as toplevel module);
}

class cChannelType extends cComponentType
{
}

class cRegistrationList extends cNamedObject
{
    cObject *contents[] @getter(get) @sizeGetter(size) @hint(Contained objects);
}

//----

class cGate extends cObject
{
    char type @group(general) @enum(cGate_Type) @hint(Gate type);
    int id @group(general) @hint(Gate id, unique within this module);
    bool isVector @getter(isVector) @group(general) @hint(Whether this gate is part of a gate vector);
    int index @group(general) @hint(If vector: index of this gate within its gate vector);
    bool deliverOnReceptionStart @group(general) @hint(Only for simple module input gates, and if message transmission duration is nonzero: whether messages arriving on this gate will be given to the module at the start or the end of the reception);

    bool isBusy @getter(isBusy) @group(general) @hint(Only if the connection has a channel with nonzero datarate);
    simtime_t transmissionFinishTime @group(general) @hint(Only if the connection has a channel with nonzero datarate);

    cChannel *channel @group(connection) @hint(Channel object associated with the connection);
    cGate *previousGate @group(connection) @hint(The gate this one is connected to -- for output gates this points out of this module, and for input gates it points inside);
    cGate *nextGate @group(connection) @hint(The gate whose "previousGate" is this gate -- note that "previous" and "next" correspond to the direction of messages);
    cGate *pathStartGate @group(connection) @hint(The last gate in the "previousGate->previousGate->previousGate..." chain);
    cGate *pathEndGate @group(connection) @hint(The last gate in the "nextGate->nextGate->nextGate..." chain);
    bool isConnectedOutside @getter(isConnectedOutside) @group(connection) @hint(For an output gate this checks nextGate, and previousGate for an input gate);
    bool isConnectedInside @getter(isConnectedInside) @group(connection) @hint(For an input gate this checks nextGate, and previousGate for an output gate);
    bool isConnected @getter(isConnected) @group(connection) @hint(Whether the gate is fully connected -- both inside and outside for a compound module gate, and outside for a simple module gate);
    bool isPathOK @getter(isPathOK) @group(connection) @hint(The chain of connections is OK if it starts and ends at a simple module);
}

//----

class cArray extends cOwnedObject
{
    bool takeOwnership @group(general) @editable @hint(Whether the array should own the objects inserted into it);
    cOwnedObject *contents[] @getter(get) @sizeGetter(size) @hint(Objects contained in the array);
}

class cMessage extends cOwnedObject
{
    long id @group(message) @hint(Automatically assigned unique identifier);
    long treeId @group(message) @hint(Identifier shared between the message and its copies);
    short kind @group(message) @editable @hint(Generic user-settable attribute, usually carries "type" or "role" or "category" of message);
    short schedulingPriority @group(message) @editable @hint(Controls order of messages with equal timestamps in FES);
    string displayString @group(message) @hint(Controls appearance of the message in GUI);
    cObject *controlInfo @group(message) @hint(Used with protocol stacks: carries extra information when a packet is sent between protocol layers);
    cArray parList @group(message) @hint(OBSOLETE: use of "parList" should be replaced with message subclassing in the few models that still use it);

    bool isScheduled @getter(isScheduled) @group(sending) @hint(Whether message is currently scheduled/in transit or not);
    bool isSelfMessage @getter(isSelfMessage) @group(sending) @hint(Whether message is a self-message, scheduled to be delivered back to the same module at a later time);
    simtime_t creationTime @group(sending) @hint(Simulation time the message was created);
    simtime_t sendingTime @group(sending) @hint(Simulation time of the current or last sending/scheduling of the message);
    simtime_t arrivalTime @group(sending) @hint(Simulation time the message will be/was delivered to the destination module);
    simtime_t timestamp @group(sending) @editable @hint(User-settable timestamp for generic use);

    int senderModuleId @group(sending) @hint(ID of the module performing the current or last send/schedule operation);
    int senderGateId @group(sending) @hint(Module-local ID of the gate on which the message was sent out -- -1==none for self-messages and after "sendDirect");
    cModule *senderModule @group(sending) @hint(ID of the module performing the current or last send/schedule operation);
    cGate *senderGate @group(sending) @hint(The gate on which the message was sent out -- NULL for self-messages and after "sendDirect");

    int arrivalModuleId @group(sending) @hint(ID of the arrival module of the current or last sending/scheduling);
    int arrivalGateId @group(sending) @hint(Module-local ID of arrival gate for current or last sending -- -1==none for self-messages);
    cModule *arrivalModule @group(sending) @hint(Arrival module for current or last sending/scheduling);
    cGate *arrivalGate @group(sending) @hint(Arrival gate for current or last sending -- NULL for self-messages);

    //XXX various IDs
}

class cPacket extends cMessage
{
    int64 bitLength @group(message) @editable @hint(Simulated length of the message in bits, affects transmission time and probability of bit errors when sent through a channel);
    int64 byteLength @group(message) @editable @hint(Length in bytes, i.e. length in bits divided by eight);
    bool hasBitError @getter(hasBitError) @group(message) @setter(setBitError) @editable @hint(Indicates that a bit error occurred when the message was sent through a channel with nonzero bit error rate);
    cPacket *encapsulatedMsg @hint(Used with protocol stacks: stores an encapsulated higher-layer packet);
    bool isReceptionStart @getter(isReceptionStart) @group(sending) @hint(When duration is nonzero: whether this message represents the start or the end of the frame reception);
    simtime_t duration @group(sending) @hint(Transmission time over a channel with datarate);
}

//----

class cStatistic extends cOwnedObject
{
    long count @hint(Number of observations collected so far);
    double weights @hint(Sum of weights, which equals the number of observations in the unweighted case);
    double sum @hint(Sum of observations);
    double sqrSum @hint(Sum of squares of the observations);
    double min @hint(Minimum of the observations);
    double max @hint(Maximum of the observations);
    double mean @hint(Mean of the observations);
    double stddev @hint(Standard deviation of the observations);
    double variance @hint(Variance of the observations);
}

class cStdDev extends cStatistic
{
}

class cWeightedStdDev extends cStdDev
{
}

class cDensityEstBase extends cStdDev
{
    bool isTransformed @getter(isTransformed) @group(histogram) @hint(If false, the object is still collecting initial observations to be used for laying out histogram cells);
    unsigned long underflowCell @group(histogram) @hint(Number of observations below the first histogram cell);
    unsigned long overflowCell @group(histogram) @hint(Number of observations above the last histogram cell);
    int numCells @group(histogram) @hint(Number of cells in the histogram);
    cDensityEstBase_Cell cellInfo[] @sizeGetter(getNumCells) @group(histogram) @hint(Histogram cell bounds, observation count in the cell, and density estimate in the cell);
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
    string str @getter(str);
}

class cFSM extends cOwnedObject
{
    int state @hint(Numeric code of the state the FSM is currently in);
    string stateName @hint(Name of the current FSM state);
}

class cMathFunction extends cNoncopyableOwnedObject
{
    int numArgs @hint(Number of "double" parameters this function takes);
}

class cNEDFunction extends cNoncopyableOwnedObject
{
    string signature @hint(Argument types and return value);
    int minArgs @hint(Number of mandatory arguments);
    int maxArgs @hint(Maximum number of arguments);
}

class cOutVector extends cNoncopyableOwnedObject
{
    bool isEnabled @getter(isEnabled) @setter(setEnabled) @editable @hint(If disabled, values passed to this object to record are simply discarded);
    long valuesReceived @hint(Number of values passed to this object to record);
    long valuesStored @hint(Number of values actually stored on the disk);
}

class cPar extends cObject
{
    string value @getter(str) @setter(parse) @editable;
    char type @enum(cPar_Type) @hint(Parameter type);
    string unit @hint(For long and double types only);
    bool isNumeric @getter(isNumeric) @group(flags) @hint(Whether parameter type is numeric, i.e. double or long);
    bool isVolatile @getter(isVolatile) @group(flags) @hint(Whether parameter was declared volatile in the NED file);
    bool isExpression @getter(isExpression)@group(flags) @hint(Whether parameter value is a constant or an expression);
    bool isShared @getter(isShared) @group(flags) @hint(Whether storage could be optimized by several modules sharing the same parameter value object);
}

class cMsgPar extends cOwnedObject
{
    string value @getter(str) @setter(parse) @editable @hint(Parameter value);
    char type @enum(cPar_Type) @hint(Parameter type);
    bool isNumeric @getter(isNumeric) @group(flags) @hint(Whether parameter is of numeric type);
    bool isConstant @getter(isConstant) @group(flags) @hint(Whether the parameter has a constant value);
}

class cLinkedList extends cOwnedObject
{
    int length @hint(Number of items contained);
}

class cMessageHeap extends cOwnedObject
{
    int length @hint(Number of items contained);
    cMessage *messages[] @getter(peek) @sizeGetter(getLength) @hint(Self-messages currently scheduled and messages currently in transit);
}

class cQueue extends cOwnedObject
{
    bool takeOwnership @group(general) @editable @hint(Whether the queue should own the objects inserted into it);
    int length @hint(Number of items in the queue);
    cOwnedObject *contents[] @getter(get) @sizeGetter(getLength) @hint(Queue contents);
}

class cPacketQueue extends cQueue
{
    int64 bitLength @hint(Total size of messages in the queue);
    int64 byteLength @hint(Total size of messages in the queue, in bytes);
}

class cHasher extends void
{
   string hash @getter(str) @hint(The current hash value);
}

class cSimulation extends cNoncopyableOwnedObject
{
    simtime_t simTime @hint(The current simulation time in seconds);
    int64 eventNumber @hint(The current event number -- counts from 0 up);
    cMessageHeap messageQueue @label(futureEvents) @hint(Self-messages scheduled, and messages in transit);
    int lastModuleId @group(modules) @hint(The largest module ID issued so far);
    cModule *systemModule @group(modules) @hint(The top-level module);
    cModule *module[] @sizeGetter(getLastModuleId) @group(modules) @label(modules) @hint(List of modules, indexed with module ID);
    cHasher *hasher @hint(Used for fingerprint calculation);
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

class cTopology_Link extends void;
class cTopology_LinkIn extends cTopology_Link;
class cTopology_LinkOut extends cTopology_Link;

class cTopology_Node extends void
{
    int moduleId @hint(ID of the module this node corresponds to);
    cModule *module @hint(The module this node corresponds to);
    double weight @hint(Node weight -- affects graph algorithms such as shortest path);
    bool enabled @getter(isEnabled) @hint(Node state -- affects graph algorithms such as shortest path);

    cTopology_LinkIn *inLink[] @getter(getLinkIn) @sizeGetter(getNumInLinks) @hint(Incoming links of this graph node);
    cTopology_LinkIn *outLink[] @getter(getLinkOut) @sizeGetter(getNumOutLinks) @hint(Outgoing links of this graph node);

    double distanceToTarget @group(shortest path) @hint(After running a shortest path algorithm: distance to the target node);
    cTopology_LinkOut *path[] @sizeGetter(getNumPaths) @group(shortest path) @hint(The next link of the shortest path or paths towards the target node);
}

class cTopology_Link extends void
{
    double weight @hint(Link weight -- affects graph algorithms such as shortest path);
    bool enabled @getter(isEnabled) @hint(Link state -- affects graph algorithms such as shortest path);
}

class cTopology_LinkIn extends cTopology_Link
{
    cTopology_Node *remoteNode @hint(The node at the remote end of this connection);
    cGate *remoteGate @hint(The gate at the remote end of this connection);
    cGate *localGate @hint(The gate at the local end of this connection);
}

class cTopology_LinkOut extends cTopology_Link
{
    cTopology_Node *remoteNode @hint(The node at the remote end of this connection);
    cGate *remoteGate @hint(The gate at the remote end of this connection);
    cGate *localGate @hint(The gate at the local end of this connection);
}

class cTopology extends cOwnedObject
{
    int numNodes @group(general) @hint(Number of nodes in this topology object);
    cTopology_Node *node[] @sizeGetter(getNumNodes) @hint(List of nodes in this topology object);
}

class cXMLElement
{
    string tagName @hint(XML element tag);
    string nodeValue @hint(Contents of text node inside this XML element);
    string sourceLocation @hint(Location this XML element was parsed from);
    bool hasAttributes @getter(hasAttributes) @hint(Whether this XML element has attributes);
    bool hasChildren @getter(hasChildren) @hint(Whether this XML element has child elements);
    string detailedInfo @getter(detailedInfo) @label(contents);
}



