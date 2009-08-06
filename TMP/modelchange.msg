//==========================================================================
//  CMODELCHANGEDATA.MSG - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// FIXME todo document:
//
// XXX use dynamic_cast<> to figure out what notification arrived!
//
// PRE_MODEL_CHANGE and POST_MODEL_CHANGE are fired on the module (or channel)
// affected by the change, and NOT on the module which executes the code that
// causes the change. That is, pre-moduleRemoved is fired on the module
// to be removed, and post-moduleRemoved is fired on its parent (because
// the original module no longer exists), and not on the module that contains
// the deleteModule() call.
//
//XXX should this whole thing be generated via a .msg file? then we'd have reflection!!! (NEEDED FOR EASY UNIT TESTING!)
//XXX per-gate object create/delete notifications
//XXX add cPre/PostComponentInitialize, cPre/PostComponentFinalize listeners


cplusplus {{

//
// A signal which is fired before simulation model changes such as module
// creation or connection creation. The signals carry data objects that
// describe the type and details the change. The data objects are subclassed
// from cModelChangeNotification, and begin with the prefix cPre...
// These classes include:
//    - cPreModuleAddNotification,
//    - cPreModuleDeleteNotification,
//    - cPreModuleReparentNotification,
//    - cPreGateAddNotification,
//    - cPreGateDeleteNotification,
//    - cPreGateVectorResizeNotification,
//    - cPreGateConnectNotification,
//    - cPreGateDisconnectNotification,
//    - cPrePathCreateNotification,
//    - cPrePathCutNotification,
//    - cPreParameterChangeNotification,
//    - cPreDisplayStringChangeNotification,
//
// See also: cComponent::emit(), cComponent::subscribe()
//
#define PRE_MODEL_CHANGE   ((simsignal_t)0)

//
// A signal which is fired after simulation model changes such as module
// creation or connection creation. The signals carry data objects that
// describe the type and details the change. The data objects are subclassed
// from cModelChangeNotification, and begin with the prefix cPost...
// These classes include:
//    - cPostModuleAddNotification,
//    - cPostModuleDeleteNotification,
//    - cPostModuleReparentNotification,
//    - cPostGateAddNotification,
//    - cPostGateDeleteNotification,
//    - cPostGateVectorResizeNotification,
//    - cPostGateConnectNotification,
//    - cPostGateDisconnectNotification,
//    - cPostPathCreateNotification,
//    - cPostPathCutNotification,
//    - cPostParameterChangeNotification,
//    - cPostDisplayStringChangeNotification,
//    - cPostModuleAddNotification,
//    - cPostModuleDeleteNotification,
//    - cPostModuleReparentNotification,
//    - cPostGateAddNotification,
//    - cPostGateDeleteNotification,
//    - cPostGateVectorResizeNotification,
//    - cPostGateConnectNotification,
//    - cPostGateDisconnectNotification,
//    - cPostPathCreateNotification,
//    - cPostPathCutNotification,
//    - cPostParameterChangeNotification,
//    - cPostDisplayStringChangeNotification,
//
// See also: cComponent::emit(), cComponent::subscribe()
//
#define POST_MODEL_CHANGE  ((simsignal_t)1)

typedef const char * opp_const_char_ptr;

}}
class opp_const_char_ptr extends void;

class cModuleType extends void;
class cModule extends void;
class cGate extends void;
class cGate::Type extends void;
class cPar extends void;
class cChannel extends void;
class cDisplayString extends void;

//
// Common base class for data objects that acompany PRE_MODEL_CHANGE
// and POST_MODEL_CHANGE notifications (signals).
//
struct cModelChangeNotification extends cObject
{
};

//
// Fired at the top of cModuleType::create(); fields contain the cModuleType
// object, and the arguments of the create() method call.
//
struct cPreModuleAddNotification extends cModelChangeNotification
{
//XXX    cModuleType *moduleType; //XXX + moduleid?
//XXX    opp_const_char_ptr moduleName;
//XXX    cModule *parentModule;
    int vectorSize;
    int index;
};

