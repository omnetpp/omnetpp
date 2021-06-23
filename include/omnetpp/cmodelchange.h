//==========================================================================
//  CMODELCHANGE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMODELCHANGE_H
#define __OMNETPP_CMODELCHANGE_H

#include "cobject.h"

namespace omnetpp {

/**
 * @brief A signal which is fired before simulation model changes such as module
 * creation or connection creation.
 *
 * The signals carry data objects that describe the type and details the change.
 * The data objects are subclassed from cModelChangeNotification, and begin with
 * the prefix "cPre".
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
SIM_API extern simsignal_t PRE_MODEL_CHANGE;

/**
 * @brief A signal which is fired after simulation model changes such as
 * module creation or connection creation.
 *
 * The signals carry data objects that describe the type and details the change.
 * The data objects are subclassed from cModelChangeNotification, and begin
 * with the prefix "cPost".
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
SIM_API extern simsignal_t POST_MODEL_CHANGE;

/**
 * @brief Common base class for data objects that accompany PRE_MODEL_CHANGE
 * and POST_MODEL_CHANGE signals.
 *
 * @ingroup Signals
 */
class SIM_API cModelChangeNotification : public cObject, noncopyable
{
};

/**
 * @brief Model change notification fired just before a module is inserted
 * into the network.
 *
 * This notification is fired in cModuleType::create(), after the module has
 * been created but not yet inserted into the network.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The new module
    cModule *parentModule;    ///< Parent module it is being inserted into
};

/**
 * @brief Model change notification fired just after a module is inserted
 * into the network.
 *
 * This notification is fired at the end of cModuleType::create(). At that
 * stage, the module is already created, its gates and parameters are added
 * and it is inserted into the model, but it is not yet initialized nor its
 * submodules are created yet.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The new module
};

/**
 * @brief Model change notification fired after a module is built.
 *
 * This notification is fired at the end of cModule::buildInside(), when
 * the module and its submodules have been full created and connected,
 * but they have not been initialized yet. When a compound module is created,
 * modules are reported to have been built in bottom-up order.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleBuildNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module
};

/**
 * @brief Model change notification fired after a component is initialized.
 *
 * This notification is fired when the module or channel has completed the
 * last stage of initialization. When a compound module is created,
 * modules will be reported to have been initialized in bottom-up order.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostComponentInitializeNotification : public cModelChangeNotification
{
  public:
    cComponent *component;    ///< The component which was initialized
};

/**
 * @brief Model change notification fired just before a module is deleted.
 *
 * This notification is fired at the top of cModule::deleteModule(),
 * when the module still exists.
 *
 * Note that this notification is also fired when the network is being
 * deleted after simulation completion.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module that is about to be deleted
};

/**
 * @brief Model change notification fired after a module has been deleted.
 *
 * This notification is fired at the end of cModule::deleteModule(). The
 * module object no longer exists at this point, and its submodules have
 * also been deleted. Fields include properties of the deleted module.
 * It also includes the module pointer (in case it serves as a key in
 * some user data structure), but it must NOT be dereferenced because it
 * points to a deleted object.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< Pointer of the deleted module. The module object has already been deleted at this point, and the pointer invalid -- you SHOULD NOT DEREFERENCE it in any way.
    int moduleId;             ///< The ID of the deleted module
    cModuleType *moduleType;  ///< Type of the deleted module
    const char *moduleName;   ///< Name of the deleted module
    cModule *parentModule;    ///< Parent module of the deleted module
    int vectorSize;           ///< Size of the module vector that contained the deleted module; -1 if not a vector
    int index;                ///< Index of the deleted in its vector; -1 if not part of a module vector
};

/**
 * @brief Model change notification fired just before a module is reparented.
 *
 * This notification is fired at the top of cModule::changeParentTo(),
 * before any changes have been done.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreModuleReparentNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module which is about to be reparented
    cModule *newParentModule; ///< The new parent for the module
};

/**
 * @brief Model change notification fired after a module has been reparented.
 *
 * This notification is fired at the end of cModule::changeParentTo().
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostModuleReparentNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The reparented module
    cModule *oldParentModule; ///< The old parent for the module
};

/**
 * @brief Model change notification fired just before a gate or gate vector
 * is added to the module.
 *
 * This notification is fired at the top of cModule::addGate().
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields in this class carry the module object on which the gate or gate vector
 * being created, and the arguments of the addGate() method call.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreGateAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module to which the new gate or gate vector will be added
    const char *gateName;     ///< Name of the new gate or gate vector
    cGate::Type gateType;     ///< Type of the new gate or gate vector (INPUT, OUTPUT or INOUT)
    bool isVector;            ///< Whether a new gate or a gate vector will be added
    int size;                 ///< If isVector=true: The gate vector size
};

/**
 * @brief Model change notification fired after a gate or gate vector has been
 * added to the module.
 *
 * This notification is fired at the bottom of cModule::addGate().
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields in this class carry the module object on which the gate or gate vector
 * was created, and the name of the gate or gate vector.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostGateAddNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module to which the new gate or gate vector was added
    const char *gateName;     ///< Name of the new gate or gate vector
};

/**
 * @brief Model change notification fired just before a gate is deleted.
 *
 * This notification is fired at the top of cModule::deleteGate(). The gate
 * or gate vector in question still exists at this point.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreGateDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< Parent of the gate or gate vector
    const char *gateName;     ///< Name of the gate or gate vector
};

/**
 * @brief Model change notification fired after a gate has been deleted.
 *
 * This notification is fired at the end of cModule::deleteGate(). The gate
 * or gate vector no longer exists at this point.
 *
 * Note: this notification is fired for the gate or gate vector as a
 * whole, and not for individual gate objects in it. That is, a single
 * notification is fired for an inout gate (which is a gate pair) and
 * for gate vectors as well.
 *
 * Fields include properties of the deleted gate or gate vector.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostGateDeleteNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< Parent of the gate or gate vector
    const char *gateName;     ///< Name of the gate or gate vector
    cGate::Type gateType;     ///< Type of the gate or gate vector
    bool isVector;            ///< Name of the gate vector that was deleted
    int vectorSize;           ///< If a gate vector was deleted: size of the vector
};

/**
 * @brief Model change notification fired just before a gate vector is resized.
 *
 * This notification is fired at the top of cModule::setGateSize(). Note that
 * other cModule methods used for implementing the NED "gate++" syntax also
 * expand the gate vector, and fire this notification. These methods are
 * getOrCreateFirstUnconnectedGate() and getOrCreateFirstUnconnectedGatePair()).
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreGateVectorResizeNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module of the gate vector
    const char *gateName;     ///< The name of the gate vector
    int newSize;              ///< The new size of the gate vector
};

/**
 * @brief Model change notification fired after a gate vector has been resized.
 *
 * This notification is fired at the bottom of cModule::setGateSize(). Note that
 * other cModule methods used for implementing the NED "gate++" syntax also
 * expand the gate vector, and fire this notification. These methods are
 * getOrCreateFirstUnconnectedGate() and getOrCreateFirstUnconnectedGatePair()).
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostGateVectorResizeNotification : public cModelChangeNotification
{
  public:
    cModule *module;          ///< The module of the gate vector
    const char *gateName;     ///< The name of the gate vector
    int oldSize;              ///< The old size of the gate vector
};

/**
 * @brief Model change notification fired just before a gate is connected.
 *
 * This notification is fired at the top of cGate::connectTo(), on the
 * module that contains the connection's source gate. If you wish to listen
 * on the target gate of the connection being connected, you should add the
 * listener to the parent module (as notifications propagate up).
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @see cPrePathCreateNotification
 * @ingroup Signals
 */
class SIM_API cPreGateConnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;              ///< The gate that is about to be connected
    cGate *targetGate;        ///< The gate it will be connected to
    cChannel *channel;        ///< The channel object to be associated with the connection
};

