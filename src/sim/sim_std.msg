//==============================================================
//   SIM_STD.MSG  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==============================================================

//--------------------------------------------------------------
// Copyright (C) 1992-2017 Andras Varga
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------

//
// Property descriptors for the simkernel classes.
// Usage: opp_msgc -Xnc -Xns sim_std.msg
//

enum omnetpp::cPar::Type;
enum omnetpp::cGate::Type;
enum omnetpp::cFigure::LineStyle;
enum omnetpp::cFigure::CapStyle;
enum omnetpp::cFigure::JoinStyle;
enum omnetpp::cFigure::FillRule;
enum omnetpp::cFigure::Arrowhead;
enum omnetpp::cFigure::Anchor;
enum omnetpp::cFigure::Interpolation;

cplusplus
{{

#ifdef _MSC_VER
#  pragma warning(disable:4996)  // deprecation (cLinkedList)
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "resultfilters.h"
#include "resultrecorders.h"

namespace omnetpp {

//
// Register enums from the sim. kernel
//
#define cF cFigure
Register_Enum(cPar::Type,         (cPar::BOOL, cPar::DOUBLE, cPar::LONG, cPar::STRING, cPar::XML));
Register_Enum(cGate::Type,        (cGate::INPUT, cGate::OUTPUT, cGate::INOUT));
Register_Enum(cFigure::LineStyle, (cF::LINE_SOLID, cF::LINE_DOTTED, cF::LINE_DASHED));
Register_Enum(cFigure::CapStyle,  (cF::CAP_BUTT, cF::CAP_SQUARE, cF::CAP_ROUND));
Register_Enum(cFigure::JoinStyle, (cF::JOIN_BEVEL, cF::JOIN_MITER, cF::JOIN_ROUND));
Register_Enum(cFigure::FillRule,  (cF::FILL_EVENODD, cF::FILL_NONZERO));
Register_Enum(cFigure::Arrowhead, (cF::ARROW_NONE, cF::ARROW_SIMPLE, cF::ARROW_TRIANGLE, cF::ARROW_BARBED));
Register_Enum(cFigure::Anchor,    (cF::ANCHOR_CENTER, cF::ANCHOR_N, cF::ANCHOR_E, cF::ANCHOR_S, cF::ANCHOR_W, cF::ANCHOR_NW, cF::ANCHOR_NE, cF::ANCHOR_SE, cF::ANCHOR_SW, cF::ANCHOR_BASELINE_START, cF::ANCHOR_BASELINE_MIDDLE, cF::ANCHOR_BASELINE_END));
Register_Enum(cFigure::Interpolation, (cF::INTERPOLATION_NONE, cF::INTERPOLATION_FAST, cF::INTERPOLATION_BEST));
#undef cF

//
// Define operator<< for some classes
//
inline std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.str() << "\"";
}

inline std::ostream& operator<<(std::ostream& os, const cDensityEstBase::Cell& cell) {
    return os << "[" << cell.lower << ", " << cell.upper << ")  ==>  " << cell.value << " (" << cell.relativeFreq << ")";
}

typedef cFigure::Point Point;
#define Rectangle cFigure::Rectangle  // typedef would conflict with MinGW's Rectangle in <wingdi.h>
typedef cFigure::Transform Transform;
typedef cFigure::Color Color;
typedef cFigure::Font Font;
typedef cFigure::Pixmap Pixmap;

using namespace canvas_stream_ops;

//
// This function is to be referenced from sim. kernel classes, to force the
// linker to include this file in executables
//
void std_sim_descriptor_dummy() {}

}  // namespace omnetpp

}}

namespace omnetpp;

struct cDensityEstBase::Cell;

struct Point;
struct Rectangle;
struct Transform;
struct Color;
struct Font;
struct Pixmap;

class noncobject StringVector;

class cObject
{
    @icon(cogwheel);
    string className @group(base) @hint("C++ class name") @eventlog(skip);
    string fullName @group(base) @hint("If object is part of a module/gate vector: name with index");
    string fullPath @group(base) @hint("Hierarchical name, composed of full names of this object and its owners/parents") @eventlog(skip);
    string info @getter(str) @group(base) @hint("Text provided by the object's \"str()\" method") @eventlog(skip);
}

