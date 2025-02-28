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
//

cplusplus(h) {{
namespace omnetpp {
extern void std_sim_descriptor_dummy();
}
}}


cplusplus(cc)
{{
#ifdef _MSC_VER
#  pragma warning(disable:4996)  // deprecation
#endif

#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "expressionfilter.h"
#include "nedsupport.h" // makeNedValue()

namespace omnetpp {

//
// Register enums from the sim. kernel
//
#define cF cFigure
Register_Enum(cPar::Type,         (cPar::BOOL, cPar::DOUBLE, cPar::INT, cPar::STRING, cPar::OBJECT, cPar::XML));
Register_Enum(cGate::Type,        (cGate::INPUT, cGate::OUTPUT, cGate::INOUT));
Register_Enum(cHistogram::Mode,   (cHistogram::MODE_INTEGERS, cHistogram::MODE_REALS, cHistogram::MODE_AUTO));
Register_Enum(cFigure::LineStyle, (cF::LINE_SOLID, cF::LINE_DOTTED, cF::LINE_DASHED));
Register_Enum(cFigure::CapStyle,  (cF::CAP_BUTT, cF::CAP_SQUARE, cF::CAP_ROUND));
Register_Enum(cFigure::JoinStyle, (cF::JOIN_BEVEL, cF::JOIN_MITER, cF::JOIN_ROUND));
Register_Enum(cFigure::FillRule,  (cF::FILL_EVENODD, cF::FILL_NONZERO));
Register_Enum(cFigure::Arrowhead, (cF::ARROW_NONE, cF::ARROW_SIMPLE, cF::ARROW_TRIANGLE, cF::ARROW_BARBED));
Register_Enum(cFigure::Anchor,    (cF::ANCHOR_CENTER, cF::ANCHOR_N, cF::ANCHOR_E, cF::ANCHOR_S, cF::ANCHOR_W, cF::ANCHOR_NW, cF::ANCHOR_NE, cF::ANCHOR_SE, cF::ANCHOR_SW, cF::ANCHOR_BASELINE_START, cF::ANCHOR_BASELINE_MIDDLE, cF::ANCHOR_BASELINE_END));
Register_Enum(cFigure::Alignment, (cF::ALIGN_LEFT, cF::ALIGN_RIGHT, cF::ALIGN_CENTER));
Register_Enum(cFigure::Interpolation, (cF::INTERPOLATION_NONE, cF::INTERPOLATION_FAST, cF::INTERPOLATION_BEST));
Register_Enum(cValue::Type,       (cValue::UNDEF, cValue::BOOL, cValue::INT, cValue::DOUBLE, cValue::STRING, cValue::POINTER));
#undef cF

//
// Define operator<< for some classes
//
inline std::ostream& operator<<(std::ostream& os, const cDisplayString& d) {
    return os << "\"" << d.str() << "\"";
}

using namespace canvas_stream_ops;

//
// This function is to be referenced from sim. kernel classes, to force the
// linker to include this file in executables
//
void std_sim_descriptor_dummy() {}

// This cannot go into the header, because ExpressionFilter is not available via <omnetpp.h>
template<> inline ExpressionFilter *fromAnyPtr(any_ptr ptr) {return static_cast<ExpressionFilter *>(ptr.get<cObject>());}

static void trySetName(cObject *obj, const char *s) 
{
    if (auto named = dynamic_cast<cNamedObject*>(obj))
        named->setName(s);
    else
        throw cRuntimeError("Cannot set name for a %s: not a cNamedObject", obj->getClassName());
}

}  // namespace omnetpp

}}

//TODO remove @descriptor(readonly) where possible

namespace omnetpp;

@property[icon](type=string; usage=class; desc="Icon for objects of this class in Qtenv inspectors");
@property[label](type=string; usage=field; desc="When specified, this string will be displayed as field name in Qtenv inspectors");
@property[hint](type=string; usage=field; desc="Short description of the field, displayed in Qtenv inspectors as tooltip");
@property[group](type=string; usage=field; desc="Used for grouping of fields in Qtenv inspectors");
@property[packetData](type=string; usage=class,field; desc="Denotes packet data in frameworks such as INET; used in Qtenv inspectors");
@property[eventlog](type=string; usage=field; desc="When @eventlog(skip) is given, eventlog recording will skip this field when serializing objects");