/**
 * @brief Model change notification fired after a gate has been connected.
 *
 * This notification is fired at the end of cGate::connectTo(), to announce
 * that a connection between the given gate and its peer (gate->getNextGate())
 * has been created.
 *
 * This notification is fired on the module that contains the source gate.
 * of the connection. If you wish to listen on the target gate of the
 * connection being connected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @see cPostPathCreateNotification
 */
class SIM_API cPostGateConnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;              ///< The gate that has been connected
};

/**
 * @brief Model change notification fired just before a gate has been
 * disconnected.
 *
 * This notification is fired at the top of cGate::disconnect(), to announce
 * that the connection between the given gate and its peer (gate->getNextGate())
 * is about to be deleted. It is fired on the module that contains the source
 * gate of the connection. If you wish to listen on the target gate of the
 * connection being disconnected, you should add the listener to the
 * parent module (as notifications propagate up).
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @see cPrePathCutNotification
 * @ingroup Signals
 */
class SIM_API cPreGateDisconnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;              ///< The gate that is about to be disconnected
};

/**
 * @brief Model change notification fired after a gate is disconnected.
 *
 * This notification is fired at the end of cGate::disconnect(), to announce
 * that the connection between the given gates has been deleted. It is fired
 * on the module that contains the source gate of the connection. If you wish
 * to listen on the target gate of the connection being disconnected, you
 * should add the listener to the parent module (as notifications propagate up).
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @see cPostPathCutNotification
 * @ingroup Signals
 */