//
// Fired at the end of cModuleType::create(); at that stage the module is
// already created, its gates and parameters are added and it is inserted
// into the model, but it is not yet initialized nor its submodules
// are created yet.
//
struct cPostModuleAddNotification extends cModelChangeNotification
{
    cModule *module;
};

//
// Fired at the top of cModule::deleteModule(). The module still exists
// at this point.
//
//XXX called also on network teardown
struct cPreModuleDeleteNotification extends cModelChangeNotification
{
    cModule *module;
};

//
// Fired at the end of cModule::deleteModule(). The module object no longer
// exists at this point, and its submodules have also been deleted.
// Fields include properties of the deleted module. It also includes the
// module pointer (in case it serves as a key in some user data structure),
// but it must NOT be dereferenced because it points to a deleted object.
//
struct cPostModuleDeleteNotification extends cModelChangeNotification
{
    cModule *module;  // this is the pointer of the already deleted module -- DO NOT DEREFERENCE
    int moduleId;
    cModuleType *moduleType;
    opp_const_char_ptr moduleName;
    cModule *parentModule;
    int vectorSize;
    int index;
};

//
// Fired at the top of cModule::changeParentTo(), before any changes have
// been done.
//
struct cPreModuleReparentNotification extends cModelChangeNotification
{
    cModule *module;
    cModule *newParentModule;
};

//
// Fired at the end of cModule::changeParentTo().
//
struct cPostModuleReparentNotification extends cModelChangeNotification
{
    cModule *module;
    cModule *oldParentModule;
};

//XXX TODO introduce GateObjectAdded/Removed signals as well?

//
// This gets fired at the top of cModule::addGate(), that is, when a gate
// or gate vector is added to the module.
//
// Note: this notification is fired for the gate or gate vector as a
// whole, and not for individual gate objects in it. That is, a single
// notification is fired for an inout gate (which is a gate pair) and
// for gate vectors as well.
//
// Fields in this class carry the module object on which the gate or gate vector
// being created, and the arguments of the addGate() method call.
//
struct cPreGateAddNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
    cGate::Type gateType;
    bool isVector;
};

//
// This gets fired at the bottom of cModule::addGate(), that is, when a gate
// or gate vector was added to the module.
//
// Note: this notification is fired for the gate or gate vector as a
// whole, and not for individual gate objects in it. That is, a single
// notification is fired for an inout gate (which is a gate pair) and
// for gate vectors as well.
//
// Fields in this class carry the module object on which the gate or gate vector
// was created, and the name of the gate or gate vector.
//
struct cPostGateAddNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
};

//
// Fired at the top of cModule::deleteGate(). The gate or gate vector still
// exists at this point.
//
// Note: this notification is fired for the gate or gate vector as a
// whole, and not for individual gate objects in it. That is, a single
// notification is fired for an inout gate (which is a gate pair) and
// for gate vectors as well.
//
struct cPreGateDeleteNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
};

//
// Fired at the end of cModule::deleteGate(). The gate or gate vector
// no longer exists at this point.
//
// Note: this notification is fired for the gate or gate vector as a
// whole, and not for individual gate objects in it. That is, a single
// notification is fired for an inout gate (which is a gate pair) and
// for gate vectors as well.
//
// Fields include properties of the deleted gate or gate vector.
//
struct cPostGateDeleteNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
    cGate::Type gateType;
    bool isVector;
    int vectorSize;
};

//
// Fired at the top of cModule::setGateSize(). Note that other cModule methods
// used for implementing the NED "gate++" syntax also expand the gate vector,
// and fire this notification. These methods are getOrCreateFirstUnconnectedGate()
// and getOrCreateFirstUnconnectedGatePair()).
//
struct cPreGateVectorResizeNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
    int newSize;
};