enum cPar::Type;
enum cGate::Type;
enum cHistogram::Mode;
enum cFigure::LineStyle;
enum cFigure::CapStyle;
enum cFigure::JoinStyle;
enum cFigure::FillRule;
enum cFigure::Arrowhead;
enum cFigure::Anchor;
enum cFigure::Alignment;
enum cFigure::Interpolation;
enum cValue::Type;

class simtime_t
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @primitive;
    @polymorphic(false);
    @cppType(omnetpp::simtime_t);
    @toString(simtime2string($));
    @fromString(string2simtime($));
    @toValue($.dbl()); // note: precision loss
    @fromValue($.doubleValue());
    @defaultValue(SIMTIME_ZERO);
}

class msgid_t
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @primitive;
    @castFunction(false);
    @fromString(string2int64($));
    @toString(int642string($));
    @defaultValue(0);
    @toValue((omnetpp::intval_t)($));
    @fromValue(omnetpp::checked_int_cast<int64_t>($.intValue()));
}

class txid_t
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @primitive;
    @castFunction(false);
    @fromString(string2int64($));
    @toString(int642string($));
    @defaultValue(0);
    @toValue((omnetpp::intval_t)($));
    @fromValue(omnetpp::checked_int_cast<int64_t>($.intValue()));
}

class cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @castFunction(false); // defined in the omnetpp headers
    @icon(cogwheel);
    string className @group(base) @hint("C++ class name") @eventlog(skip);
    string name @group(base) @editable @setter(trySetName($,value)) @hint("A string attribute of the object, also used for fullName and fullPath");
    string fullName @group(base) @hint("If object is part of a module/gate vector: name with index");
    string fullPath @group(base) @hint("Hierarchical name, composed of full names of this object and its owners/parents") @eventlog(skip);
    string info @getter(str) @group(base) @hint("Text provided by the object's \"str()\" method") @eventlog(skip);
}

class cNamedObject extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cOwnedObject extends cNamedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cObject *owner @group(base) @hint("Module or container this object belongs to") @eventlog(skip);
}

class cNoncopyableOwnedObject extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cSoftOwner extends cNoncopyableOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cOwnedObject *ownedObjects[] @getter(getOwnedObject) @sizeGetter(getNumOwnedObjects) @group(owned objects) @hint("List of objects directly held by this component");
}

class cDisplayString
{
    @existingClass;
    @overwritePreviousDefinition;
    @polymorphic(false);
    @descriptor(readonly);
    string value @getter(str) @setter(parse) @editable @hint("The editable display string itself");
    @toValue(.str());
}

class cProperty extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    bool isImplicit @getter(isImplicit) @group(fields);
    string index @group(fields);
    string value @getter(str) @hint("Name and value");
}

class cIListener
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
}

class cComponent::SignalListenerList
{
    @existingClass;
    @overwritePreviousDefinition;
    @polymorphic(false);
    @descriptor(readonly);
    string signalName @hint("Name of signal listened on");
    cIListener *listeners[] @getter(getListener) @sizeGetter(countListeners); //TODO @toString(->str()) for cResultListeners
}

class cComponent extends cSoftOwner
{
    @existingClass;
    @overwritePreviousDefinition;
    string displayName @group(base) @editable @hint("Display name");
    bool isModule @readonly @getter(isModule) @group(fields) @hint("Whether this component is a module or a channel");
    cComponentType *componentType @readonly @group(fields) @hint("The object that represents the type declaration for this component");
    cPar par[] @editable @resizable(false) @getter(par) @setter(pp->par(i).parse(value)) @sizeGetter(getNumParams) @toValue(toAnyPtr(&$)) @fromValue(TODO) @label(parameters) @group(parameters, gates) @hint("NED parameters");
    cProperty *properties_[] @readonly @label(properties) @getter(->getProperties()->get(i)) @sizeGetter(->getProperties()->getNumProperties()) @group(parameters, gates) @hint("NED properties");
    cComponent::SignalListenerList listenedSignals[] @readonly @getter(getListenerList) @sizeGetter(getSignalTableSize) @toString(.str()) @toValue(toAnyPtr(&($))) @group(signals, statistics) @hint("Signal listener lists");
    cResultRecorder *results[] @readonly @getter(->getResultRecorders()[i]) @sizeGetter(->getResultRecorders().size()) @toString(->str()) @group(signals, statistics) @hint("Statistical results collected in this component");
}