class cNamedObject extends cObject
{
    string name @group(base) @editable @hint("An editable string attribute of the object, also used for fullName and fullPath");
}

class cOwnedObject extends cNamedObject
{
    cObject *owner @group(base) @hint("Module or container this object belongs to") @eventlog(skip);
}

class cNoncopyableOwnedObject extends cOwnedObject
{
}

class cDefaultList extends cNoncopyableOwnedObject
{
    cOwnedObject *defaultList[] @getter(defaultListGet) @sizeGetter(defaultListSize) @group(owned objects) @hint("List of objects directly held by this component");
}

class cDisplayString extends void
{
    string value @getter(str) @setter(parse) @editable @hint("The editable display string itself");
}

class cProperty extends cObject
{
    bool isImplicit @getter(isImplicit) @group(fields);
    string index @group(fields);
    string value @getter(str) @hint("Name and value");
}

class cIListener extends void;

class cComponent::SignalListenerList extends void
{
    string signalName @hint("Name of signal listened on");
    cIListener *listeners[] @getter(getListener) @sizeGetter(countListeners); //TODO @tostring(->str()) for cResultListeners
}

class cPar extends cObject;
class cComponentType extends cOwnedObject;
class cCanvas extends cOwnedObject;
class cOsgCanvas extends cOwnedObject;
class cResultRecorder extends cIListener;

class cComponent extends cDefaultList
{
    bool isModule @getter(isModule) @group(fields) @hint("Whether this component is a module or a channel");
    cComponentType *componentType @group(fields) @hint("The object that represents the type declaration for this component");
    cPar par[] @getter(par) @sizeGetter(getNumParams) @label(parameters) @group(parameters, gates) @hint("Parameters");
    cProperty *properties_[] @label(properties) @getter(->getProperties()->get(i)) @sizeGetter(->getProperties()->getNumProperties()) @group(parameters, gates) @hint("Properties");
    cComponent::SignalListenerList listenedSignals[] @getter(getListenerList) @sizeGetter(getSignalTableSize) @tostring(.str()) @group(parameters, gates) @hint("Signal listener lists");
    cResultRecorder *results[] @getter(->getResultRecorders()[i]) @sizeGetter(->getResultRecorders().size()) @tostring(->str()) @group(parameters, gates) @hint("Statistical results collected in this component");
}

class cChannel extends cComponent
{
    @icon(channel);
    cDisplayString displayString @group(fields) @hint("Controls appearance of this channel in the GUI") @editable @setter(setDisplayString) @fromstring($) @tostring(.str()) @getter(getDisplayString);
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
    bool isBusy @getter(isBusy) @group(fields) @hint("If channel is currently transmitting");
    simtime_t transmissionFinishTime @group(fields) @hint("If busy: when currently active transmissions will finish");
}

class cGate extends cObject;

class cModule extends cComponent
{
    @icon(compound);
    int id @group(fields) @hint("Unique module ID -- IDs of deleted modules are not issued again");
    bool isSimple @getter(isSimple) @group(fields) @hint("Whether this is a simple module");
    bool isVector @getter(isVector) @group(fields) @hint("Whether this module is part of a module vector");
    int index @group(fields) @hint("If vector: index of this module in its module vector");
    int size @getter(size) @group(fields) @hint("If vector: size of module vector this module belongs to");
    bool builtinAnimationsAllowed @editable @group(fields) @hint("Whether built-in animations are enabled on this module's graphical inspector");
    cDisplayString displayString @group(fields) @hint("Controls appearance of this module in the GUI") @editable @setter(setDisplayString) @fromstring($) @tostring(.str()) @getter(getDisplayString);
    cGate *gate[] @sizeGetter(gateCount) @getter(gateByOrdinal) @group(parameters, gates) @label(gates) @hint("Module gates");
    cCanvas *canvas @getter(getCanvasIfExists) @group(fields) @hint("Built-in canvas instance");
    cOsgCanvas *osgCanvas @getter(getOsgCanvasIfExists) @group(fields) @hint("Built-in 3D (OpenSceneGraph) canvas instance");
}