class SIM_API cPostGateDisconnectNotification : public cModelChangeNotification
{
  public:
    cGate *gate;              ///< The gate that has been disconnected
    cGate *targetGate;        ///< The gate to which it was connected
    cChannel *channel;        ///< The channel object associated with the link; it points to valid object that will be deleted once the notification has finished
};

/**
 * @brief Base class for path change notifications.
 *
 * Like gate connect/disconnect notifications, path change notifications
 * are fired when a gate is connected or disconnected. The difference is
 * that path change notifications are fired on BOTH of the owner modules of
 * the start and end gates of the path that contains the connection
 * (two notifications!), NOT on the module of the gate being connected or
 * disconnected. See also cGate's getPathStartGate() and getPathEndGate()
 * methods.
 *
 * The purpose of this notification is to make it possible to get away with
 * only local listeners in simple modules. If this notification didn't exist,
 * users would have to listen for gate connect/disconnect notifications at the
 * top-level module, which is not very efficient (as all pre/post model change
 * events from all modules would then have to be propagated up to the top).
 *
 * @ingroup Signals
 */
class SIM_API cPathChangeNotification : public cModelChangeNotification
{
  public:
    cGate *pathStartGate;     ///< The start gate of the path
    cGate *pathEndGate;       ///< The end gate of the path
    cGate *changedGate;       ///< The gate whose connection has changed
};

/**
 * @brief Model change notification fired just before a connection path is created.
 *
 * This notification is fired at the top of cGate::connectTo() on the owner
 * modules of the start AND end gates of the future connection path that will
 * be created when the gate gets connected.
 * See cPathChangeNotification for more details.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPrePathCreateNotification : public cPathChangeNotification { };

/**
 * @brief Model change notification fired after a connection path has been created.
 *
 * This notification is fired at the end of cGate::connectTo() on the owner
 * modules of the start AND end gates of the connection path that was formed
 * when the gate was connected.
 * See cPathChangeNotification for more details.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostPathCreateNotification : public cPathChangeNotification { };

/**
 * @brief Model change notification fired just before a connection path is cut.
 *
 * This notification is fired at the top of cGate::disconnect() on the owner
 * modules of the start AND end gates of the connection path that is about to
 * be cut when the gate gets connected.
 * See cPathChangeNotification for more details.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPrePathCutNotification : public cPathChangeNotification { };

/**
 * @brief Model change notification fired after a connection path has been cut.
 *
 * This notification is fired at the end of cGate::disconnect() on the owner
 * modules of the start AND end gates of the connection path that was cut when
 * the gate got disconnected.
 * See cPathChangeNotification for more details.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostPathCutNotification : public cPathChangeNotification { };

/**
 * @brief Model change notification fired just before a parameter change.
 *
 * This notification is fired before a module or channel parameter value was
 * changed.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreParameterChangeNotification : public cModelChangeNotification
{
  public:
    cPar *par;                ///< The module parameter that is about to be changed
};

/**
 * @brief Model change notification fired after a parameter change.
 *
 * This notification is fired after a module or channel parameter value was
 * changed.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostParameterChangeNotification : public cModelChangeNotification
{
  public:
    cPar *par;                ///< The module parameter that has changed
};

/**
 * @brief Model change notification fired just before a display string change.
 *
 * This notification is fired before a display string gets changed.
 *
 * This object accompanies the PRE_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPreDisplayStringChangeNotification : public cModelChangeNotification
{
  public:
    cDisplayString *displayString;  ///< The display string that is about to be updated
};

/**
 * @brief Model change notification fired after a dispay string change.
 *
 * This notification is fired after a display string gets changed.
 *
 * This object accompanies the POST_MODEL_CHANGE signal.
 *
 * @ingroup Signals
 */
class SIM_API cPostDisplayStringChangeNotification : public cModelChangeNotification
{
  public:
    cDisplayString *displayString;  ///< The display string that was updated
};

}  // namespace omnetpp

#endif