class cChannel extends cComponent
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(channel);
    cDisplayString displayString @group(fields) @hint("Controls appearance of this channel in the GUI") @editable @setter(setDisplayString) @fromString($) @toString(.str()) @getter(getDisplayString) @fromValue($.stringValue());
}

class cIdealChannel extends cChannel
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    // nothing special
}

class cDelayChannel extends cChannel
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    // nothing special
}

class cDatarateChannel extends cChannel
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    // Note: no need to spell out delay etc, they come from cComponent parameters
    bool isBusy @getter(isBusy) @group(fields) @hint("If channel is currently transmitting");
    simtime_t transmissionFinishTime @group(fields) @hint("If busy: when currently active transmissions will finish");
}

class cModule extends cComponent
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(compound);
    int id @group(fields) @hint("Unique module ID -- IDs of deleted modules are not issued again");
    bool isSimple @getter(isSimple) @group(fields) @hint("Whether this is a simple module");
    bool isVector @getter(isVector) @group(fields) @hint("Whether this module is part of a module vector");
    int index @group(fields) @toString(pp->isVector() ? long2string($) : "<not a vector>") @hint("If vector: index of this module in its module vector");
    int vectorSize @group(fields) @toString(pp->isVector() ? long2string($) : "<not a vector>") @hint("If vector: size of module vector this module belongs to");
    bool builtinAnimationsAllowed @editable @group(fields) @hint("Whether built-in animations are enabled on this module's graphical inspector");
    cDisplayString displayString @group(fields) @hint("Controls appearance of this module in the GUI") @editable @setter(setDisplayString) @fromString($) @toString(.str()) @getter(getDisplayString) @fromValue($.stringValue());
    cGate *gate[] @sizeGetter(gateCount) @getter(gateByOrdinal) @group(parameters, gates) @label(gates) @hint("Module gates");
    cCanvas *canvas @getter(getCanvasIfExists) @group(fields) @hint("Built-in canvas instance");
    cOsgCanvas *osgCanvas @getter(getOsgCanvasIfExists) @group(fields) @hint("Built-in 3D (OpenSceneGraph) canvas instance");
}

class cSimpleModule extends cModule
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
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
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string nedSource @getter(getNedSource) @hint("NED source of component if available");
    string documentation @getter(getDocumentation) @hint("Documentation of component if available");
}

class cModuleType extends cComponentType
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    bool isNetwork @getter(isNetwork) @group(fields) @hint("Whether this module type can be used as toplevel module");
}

class cChannelType extends cComponentType
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cRegistrationList extends cNamedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cObject *contents[] @getter(get) @sizeGetter(size) @hint("Contained objects");
}

//----

class cGate extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(gate);
    char type @group(fields) @enum(cGate::Type) @hint("Gate type");
    int id @group(fields) @hint("Gate id, unique within this module");
    bool isVector @getter(isVector) @group(fields) @hint("Whether this gate is part of a gate vector");
    int index @group(fields) @hint("If vector: index of this gate within its gate vector");
    bool deliverImmediately @group(fields) @hint("Only for simple module input gates, and if transmission duration is nonzero: whether packets arriving on this gate will be given to the module immediately, or at the end of the reception");

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
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(container);
    bool takeOwnership @group(fields) @editable @hint("Whether the array should own the objects inserted into it");
    cObject *contents[] @getter(get) @sizeGetter(size) @hint("Objects contained in the array");
}

class cEvent extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    short schedulingPriority @group(event) @editable @hint("Controls order of messages with equal timestamps in FES");
    cObject *targetObject @group(event) @hint("The object this message will be delivered to");
    bool isScheduled @getter(isScheduled) @group(sending) @hint("Whether event or message is currently scheduled (or in transit)");
    simtime_t arrivalTime @group(sending) @hint("Simulation time the event or message will be (or was) delivered");
}