class cSimpleModule extends cModule
{
    @icon(simple);
    bool isTerminated @getter(isTerminated) @group(fields) @hint("Whether this module has terminated");
    bool usesActivity @getter(usesActivity) @group(activity) @hint("Whether this simple module was programmed using the \"activity\" or the \"handleMessage\" C++ method");
    unsigned int stackSize @group(activity) @hint("If module uses \"activity\": stack size allocated for this module -- zero means info not available");
    unsigned int stackUsage @group(activity) @hint("If module uses \"activity\": stack actually used by this module -- zero means info not available");
}

//TODO: use @icon(placeholder) for cPlaceholderModule

//----

class cComponentType extends cNoncopyableOwnedObject
{
    string nedSource @getter(getNedSource) @hint("NED source of component if available");
}

class cModuleType extends cComponentType
{
    bool isNetwork @getter(isNetwork) @group(fields) @hint("Whether this module type can be used as toplevel module");
}

class cChannelType extends cComponentType
{
}

class cRegistrationList extends cNamedObject
{
    cObject *contents[] @getter(get) @sizeGetter(size) @hint("Contained objects");
}

//----

class cGate extends cObject
{
    @icon(gate);
    char type @group(fields) @enum(omnetpp::cGate::Type) @hint("Gate type");
    int id @group(fields) @hint("Gate id, unique within this module");
    bool isVector @getter(isVector) @group(fields) @hint("Whether this gate is part of a gate vector");
    int index @group(fields) @hint("If vector: index of this gate within its gate vector");
    bool deliverOnReceptionStart @group(fields) @hint("Only for simple module input gates, and if message transmission duration is nonzero: whether messages arriving on this gate will be given to the module at the start or the end of the reception");

    cChannel *channel @group(connection) @hint("Channel object associated with the connection");
    cGate *previousGate @group(connection) @hint("The gate this one is connected to -- for output gates this points out of this module, and for input gates it points inside");
    cGate *nextGate @group(connection) @hint("The gate whose \"previousGate\" is this gate -- note that \"previous\" and \"next\" correspond to the direction of messages");
    cGate *pathStartGate @group(connection) @hint("The last gate in the \"previousGate->previousGate->previousGate...\" chain");
    cGate *pathEndGate @group(connection) @hint("The last gate in the \"nextGate->nextGate->nextGate...\" chain");
    bool isConnectedOutside @getter(isConnectedOutside) @group(connection) @hint("For an output gate this checks nextGate, and previousGate for an input gate");
    bool isConnectedInside @getter(isConnectedInside) @group(connection) @hint("For an input gate this checks nextGate, and previousGate for an output gate");
    bool isConnected @getter(isConnected) @group(connection) @hint("Whether the gate is fully connected -- both inside and outside for a compound module gate, and outside for a simple module gate");
    bool isPathOK @getter(isPathOK) @group(connection) @hint("The chain of connections is OK if it starts and ends at a simple module");
}

//----

class cArray extends cOwnedObject
{
    @icon(container);
    bool takeOwnership @group(fields) @editable @hint("Whether the array should own the objects inserted into it");
    cOwnedObject *contents[] @getter(get) @sizeGetter(size) @hint("Objects contained in the array");
}

class cEvent extends cOwnedObject
{
    short schedulingPriority @group(event) @editable @hint("Controls order of messages with equal timestamps in FES");
    cObject *targetObject @group(event) @hint("The object this message will be delivered to");
    bool isScheduled @getter(isScheduled) @group(sending) @hint("Whether event or message is currently scheduled (or in transit)");
    simtime_t arrivalTime @group(sending) @hint("Simulation time the event or message will be (or was) delivered");
}

