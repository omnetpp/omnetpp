//==========================================================================
//  CMODELCHANGE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMODELCHANGE_H
#define __CMODELCHANGE_H

#include "cobject.h"

NAMESPACE_BEGIN

/**
 * A signal which is fired before simulation model changes such as module
 * creation or connection creation. The signals carry data objects that
 * describe the type and details the change. The data objects are subclassed
 * from cModelChangeNotification, and begin with the prefix "cPre".
 *
 * These classes include:
 *    - cPreModuleAddNotification,
 *    - cPreModuleDeleteNotification,
 *    - cPreModuleReparentNotification,
 *    - cPreGateAddNotification,
 *    - cPreGateDeleteNotification,
 *    - cPreGateVectorResizeNotification,
 *    - cPreGateConnectNotification,
 *    - cPreGateDisconnectNotification,
 *    - cPrePathCreateNotification,
 *    - cPrePathCutNotification,
 *    - cPreParameterChangeNotification,
 *    - cPreDisplayStringChangeNotification
 *
 * In the listener, use dynamic_cast<> to figure out what notification arrived.
 *
 * This signal is fired on the module or channel affected by the change,
 * and NOT on the module which executes the code that causes the change.
 * For example, preModuleDeleted is fired on the module about to be removed,
 * and not on the module that contains the deleteModule() call.
 *
 * See also: cComponent::emit(), cComponent::subscribe()
 *
 * @ingroup Signals
 */
#define PRE_MODEL_CHANGE   ((simsignal_t)0)

/**
 * A signal which is fired after simulation model changes such as module
 * creation or connection creation. The signals carry data objects that
 * describe the type and details the change. The data objects are subclassed
 * from cModelChangeNotification, and begin with the prefix "cPost".
 *
 * These classes include:
 *    - cPostModuleAddNotification,
 *    - cPostModuleDeleteNotification,
 *    - cPostModuleReparentNotification,
 *    - cPostGateAddNotification,
 *    - cPostGateDeleteNotification,
 *    - cPostGateVectorResizeNotification,
 *    - cPostGateConnectNotification,
 *    - cPostGateDisconnectNotification,
 *    - cPostPathCreateNotification,
 *    - cPostPathCutNotification,
 *    - cPostParameterChangeNotification,
 *    - cPostDisplayStringChangeNotification,
 *    - cPostModuleAddNotification,
 *    - cPostModuleDeleteNotification,
 *    - cPostModuleReparentNotification,
 *    - cPostGateAddNotification,
 *    - cPostGateDeleteNotification,
 *    - cPostGateVectorResizeNotification,
 *    - cPostGateConnectNotification,
 *    - cPostGateDisconnectNotification,
 *    - cPostPathCreateNotification,
 *    - cPostPathCutNotification,
 *    - cPostParameterChangeNotification,
 *    - cPostDisplayStringChangeNotification
 *
 * In the listener, use dynamic_cast<> to figure out what notification arrived.
 *
 * This signal is fired on the module or channel affected by the change,
 * and NOT on the module which executes the code that causes the change.
 * For example, postModuleDeleted is fired on the deleted module's parent
 * module (because the original module no longer exists), and not on the
 * module that contained the deleteModule() call.
 *
 * See also: cComponent::emit(), cComponent::subscribe()
 *
 * @ingroup Signals
 */
#define POST_MODEL_CHANGE  ((simsignal_t)1)


/**
 * Common base class for data objects that accompany PRE_MODEL_CHANGE
 * and POST_MODEL_CHANGE notifications (signals).
 *
 * @ingroup Signals
 */
class SIM_API cModelChangeNotification : public cObject, noncopyable
{
};