class cMessage extends cEvent
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon("message");
    msgid_t id @group("message") @hint("Automatically assigned unique identifier");
    msgid_t treeId @group("message") @hint("Identifier shared between the message and its copies");
    short kind @group("message") @editable @hint("Generic user-settable attribute, usually carries \"type\" or \"role\" or \"category\" of message");
    string displayString @group("message") @hint("Controls appearance of the message in GUI");
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
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon("packet");
    int64_t bitLength @group("packet") @editable @hint("Simulated length of the message in bits, affects transmission time and probability of bit errors when sent through a channel");
    int64_t byteLength @group("packet") @editable @hint("Length in bytes, i.e. length in bits divided by eight");
    bool hasBitError @getter(hasBitError) @group("packet") @setter(setBitError) @editable @hint("Indicates that a bit error occurred when the message was sent through a channel with nonzero bit error rate");
    cPacket *encapsulatedPacket @packetData @hint("Used with protocol stacks: stores an encapsulated higher-layer packet");
    bool txChannelEncountered @group(sending) @hint("If true, the packet has encountered a transmission channel during its last send");
    bool isUpdate @group(sending) @hint("If true, this is not a separate packet but a modification to a previous packet transmission (see remainingDuration too)");
    txid_t transmissionId @group(sending) @editable @hint("When isUpdate=true: identifies the original packet transmission");
    simtime_t duration @group(sending) @hint("Transmission time over a channel with datarate");
    simtime_t remainingDuration @group(sending) @hint("Remaining transmission time over a channel with datarate (see isUpdate=true)");
    bool isReceptionStart @getter(isReceptionStart) @group(sending) @hint("True if duration==remainingDuration");
    bool isReceptionEnd @getter(isReceptionEnd) @group(sending) @hint("True if remainingDuration is zero");
}

//----

class cRNG extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
}

class cRandom extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @icon(cogwheel); //TODO
    cRNG *rng @getter(getRNG) @setter(setRNG);
}

class cUniform extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double a;
    double b;
}

class cExponential extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double mean;
}

class cNormal extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double mean;
    double stddev;
}

class cTruncNormal extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double mean;
    double stddev;
}

class cGamma extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double alpha;
    double theta;
}

class cBeta extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double alpha1;
    double alpha2;
}

class cErlang extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    unsigned int k;
    double mean;
}

class cChiSquare extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    unsigned int k;
}

class cStudentT extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    unsigned int i;
}

class cCauchy extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double a;
    double b;
}

class cTriang extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double a;
    double b;
    double c;
}

class cWeibull extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double a;
    double b;
}

class cParetoShifted extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double a;
    double b;
    double c;
}

class cIntUniform extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    int a;
    int b;
}

class cBernoulli extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double p;
}

class cBinomial extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    int n;
    double p;
}

class cGeometric extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double p;
}

class cNegBinomial extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    int n;
    double p;
}

class cPoisson extends cRandom
{
    @existingClass;
    @overwritePreviousDefinition;
    double lambda;
}

//----

class cStatistic extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @icon(statistic);
    long count @readonly @group(statistical summary) @hint("Number of observations collected, regardless of their weights");
    double sumWeights @readonly @group(statistical summary) @hint("Sum of weights, which equals the number of observations in the unweighted case");
    double min @readonly @group(statistical summary) @hint("Minimum of the observations");
    double max @readonly @group(statistical summary) @hint("Maximum of the observations");
    double mean @readonly @group(statistical summary) @hint("Mean of the observations");
    double stddev @readonly @group(statistical summary) @hint("Standard deviation of the observations");
    double variance @readonly @group(statistical summary) @hint("Variance of the observations");
}

class cStdDev extends cStatistic
{
    @existingClass;
    @overwritePreviousDefinition;
}

class cAbstractHistogram extends cStdDev
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    bool binsAlreadySetUp @readonly @getter(binsAlreadySetUp) @group(histogram) @hint("If false, the object is still collecting initial observations to gather information for laying out histogram bins");
    double underflowSumWeights @readonly @group(histogram) @hint("Total weight of observations above the last histogram bin");
    double overflowSumWeights @readonly @group(histogram) @hint("Total weight of observations above the last histogram bin");
    int numBins @readonly @group(histogram) @hint("Number of bins in the histogram");
    cAbstractHistogram::Bin binInfo[] @readonly @getter(internalGetBinInfo) @sizeGetter(getNumBins) @group(histogram) @hint("Histogram bin bounds, observation count in the bin, and density estimate in the bin");
}