class cMessage extends cEvent
{
    @icon("message");
    long id @group("message") @hint("Automatically assigned unique identifier");
    long treeId @group("message") @hint("Identifier shared between the message and its copies");
    short kind @group("message") @editable @hint("Generic user-settable attribute, usually carries \"type\" or \"role\" or \"category\" of message");
    string displayString @group("message") @hint("Controls appearance of the message in GUI");
    cObject *tags[] @getter(getTag) @sizeGetter(getNumTags) @group("message") @hint("Pieces of metadata attached to the message");
    cObject *controlInfo @hint("Used with protocol stacks: carries extra information when a packet is sent between protocol layers");
    cArray *parList @getter(getParListPtr) @group("message") @hint("OBSOLETE: use of \"parList\" should be replaced with message subclassing in the few models that still use it");

    bool isSelfMessage @getter(isSelfMessage) @group(sending) @hint("Whether message is a self-message, scheduled to be delivered back to the same module at a later time");
    simtime_t creationTime @group(sending) @hint("Simulation time the message was created");
    simtime_t sendingTime @group(sending) @hint("Simulation time of the current or last sending/scheduling of the message");
    simtime_t timestamp @group(sending) @editable @hint("User-settable timestamp for generic use");

    int senderModuleId @group(sending) @hint("ID of the module performing the current or last send/schedule operation");
    int senderGateId @group(sending) @hint("Module-local ID of the gate on which the message was sent out -- -1==none for self-messages and after \"sendDirect\"");
    cModule *senderModule @group(sending) @hint("ID of the module performing the current or last send/schedule operation") @eventlog(fullPath);
    cGate *senderGate @group(sending) @hint("The gate on which the message was sent out -- nullptr for self-messages and after \"sendDirect\"") @eventlog(fullName);

    int arrivalModuleId @group(sending) @hint("ID of the arrival module of the current or last sending/scheduling");
    int arrivalGateId @group(sending) @hint("Module-local ID of arrival gate for current or last sending -- -1==none for self-messages");
    cModule *arrivalModule @group(sending) @hint("Arrival module for current or last sending/scheduling") @eventlog(fullPath);
    cGate *arrivalGate @group(sending) @hint("Arrival gate for current or last sending -- nullptr for self-messages") @eventlog(fullName);

    //XXX various IDs
}

class cPacket extends cMessage
{
    @icon("packet");
    int64_t bitLength @group("packet") @editable @hint("Simulated length of the message in bits, affects transmission time and probability of bit errors when sent through a channel");
    int64_t byteLength @group("packet") @editable @hint("Length in bytes, i.e. length in bits divided by eight");
    bool hasBitError @getter(hasBitError) @group("packet") @setter(setBitError) @editable @hint("Indicates that a bit error occurred when the message was sent through a channel with nonzero bit error rate");
    cPacket *encapsulatedPacket @hint("Used with protocol stacks: stores an encapsulated higher-layer packet");
    bool isReceptionStart @getter(isReceptionStart) @group(sending) @hint("When duration is nonzero: whether this message represents the start or the end of the frame reception");
    simtime_t duration @group(sending) @hint("Transmission time over a channel with datarate");
}

//----

class cRNG extends cObject
{
}

class cRandom extends cOwnedObject
{
    @icon(cogwheel); //TODO
    cRNG *rng @getter(getRNG) @setter(setRNG);
}

class cUniform extends cRandom
{
    double a;
    double b;
}

class cExponential extends cRandom
{
    double mean;
}

class cNormal extends cRandom
{
    double mean;
    double stddev;
}

class cTruncNormal extends cRandom
{
    double mean;
    double stddev;
}

class cGamma extends cRandom
{
    double alpha;
    double theta;
}

class cBeta extends cRandom
{
    double alpha1;
    double alpha2;
}

class cErlang extends cRandom
{
    unsigned int k;
    double mean;
}

class cChiSquare extends cRandom
{
    unsigned int k;
}

class cStudentT extends cRandom
{
    unsigned int i;
}

class cCauchy extends cRandom
{
    double a;
    double b;
}

class cTriang extends cRandom
{
    double a;
    double b;
    double c;
}

class cWeibull extends cRandom
{
    double a;
    double b;
}

class cParetoShifted extends cRandom
{
    double a;
    double b;
    double c;
}

class cIntUniform extends cRandom
{
    int a;
    int b;
}

class cBernoulli extends cRandom
{
    double p;
}

class cBinomial extends cRandom
{
    int n;
    double p;
}