//
// Fired at the end of cModule::setGateSize(). Note that other cModule methods
// used for implementing the NED "gate++" syntax also expand the gate vector,
// and fire this notification. These methods are getOrCreateFirstUnconnectedGate()
// and getOrCreateFirstUnconnectedGatePair()).
//
struct cPostGateVectorResizeNotification extends cModelChangeNotification
{
    cModule *module;
    opp_const_char_ptr gateName;
    int oldSize;
};

//
// XXX improve docu
// This notification is fired on the module that contains the source gate.
// of the connection. Those who wish to listen on the target gate of the
// connection being connected/disconnected should add the listener to the
// parent module (as notifications propagate up).
//
struct cPreGateConnectNotification extends cModelChangeNotification
{
    cGate *gate;
    cGate *targetGate;
    cChannel *channel;
};

//
// XXX improve docu
// This notification announces the connection is between gate and gate->getNextGate().
// This notification is fired on the module that contains the source gate.
// of the connection. Those who wish to listen on the target gate of the
// connection being connected/disconnected should add the listener to the
// parent module (as notifications propagate up).
//
struct cPostGateConnectNotification extends cModelChangeNotification
{
    cGate *gate;
};

//
// XXX improve docu
// This notification announces the deletion of the connection between gate
// and gate->getNextGate().
// This notification is fired on the module that contains the source gate.
// of the connection. Those who wish to listen on the target gate of the
// connection being connected/disconnected should add the listener to the
// parent module (as notifications propagate up).
//
struct cPreGateDisconnectNotification extends cModelChangeNotification
{
    cGate *gate;
};

//
// XXX improve docu
// This notification announces the deletion of the connection between gate
// and gate->getNextGate().
// This notification is fired on the module that contains the source gate.
// of the connection. Those who wish to listen on the target gate of the
// connection being connected/disconnected should add the listener to the
// parent module (as notifications propagate up).
//
struct cPostGateDisconnectNotification extends cModelChangeNotification
{
    cGate *gate;
    cGate *targetGate;
    cChannel *channel; // points to  valid object, but will be deleted once the notification has finished
};

//
// Base class for path change notifications. Like xxx, they are fired when a gate
// gets connected/disconnected; the difference is that path change notifications
// are fired on the owner module of pathStartGate and pathEndGate, not on the
// module of the gate being connected/disconnected.
//
// Purpose: make it possible to get away with only local listeners
// in simple modules. If this notification didn't exist, users would have
// to listen for gate connect/disconnect notifications at the toplevel
// module, which is not very efficient (as ALL pre/post model change
// events from all module would then be propagated up to the top).
//
struct cPathChangeNotification extends cModelChangeNotification
{
    cGate *pathStartGate;
    cGate *pathEndGate;
    cGate *changedGate; // the gate that got connected or disconnected
};

//
// XXX
//
struct cPrePathCreateNotification extends cPathChangeNotification { };

//
// XXX
//
struct cPostPathCreateNotification extends cPathChangeNotification { };

//
// XXX
//
struct cPrePathCutNotification extends cPathChangeNotification { };

//
// XXX
//
struct cPostPathCutNotification extends cPathChangeNotification { };

//
// This notification gets fired before a module or channel parameter value was
// changed.
//
struct cPreParameterChangeNotification extends cModelChangeNotification
{
    cPar *par;
};

//
// This notification gets fired after a module or channel parameter value was
// changed.
//
struct cPostParameterChangeNotification extends cModelChangeNotification
{
    cPar *par;
};

//
// This notification gets fired before a display string gets changed.
//
struct cPreDisplayStringChangeNotification extends cModelChangeNotification
{
    cDisplayString *displayString;
};

//
// This notification gets fired after a display string gets changed.
//
struct cPostDisplayStringChangeNotification extends cModelChangeNotification
{
    cDisplayString *displayString;
};