class cAbstractHistogram::Bin
{
    @existingClass;
    @overwritePreviousDefinition;
    @opaque;
    @polymorphic(false);
    @toString(.str());
    @toValue(.str());
    @descriptor(false);
}

class cHistogram extends cAbstractHistogram
{
    @existingClass;
    @overwritePreviousDefinition;
    cIHistogramStrategy *strategy @group(histogram) @hint("Determines when and how bins are created");
}

class cIHistogramStrategy extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
}

class cFixedRangeHistogramStrategy extends cIHistogramStrategy
{
    @existingClass;
    @overwritePreviousDefinition;
    double lowerLimit @hint("Histogram range lower endpoint");
    double upperLimit @hint("Histogram range upper endpoint");
    double binSize;
    int mode @enum(cHistogram::Mode);
}

class cPrecollectionBasedHistogramStrategy extends cIHistogramStrategy
{
    @existingClass;
    @overwritePreviousDefinition;
    int numToPrecollect;
    int numToCollate;
    int rangeUnchangedThreshold;
}

class cDefaultHistogramStrategy extends cPrecollectionBasedHistogramStrategy
{
    @existingClass;
    @overwritePreviousDefinition;
    int numBinsHint @hint("Desired number of bins");
}

class cAutoRangeHistogramStrategy extends cPrecollectionBasedHistogramStrategy
{
    @existingClass;
    @overwritePreviousDefinition;
    double lowerLimitHint @hint("Histogram range lower endpoint");
    double upperLimitHint @hint("Histogram range upper endpoint");
    double rangeExtensionFactor;
    int mode @enum(cHistogram::Mode);
    double binSizeHint @hint("Requested bin size");
    bool binSizeRounding;
    int numBinsHint @hint("Desired number of bins");
    bool autoExtend;
    bool binMerging;
    int maxNumBins;
}

class cPrecollectionBasedDensityEst extends cAbstractHistogram
{
    @existingClass;
    @overwritePreviousDefinition;
}

class cKSplit extends cPrecollectionBasedDensityEst
{
    @existingClass;
    @overwritePreviousDefinition;
}

class cPSquare extends cAbstractHistogram
{
    @existingClass;
    @overwritePreviousDefinition;
}

//----

class cExpression extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    string str @readonly @getter(str);
}

class cValue
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @toString(.str());
    int type @readonly @enum(cValue::Type) @hint("Value type");
    cObject *containedObject @readonly @getter(cValue::getContainedObject($)) @hint("Only of type==OBJECT");
}

class cValueArray extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cValue elements[] @getter(get) @setter(set) @sizeGetter(size);
}

struct cValueMap::Entry
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @toString(cValueMap::entryToStr($));
    string first @hint("Key");
    cValue second @hint("Value");
}

class cValueMap extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cValueMap::Entry entries[] @sizeGetter(size) @getter(getEntry) @toValue(toAnyPtr(&($)));
}

class cFSM extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    int state @hint("Numeric code of the state the FSM is currently in");
    string stateName @hint("Name of the current FSM state");
}

class cNedMathFunction extends cNoncopyableOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string category @hint("Function category");
    string description;
    int numArgs @hint("Number of \"double\" parameters this function takes");
}

class cNedFunction extends cNoncopyableOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
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
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(outvector);
    bool isEnabled @getter(isEnabled) @setter(setEnabled) @editable @hint("If disabled, values passed to this object to record are simply discarded");
    long valuesReceived @hint("Number of values passed to this object to record");
    long valuesStored @hint("Number of values actually stored on the disk");
}