class cGeometric extends cRandom
{
    double p;
}

class cNegBinomial extends cRandom
{
    int n;
    double p;
}

class cPoisson extends cRandom
{
    double lambda;
}

//----

class cStatistic extends cOwnedObject
{
    @icon(statistic);
    long count @hint("Number of observations collected, regardless of their weights");
    double sumWeights @getter(getWeights) @hint("Sum of weights, which equals the number of observations in the unweighted case");
    double min @hint("Minimum of the observations");
    double max @hint("Maximum of the observations");
    double mean @hint("Mean of the observations");
    double stddev @hint("Standard deviation of the observations");
    double variance @hint("Variance of the observations");
}

class cStdDev extends cStatistic
{
}

class cWeightedStdDev extends cStdDev
{
}

class cDensityEstBase extends cStdDev
{
    bool isTransformed @getter(isTransformed) @group(histogram) @hint("If false, the object is still collecting initial observations to be used for laying out histogram cells");
    unsigned long underflowCell @group(histogram) @hint("Number of observations below the first histogram cell");
    unsigned long overflowCell @group(histogram) @hint("Number of observations above the last histogram cell");
    int numCells @group(histogram) @hint("Number of cells in the histogram");
    cDensityEstBase::Cell cellInfo[] @getter(internalGetCellInfo) @sizeGetter(getNumCells) @group(histogram) @hint("Histogram cell bounds, observation count in the cell, and density estimate in the cell");
}

class cHistogramBase extends cDensityEstBase
{
}

class cHistogram extends cHistogramBase
{
}

class cVarHistogram extends cHistogramBase
{
}

class cLongHistogram extends cHistogram
{
}

class cDoubleHistogram extends cHistogram
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
    int state @hint("Numeric code of the state the FSM is currently in");
    string stateName @hint("Name of the current FSM state");
}

class cNEDMathFunction extends cNoncopyableOwnedObject
{
    string category @hint("Function category");
    string description;
    int numArgs @hint("Number of \"double\" parameters this function takes");
}

class cNEDFunction extends cNoncopyableOwnedObject
{
    string signature @hint("Argument types and return value");
    string category @hint("Function category");
    string description;
    char returnType;
    char argType[] @sizeGetter(getMaxArgs);
    int minArgs @hint("Number of mandatory arguments");
    int maxArgs @hint("Maximum number of arguments");
}

class cOutVector extends cNoncopyableOwnedObject
{
    @icon(outvector);
    bool isEnabled @getter(isEnabled) @setter(setEnabled) @editable @hint("If disabled, values passed to this object to record are simply discarded");
    long valuesReceived @hint("Number of values passed to this object to record");
    long valuesStored @hint("Number of values actually stored on the disk");
}

class cPar extends cObject
{
    @icon(param);
    string value @getter(str) @setter(parse) @editable;
    char type @enum(omnetpp::cPar::Type) @hint("Parameter type");
    string unit @hint("For long and double types only");
    bool isNumeric @getter(isNumeric) @group(flags) @hint("Whether parameter type is numeric, i.e. double or long");
    bool isVolatile @getter(isVolatile) @group(flags) @hint("Whether parameter was declared volatile in the NED file");
    bool isExpression @getter(isExpression)@group(flags) @hint("Whether parameter value is a constant or an expression");
    bool isShared @getter(isShared) @group(flags) @hint("Whether storage could be optimized by several modules sharing the same parameter value object");
}

class cMsgPar extends cOwnedObject
{
    string value @getter(str) @setter(parse) @editable @hint("Parameter value");
    char type @enum(omnetpp::cPar::Type) @hint("Parameter type");
    bool isNumeric @getter(isNumeric) @group(flags) @hint("Whether parameter is of numeric type");
    bool isConstant @getter(isConstant) @group(flags) @hint("Whether the parameter has a constant value");
}

class cFutureEventSet extends cOwnedObject
{
    @icon(container);
    int length @hint("Number of items contained");
    cEvent *events[] @getter(get) @sizeGetter(getLength) @hint("Events and self-messages and currently scheduled, and messages currently in transit");
}

class cEventHeap extends cFutureEventSet
{
}