/**
 * Fired at the top of cModuleType::create(); fields contain the cModuleType
 * object, and the arguments of the create() method call.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleAddNotification : public cModelChangeNotification
{
  public:
    cModuleType *moduleType;
    const char *moduleName;
    cModule *parentModule;
    int vectorSize;
    int index;
};

/**
 * Fired at the end of cModuleType::create(); at that stage the module is
 * already created, its gates and parameters are added and it is inserted
 * into the model, but it is not yet initialized nor its submodules
 * are created yet.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;
};

/**
 * Fired at the top of cModule::deleteModule(). The module still exists
 * at this point. Note that this notification also gets fired when the
 * network is being deleted after simulation completion.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;
};

/**
 * Fired at the end of cModule::deleteModule(). The module object no longer
 * exists at this point, and its submodules have also been deleted.
 * Fields include properties of the deleted module. It also includes the
 * module pointer (in case it serves as a key in some user data structure),
 * but it must NOT be dereferenced because it points to a deleted object.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;  // this is the pointer of the already deleted module -- DO NOT DEREFERENCE
    int moduleId;
    cModuleType *moduleType;
    const char *moduleName;
    cModule *parentModule;
    int vectorSize;
    int index;
};

/**
 * Fired at the top of cModule::changeParentTo(), before any changes have
 * been done.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleReparentNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    cModule *newParentModule;
};

/**
 * Fired at the end of cModule::changeParentTo().
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleReparentNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    cModule *oldParentModule;
};

/**
 * This gets fired at the top of cModule::addGate(), that is, when a gate
 * or gate vector is added to the module.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields in this class carry the module object on which the gate or gate vector
 * being created, and the arguments of the addGate() method call.
 *
 * @ingroup Signals
 */
class SIM_API cPreGateAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
    cGate::Type gateType;
    bool isVector;
};

/**
 * This gets fired at the bottom of cModule::addGate(), that is, when a gate
 * or gate vector was added to the module.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields in this class carry the module object on which the gate or gate vector
 * was created, and the name of the gate or gate vector.
 *
 * @ingroup Signals
 */
class SIM_API cPostGateAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
};

/**
 * Fired at the top of cModule::deleteGate(). The gate or gate vector still
 * exists at this point.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * @ingroup Signals
 */
class SIM_API cPreGateDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
};

/**
 * Fired at the end of cModule::deleteGate(). The gate or gate vector
 * no longer exists at this point.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields include properties of the deleted gate or gate vector.
 *
 * @ingroup Signals
 */
class SIM_API cPostGateDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
    cGate::Type gateType;
    bool isVector;
    int vectorSize;
};

/**
 * Fired at the top of cModule::setGateSize(). Note that other cModule methods
 * used for implementing the NED "gate++" syntax also expand the gate vector,
 * and fire this notification. These methods are getOrCreateFirstUnconnectedGate()
 * and getOrCreateFirstUnconnectedGatePair()).
 *
 * @ingroup Signals
 */
class SIM_API cPreGateVectorResizeNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
    int newSize;
};

/**
 * Fired at the end of cModule::setGateSize(). Note that other cModule methods
 * used for implementing the NED "gate++" syntax also expand the gate vector,
 * and fire this notification. These methods are getOrCreateFirstUnconnectedGate()
 * and getOrCreateFirstUnconnectedGatePair()).
 *
 * @ingroup Signals
 */
class SIM_API cPostGateVectorResizeNotification : public cModelChangeNotification
{
  public:
    cModule *module;
    const char *gateName;
    int oldSize;
};

/**
 * This notification is fired at the top of cGate::connectTo().
 * This notification is fired on the module that contains the source gate.
 * of the connection. If you wish to listen on the target gate of the
 * connection being connected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * @see cPrePathCreateNotification
 * @ingroup Signals
 */
class SIM_API cPreGateConnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;
    cGate *targetGate;
    cChannel *channel;
};

/**
 * This notification is fired at the end of cGate::connectTo(), to announce that
 * a connection between the given gate and its peer (gate->getNextGate())
 * has been created.
 * This notification is fired on the module that contains the source gate.
 * of the connection. If you wish to listen on the target gate of the
 * connection being connected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * @see cPostPathCreateNotification
 */
class SIM_API cPostGateConnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;
};