class cPar extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(param);
    @fromString(parse);
    string value @getter(str) @setter(parse) @editable;
    char type @enum(cPar::Type) @hint("Parameter type");
    string unit @hint("For long and double types only");
    bool isNumeric @getter(isNumeric) @group(flags) @hint("Whether parameter type is numeric, i.e. double or long");
    bool isVolatile @getter(isVolatile) @group(flags) @hint("Whether parameter was declared volatile in the NED file");
    bool isMutable @getter(isMutable) @group(flags) @hint("Whether parameter setting the parameter value at runtime is supported");
    bool isExpression @getter(isExpression)@group(flags) @hint("Whether parameter value is a constant or an expression");
    bool isShared @getter(isShared) @group(flags) @hint("Whether storage could be optimized by several modules sharing the same parameter value object");
    string assignedFrom @getter(getSourceLocation) @hint("File/line info where the value of this parameter was parsed from");
    string baseDirectory @getter(getBaseDirectory) @hint("Base directory for interpreting potential relative path names in the contained expression");
}

class cMsgPar extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string value @getter(str) @setter(parse) @editable @hint("Parameter value");
    char type @enum(cPar::Type) @hint("Parameter type");
    bool isNumeric @getter(isNumeric) @group(flags) @hint("Whether parameter is of numeric type");
    bool isConstant @getter(isConstant) @group(flags) @hint("Whether the parameter has a constant value");
}

class cFutureEventSet extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(container);
    int length @hint("Number of items contained");
    cEvent *events[] @getter(get) @sizeGetter(getLength) @hint("Events and self-messages and currently scheduled, and messages currently in transit");
}

class cEventHeap extends cFutureEventSet
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cQueue extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @icon(queue);
    bool takeOwnership @group(fields) @editable @hint("Whether the queue should own the objects inserted into it");
    int length @hint("Number of items in the queue");
    cObject *contents[] @getter(get) @sizeGetter(getLength) @hint("Queue contents");
}

class cPacketQueue extends cQueue
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    int64_t bitLength @hint("Total size of messages in the queue");
    int64_t byteLength @hint("Total size of messages in the queue, in bytes");
}

class cHasher
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @polymorphic(false);
    string hash @getter(str) @hint("The current hash value");
}

class cFingerprintCalculator extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cSimulation extends cNamedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
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
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cSequentialScheduler extends cScheduler
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cRealTimeScheduler extends cScheduler
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cTopology::Node
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @polymorphic(false);
    int moduleId @hint("ID of the module this node corresponds to");
    cModule *module @hint("The module this node corresponds to");
    double weight @hint("Node weight -- affects graph algorithms such as shortest path");
    bool enabled @getter(isEnabled) @hint("Node state -- affects graph algorithms such as shortest path");

    cTopology::LinkIn *inLink[] @getter(getLinkIn) @sizeGetter(getNumInLinks) @hint("Incoming links of this graph node");
    cTopology::LinkOut *outLink[] @getter(getLinkOut) @sizeGetter(getNumOutLinks) @hint("Outgoing links of this graph node");

    double distanceToTarget @group(shortest path) @hint("After running a shortest path algorithm: distance to the target node");
    cTopology::LinkOut *path[] @sizeGetter(getNumPaths) @group(shortest path) @hint("The next link of the shortest path or paths towards the target node");
}

class cTopology::Link
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @polymorphic(false);
    double weight @hint("Link weight -- affects graph algorithms such as shortest path");
    bool enabled @getter(isEnabled) @hint("Link state -- affects graph algorithms such as shortest path");
}

class cTopology::LinkIn extends cTopology::Link
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @polymorphic(false);
    cTopology::Node *remoteNode @hint("The node at the remote end of this connection");
    cGate *remoteGate @hint("The gate at the remote end of this connection");
    cGate *localGate @hint("The gate at the local end of this connection");
}

class cTopology::LinkOut extends cTopology::Link
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @polymorphic(false);
    cTopology::Node *remoteNode @hint("The node at the remote end of this connection");
    cGate *remoteGate @hint("The gate at the remote end of this connection");
    cGate *localGate @hint("The gate at the local end of this connection");
}

class cTopology extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    int numNodes @group(fields) @hint("Number of nodes in this topology object");
    cTopology::Node *node[] @sizeGetter(getNumNodes) @hint("List of nodes in this topology object");
}

class cXMLElement extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string tagName @hint("XML element tag");
    string nodeValue @hint("Contents of text node inside this XML element");
    string sourceLocation @hint("Location this XML element was parsed from");
    string attributes[] @getter(getAttrDesc) @sizeGetter(getNumAttrs) @hint("Attributes");
    cXMLElement *children[] @getter(getChild) @sizeGetter(getNumChildren) @hint("Child elements");
    string xml @getter(getXML) @hint("Element tree as XML fragment");
}