class cQueue extends cOwnedObject
{
    @icon(queue);
    bool takeOwnership @group(fields) @editable @hint("Whether the queue should own the objects inserted into it");
    int length @hint("Number of items in the queue");
    cOwnedObject *contents[] @getter(get) @sizeGetter(getLength) @hint("Queue contents");
}

class cPacketQueue extends cQueue
{
    int64_t bitLength @hint("Total size of messages in the queue");
    int64_t byteLength @hint("Total size of messages in the queue, in bytes");
}

class cHasher extends void
{
   string hash @getter(str) @hint("The current hash value");
}

class cFingerprintCalculator extends cObject
{
}

class cSimulation extends cNoncopyableOwnedObject
{
    @icon(container);
    simtime_t simTime @hint("The current simulation time in seconds");
    int64_t eventNumber @hint("The current event number -- counts from 0 up");
    cFutureEventSet *fes @getter(getFES) @label(futureEvents) @hint("Self-messages scheduled, and messages in transit");
    int lastComponentId @group(components) @hint("The largest component ID issued so far");
    cModule *systemModule @group(components) @hint("The top-level module");
    cComponent *component[] @sizeGetter(getLastComponentId) @group(components) @label(components) @hint("List of components indexed by component ID");
    cFingerprintCalculator *fingerprintCalculator @hint("Used for computing simulation fingerprint");
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
    int moduleId @hint("ID of the module this node corresponds to");
    cModule *module @hint("The module this node corresponds to");
    double weight @hint("Node weight -- affects graph algorithms such as shortest path");
    bool enabled @getter(isEnabled) @hint("Node state -- affects graph algorithms such as shortest path");

    cTopology_LinkIn *inLink[] @getter(getLinkIn) @sizeGetter(getNumInLinks) @hint("Incoming links of this graph node");
    cTopology_LinkOut *outLink[] @getter(getLinkOut) @sizeGetter(getNumOutLinks) @hint("Outgoing links of this graph node");

    double distanceToTarget @group(shortest path) @hint("After running a shortest path algorithm: distance to the target node");
    cTopology_LinkOut *path[] @sizeGetter(getNumPaths) @group(shortest path) @hint("The next link of the shortest path or paths towards the target node");
}

class cTopology_Link extends void
{
    double weight @hint("Link weight -- affects graph algorithms such as shortest path");
    bool enabled @getter(isEnabled) @hint("Link state -- affects graph algorithms such as shortest path");
}

class cTopology_LinkIn extends cTopology_Link
{
    cTopology_Node *remoteNode @hint("The node at the remote end of this connection");
    cGate *remoteGate @hint("The gate at the remote end of this connection");
    cGate *localGate @hint("The gate at the local end of this connection");
}

class cTopology_LinkOut extends cTopology_Link
{
    cTopology_Node *remoteNode @hint("The node at the remote end of this connection");
    cGate *remoteGate @hint("The gate at the remote end of this connection");
    cGate *localGate @hint("The gate at the local end of this connection");
}

class cTopology extends cOwnedObject
{
    int numNodes @group(fields) @hint("Number of nodes in this topology object");
    cTopology_Node *node[] @sizeGetter(getNumNodes) @hint("List of nodes in this topology object");
}

class cXMLElement extends cObject
{
    string tagName @hint("XML element tag");
    string nodeValue @hint("Contents of text node inside this XML element");
    string sourceLocation @hint("Location this XML element was parsed from");
    string attributes[] @getter(getAttr) @sizeGetter(getNumAttrs) @hint("Attributes");
    cXMLElement *children[] @getter(getChild) @sizeGetter(getNumChildren) @hint("Child elements");
    string xml @getter(getXML) @hint("Element tree as XML fragment");
}

class cFigure extends cOwnedObject
{
    int id @group(figure);
    bool visible @group(figure) @getter(isVisible) @editable;
    double zIndex @group(figure) @editable;
    string tooltip @group(figure) @editable;
    cObject *associatedObject @group(figure);
    string tags @group(figure) @editable;
    Transform transform @group(figure) @editable @tostring(.str()) @fromstring(cFigure::parseTransform($));
    cFigure *children[] @group(figure) @getter(getFigure) @sizeGetter(getNumFigures);
}