/**
 * This notification is fired at the top of cGate::disconnect(), to announce
 * that the connection between the given gate and its peer (gate->getNextGate())
 * is about to be deleted.
 * This notification is fired on the module that contains the source gate.
 * of the connection. If you wish to listen on the target gate of the
 * connection being disconnected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * @see cPrePathCutNotification
 * @ingroup Signals
 */
class SIM_API cPreGateDisconnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;
};

/**
 * This notification is fired at the end of cGate::disconnect(), to announce
 * that the connection between the given gates has been deleted.
 * This notification is fired on the module that contains the source gate.
 * of the connection. If you wish to listen on the target gate of the
 * connection being disconnected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * @see cPostPathCutNotification
 * @ingroup Signals
 */
class SIM_API cPostGateDisconnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;
    cGate *targetGate;
    cChannel *channel; // points to  valid object, but will be deleted once the notification has finished
};

/**
 * Base class for path change notifications. Like gate connect/disconnect
 * notifications, they are fired when a gate gets connected/disconnected;
 * the difference is that path change notifications are fired on the owner
 * modules of the start AND end gates of the path that contains the connection
 * (two notifications!), NOT on the module of the gate being connected or
 * disconnected. See also cGate's getPathStartGate() and getPathEndGate()
 * methods.
 *
 * The purpose of this notification is to make it possible to get away with
 * only local listeners in simple modules. If this notification didn't exist,
 * users would have to listen for gate connect/disconnect notifications at the
 * top-level module, which is not very efficient (as ALL pre/post model change
 * events from all modules would then have to be propagated up to the top).
 *
 * @ingroup Signals
 */
class SIM_API cPathChangeNotification : public cModelChangeNotification
{
  public:
    cGate *pathStartGate;
    cGate *pathEndGate;
    cGate *changedGate; // the gate that got connected or disconnected
};

/**
 * This notification is fired at the top of cGate::connectTo() on the owner
 * modules of the start AND end gates of the future connection path that will
 * be created when the gate gets connected.
 * See cPathChangeNotification for more details.
 *
 * @ingroup Signals
 */
class SIM_API cPrePathCreateNotification : public cPathChangeNotification { };

/**
 * This notification is fired at the end of cGate::connectTo() on the owner
 * modules of the start AND end gates of the connection path that was formed
 * when the gate was connected.
 * See cPathChangeNotification for more details.
 *
 * @ingroup Signals
 */
class SIM_API cPostPathCreateNotification : public cPathChangeNotification { };

/**
 * This notification is fired at the top of cGate::disconnect() on the owner
 * modules of the start AND end gates of the connection path that is about to
 * be cut when the gate gets connected.
 * See cPathChangeNotification for more details.
 *
 * @ingroup Signals
 */
class SIM_API cPrePathCutNotification : public cPathChangeNotification { };

/**
 * This notification is fired at the end of cGate::disconnect() on the owner
 * modules of the start AND end gates of the connection path that was cut when
 * the gate got disconnected.
 * See cPathChangeNotification for more details.
 *
 * @ingroup Signals
 */
class SIM_API cPostPathCutNotification : public cPathChangeNotification { };

/**
 * This notification gets fired before a module or channel parameter value was
 * changed.
 *
 * @ingroup Signals
 */
class SIM_API cPreParameterChangeNotification : public cModelChangeNotification
{
  public:
    cPar *par;
};

/**
 * This notification gets fired after a module or channel parameter value was
 * changed.
 *
 * @ingroup Signals
 */
class SIM_API cPostParameterChangeNotification : public cModelChangeNotification
{
  public:
    cPar *par;
};

/**
 * This notification gets fired before a display string gets changed.
 *
 * @ingroup Signals
 */
class SIM_API cPreDisplayStringChangeNotification : public cModelChangeNotification
{
  public:
    cDisplayString *displayString;
};

/**
 * This notification gets fired after a display string gets changed.
 *
 * @ingroup Signals
 */
class SIM_API cPostDisplayStringChangeNotification : public cModelChangeNotification
{
  public:
    cDisplayString *displayString;
};

NAMESPACE_END

#endif