class cFigure::Point
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @editable;
    @polymorphic(false);
    @toString(.str());
    @fromString(cFigure::parsePoint($));
    @toValue(.str());
    @fromValue(cFigure::parsePoint($));
}

class cFigure::Rectangle
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @editable;
    @polymorphic(false);
    @toString(.str());
    @fromString(cFigure::parseRectangle($));
    @toValue(.str());
    @fromValue(cFigure::parseRectangle($));
}

class cFigure::Transform
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @editable;
    @polymorphic(false);
    @toString(.str());
    @fromString(cFigure::parseTransform($));
    @toValue(.str());
    @fromValue(cFigure::parseTransform($));
}

class cFigure::Color
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @editable;
    @polymorphic(false);
    @toString(.str());
    @fromString(cFigure::parseColor($));
    @toValue(.str());
    @fromValue(cFigure::parseColor($));
}

class cFigure::Font
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @editable;
    @polymorphic(false);
    @toString(.str());
    @fromString(cFigure::parseFont($));
    @toValue(.str());
    @fromValue(cFigure::parseFont($));
}

class cFigure::Pixmap
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);
    @opaque;
    @polymorphic(false);
    @toValue(.str());
}

class cFigure extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    int id @group(figure);
    bool visible @group(figure) @getter(isVisible) @editable;
    double zIndex @group(figure) @editable;
    string tooltip @group(figure) @editable;
    cObject *associatedObject @group(figure);
    string tags @group(figure) @editable;
    cFigure::Transform transform @group(figure) @editable;
    cFigure *children[] @group(figure) @getter(getFigure) @sizeGetter(getNumFigures);
}

class cGroupFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cPanelFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point position @group(geometry) @editable;
    Point anchorPoint @group(geometry) @editable;
}

class cAbstractLineFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Color lineColor @group(line) @editable;
    int lineStyle @group(line) @editable @enum(cFigure::LineStyle);
    double lineWidth @group(line) @editable;
    double lineOpacity @group(line) @editable;
    int capStyle @group(line) @editable @enum(cFigure::CapStyle);
    int startArrowhead @group(line) @editable @enum(cFigure::Arrowhead);
    int endArrowhead @group(line) @editable @enum(cFigure::Arrowhead);
    bool zoomLineWidth @group(line) @editable;
}

class cLineFigure extends cAbstractLineFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point start @group(geometry) @editable;
    Point end @group(geometry) @editable;
}

class cArcFigure extends cAbstractLineFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Rectangle bounds @group(geometry) @editable;
    double startAngle @group(geometry) @editable;
    double endAngle @group(geometry) @editable;
}

class cPolylineFigure extends cAbstractLineFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point points[] @group(geometry) @editable @getter(getPoint) @setter(setPoint) @sizeGetter(getNumPoints) @sizeSetter(setNumPoints);
    bool smooth @group(geometry) @editable;
    int joinStyle @group(line) @editable @enum(cFigure::JoinStyle);
}

class cAbstractShapeFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    bool outlined @group(outline) @getter(isOutlined) @editable;
    bool filled @group(fill) @getter(isFilled) @editable;
    Color lineColor @group(outline) @editable;
    Color fillColor @group(fill) @editable;
    int lineStyle @group(outline) @editable @enum(cFigure::LineStyle);
    double lineWidth @group(outline) @editable;
    double lineOpacity @group(outline) @editable;
    double fillOpacity @group(fill) @editable;
    bool zoomLineWidth @group(outline) @editable;
}

class cRectangleFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Rectangle bounds @group(geometry) @editable;
    double cornerRx @group(geometry) @editable;
    double cornerRy @group(geometry) @editable;
}

class cOvalFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Rectangle bounds @group(geometry) @editable;
}

class cRingFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Rectangle bounds @group(geometry) @editable;
    double innerRx @group(geometry) @editable;
    double innerRy @group(geometry) @editable;
}

class cPieSliceFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Rectangle bounds @group(geometry) @editable;
    double startAngle @group(geometry) @editable;
    double endAngle @group(geometry) @editable;
}

class cPolygonFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point points[] @group(geometry) @editable @getter(getPoint) @setter(setPoint) @sizeGetter(getNumPoints)  @sizeSetter(setNumPoints);
    bool smooth @group(geometry) @editable;
    int joinStyle @group(outline) @editable @enum(cFigure::JoinStyle);
    int fillRule @group(fill) @editable @enum(cFigure::FillRule);
}

class cPathFigure extends cAbstractShapeFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string path @group(geometry) @editable;
    Point offset @group(geometry) @editable;
    int joinStyle @group(outline) @editable @enum(cFigure::JoinStyle);
    int capStyle @group(outline) @editable @enum(cFigure::CapStyle);
    int fillRule @group(fill) @editable @enum(cFigure::FillRule);
}

class cAbstractTextFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point position @group(geometry) @editable;
    int anchor @group(geometry) @editable @enum(cFigure::Anchor);
    int alignment @group(geometry) @editable @enum(cFigure::Alignment);
    string text @group(text) @editable;
    Font font @group(text) @editable;
    Color color @group(text) @editable;
    double opacity @group(text) @editable;
    bool halo @group(text) @editable;
}

class cTextFigure extends cAbstractTextFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cLabelFigure extends cAbstractTextFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double angle @group(geometry) @editable;
}

class cAbstractImageFigure extends cFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Point position @group(geometry) @editable;
    int anchor @group(geometry) @editable @enum(cFigure::Anchor);
    double width @group(geometry) @editable;
    double height @group(geometry) @editable;
    int interpolation @group(image) @editable @enum(cFigure::Interpolation);
    double opacity @group(image) @editable;
    Color tintColor @group(image) @editable;
    double tintAmount @group(image) @editable;
}

class cImageFigure extends cAbstractImageFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    string imageName @editable;
}

class cIconFigure extends cImageFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cPixmapFigure extends cAbstractImageFigure
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    Pixmap pixmap;
}

class cCanvas extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cFigure *rootFigure;
    string allTags;
    cFigure::Color backgroundColor @editable;
}

class cOsgCanvas extends cOwnedObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(false);  // descriptor will come from osg.msg in qtenv
    @castFunction(false); // it will come from osg.msg in qtenv
}

class cResultListener extends cObject
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @implements(cIListener);
}

class cResultFilter extends cResultListener
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cResultListener *delegates[] @sizeGetter(getNumDelegates) @getter(getDelegate) @toString(->str());
}

class WarmupPeriodFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    simtime_t endWarmupPeriod;
}

class CountFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    long count;
}

class ConstantFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double constant;
}

class Constant0Filter extends ConstantFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class Constant1Filter extends ConstantFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class cNumericResultFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class IdentityFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class SumFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double sum;
}

class MeanFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double mean;
}

class MinFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double min;
}

class MaxFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double max;
}

class TimeAverageFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double timeAverage;
}

class RemoveRepeatsFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double lastValue;
}

class ExpressionFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    @castFunction(false);
    string expression @toString(.str()) @toValue(.str());
    string lastValue @toString(.str()) @toValue(nedsupport::makeNedValue($));
    simtime_t lastTimestamp;
}

class cObjectResultFilter extends cResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class PacketBytesFilter extends cObjectResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class PacketBitsFilter extends cObjectResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class SumPerDurationFilter extends cNumericResultFilter
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double sumPerDuration;
}

class cResultRecorder extends cResultListener
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cComponent *component;
    string resultName;
    string statisticName;
    string recordingMode;
}

class cNumericResultRecorder extends cResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class VectorRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    simtime_t lastWriteTime;
    double lastValue;
}

class CountRecorder extends cResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    long count;
}

class LastValueRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double lastValue;
}

class SumRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double sum;
}

class MeanRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double mean;
}

class MinRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double min;
}

class MaxRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double max;
}

class TimeAverageRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    double timeAverage @hint(Computed with the CURRENT simulation time);
}

class StatisticsRecorder extends cNumericResultRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
    cStatistic *statistic;
}

class StatsRecorder extends StatisticsRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class HistogramRecorder extends StatisticsRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class KSplitRecorder extends StatisticsRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

class PSquareRecorder extends StatisticsRecorder
{
    @existingClass;
    @overwritePreviousDefinition;
    @descriptor(readonly);
}