class cGroupFigure extends cFigure
{
}

class cPanelFigure extends cFigure
{
    Point position @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
    Point anchorPoint @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
}

class cAbstractLineFigure extends cFigure
{
    Color lineColor @group(line) @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
    int lineStyle @group(line) @editable @enum(omnetpp::cFigure::LineStyle);
    double lineWidth @group(line) @editable;
    double lineOpacity @group(line) @editable;
    int capStyle @group(line) @editable @enum(omnetpp::cFigure::CapStyle);
    int startArrowhead @group(line) @editable @enum(omnetpp::cFigure::Arrowhead);
    int endArrowhead @group(line) @editable @enum(omnetpp::cFigure::Arrowhead);
    bool zoomLineWidth @group(line) @editable;
}

class cLineFigure extends cAbstractLineFigure
{
    Point start @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
    Point end @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
}

class cArcFigure extends cAbstractLineFigure
{
    Rectangle bounds @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parseRectangle($));
    double startAngle @group(geometry) @editable;
    double endAngle @group(geometry) @editable;
}

class cPolylineFigure extends cAbstractLineFigure
{
    Point points[] @group(geometry) @editable @getter(getPoint) @setter(setPoint) @sizeGetter(getNumPoints) @tostring(.str()) @fromstring(cFigure::parsePoint($));
    bool smooth @group(geometry) @editable;
    int joinStyle @group(line) @editable @enum(omnetpp::cFigure::JoinStyle);
}

class cAbstractShapeFigure extends cFigure
{
    bool outlined @group(outline) @getter(isOutlined) @editable;
    bool filled @group(fill) @getter(isFilled) @editable;
    Color lineColor @group(outline) @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
    Color fillColor @group(fill) @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
    int lineStyle @group(outline) @editable @enum(cFigure::LineStyle);
    double lineWidth @group(outline) @editable;
    double lineOpacity @group(outline) @editable;
    double fillOpacity @group(fill) @editable;
    bool zoomLineWidth @group(outline) @editable;
}

class cRectangleFigure extends cAbstractShapeFigure
{
    Rectangle bounds @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parseRectangle($));
    double cornerRx @group(geometry) @editable;
    double cornerRy @group(geometry) @editable;
}

class cOvalFigure extends cAbstractShapeFigure
{
    Rectangle bounds @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parseRectangle($));
}

class cRingFigure extends cAbstractShapeFigure
{
    Rectangle bounds @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parseRectangle($));
    double innerRx @group(geometry) @editable;
    double innerRy @group(geometry) @editable;
}

class cPieSliceFigure extends cAbstractShapeFigure
{
    Rectangle bounds @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parseRectangle($));
    double startAngle @group(geometry) @editable;
    double endAngle @group(geometry) @editable;
}

class cPolygonFigure extends cAbstractShapeFigure
{
    Point points[] @group(geometry) @editable @getter(getPoint) @setter(setPoint) @sizeGetter(getNumPoints) @tostring(.str()) @fromstring(cFigure::parsePoint($));
    bool smooth @group(geometry) @editable;
    int joinStyle @group(outline) @editable @enum(omnetpp::cFigure::JoinStyle);
    int fillRule @group(fill) @editable @enum(omnetpp::cFigure::FillRule);
}

class cPathFigure extends cAbstractShapeFigure
{
    string path @group(geometry) @editable;
    Point offset @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
    int joinStyle @group(outline) @editable @enum(omnetpp::cFigure::JoinStyle);
    int capStyle @group(outline) @editable @enum(omnetpp::cFigure::CapStyle);
    int fillRule @group(fill) @editable @enum(omnetpp::cFigure::FillRule);
}

class cAbstractTextFigure extends cFigure
{
    Point position @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
    int anchor @group(geometry) @editable @enum(omnetpp::cFigure::Anchor);
    string text @group(text) @editable;
    Font font @group(text) @editable @tostring(.str()) @fromstring(cFigure::parseFont($));
    Color color @group(text) @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
    double opacity @group(text) @editable;
    bool halo @group(text) @editable;
}

class cTextFigure extends cAbstractTextFigure
{
}

class cLabelFigure extends cAbstractTextFigure
{
}

class cAbstractImageFigure extends cFigure
{
    Point position @group(geometry) @editable @tostring(.str()) @fromstring(cFigure::parsePoint($));
    int anchor @group(geometry) @editable @enum(omnetpp::cFigure::Anchor);
    double width @group(geometry) @editable;
    double height @group(geometry) @editable;
    int interpolation @group(image) @editable @enum(omnetpp::cFigure::Interpolation);
    double opacity @group(image) @editable;
    Color tintColor @group(image) @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
    double tintAmount @group(image) @editable;
}

class cImageFigure extends cAbstractImageFigure
{
    string imageName @editable;
}

class cIconFigure extends cImageFigure
{
}

class cPixmapFigure extends cAbstractImageFigure
{
    Pixmap pixmap;
}

class cCanvas extends cOwnedObject
{
    cFigure *rootFigure;
    string allTags;
    Color backgroundColor @editable @tostring(.str()) @fromstring(cFigure::parseColor($));
}

class cResultListener extends cIListener
{
}

class cResultFilter extends cResultListener
{
    cResultListener *delegates[] @sizeGetter(getNumDelegates) @getter(getDelegate) @tostring(->str());
}

class WarmupPeriodFilter extends cResultFilter
{
    simtime_t endWarmupPeriod;
}

class CountFilter extends cResultFilter
{
    long count;
}

class ConstantFilter extends cResultFilter
{
    double constant;
}

class Constant0Filter extends ConstantFilter
{
}

class Constant1Filter extends ConstantFilter
{
}

class cNumericResultFilter extends cResultFilter
{
}

class IdentityFilter extends cNumericResultFilter
{
}

class SumFilter extends cNumericResultFilter
{
    double sum;
}

class MeanFilter extends cNumericResultFilter
{
    double mean;
}

class MinFilter extends cNumericResultFilter
{
    double min;
}

class MaxFilter extends cNumericResultFilter
{
    double max;
}

class TimeAverageFilter extends cNumericResultFilter
{
    double timeAverage;
}

class RemoveRepeatsFilter extends cNumericResultFilter
{
    double lastValue;
}

class ExpressionFilter extends cNumericResultFilter
{
    string expression @tostring(.str());
    double lastValue;
    simtime_t lastTimestamp;
}

class UnaryExpressionFilter extends ExpressionFilter
{
    double lastInputValue;
}

class NaryExpressionFilter extends ExpressionFilter
{
}

class cObjectResultFilter extends cResultFilter
{
}

class PacketBytesFilter extends cObjectResultFilter
{
}

class PacketBitsFilter extends cObjectResultFilter
{
}

class SumPerDurationFilter extends cNumericResultFilter
{
    double sumPerDuration;
}

class cResultRecorder extends cResultListener
{
    cComponent *component;
    string resultName;
    string statisticName;
    string recordingMode;
}

class cNumericResultRecorder extends cResultRecorder
{
}

class VectorRecorder extends cNumericResultRecorder
{
    simtime_t lastWriteTime;
    double lastValue;
}

class CountRecorder extends cResultRecorder
{
    long count;
}

class LastValueRecorder extends cNumericResultRecorder
{
    double lastValue;
}

class SumRecorder extends cNumericResultRecorder
{
    double sum;
}

class MeanRecorder extends cNumericResultRecorder
{
    double mean;
}

class MinRecorder extends cNumericResultRecorder
{
    double min;
}

class MaxRecorder extends cNumericResultRecorder
{
    double max;
}

class TimeAverageRecorder extends cNumericResultRecorder
{
    double timeAverage @hint(Computed with the CURRENT simulation time);
}

class StatisticsRecorder extends cNumericResultRecorder
{
    cStatistic *statistic;
}

class StatsRecorder extends StatisticsRecorder
{
}

class HistogramRecorder extends StatisticsRecorder
{
}

class ExpressionRecorder extends cNumericResultRecorder
{
    string expression @tostring(.str());
    double currentValue;
    double lastInputValue;
}

