//==========================================================================
//   CMESSAGE.H  -  header for
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

#ifndef __OMNETPP_CMESSAGE_H
#define __OMNETPP_CMESSAGE_H

#include <vector>
#include "cevent.h"
#include "carray.h"
#include "cmsgpar.h"
#include "csimulation.h"

namespace omnetpp {

class cMsgPar;
class cGate;
class cChannel;
class cModule;
class cSimpleModule;
class cSimulation;

/**
 * @brief Predefined message kind values (values for cMessage's getKind(),
 * setKind() methods).
 *
 * Negative values are reserved for the \opp system and its
 * standard libraries. Zero and positive values can be freely used
 * by simulation models.
 */
enum eMessageKind
{
  MK_STARTER = -1,  ///< Starter message. Used by scheduleStart().
  MK_TIMEOUT = -2,  ///< Internal timeout message. Used by wait(), etc.
  MK_PARSIM_BEGIN = -1000  ///< values -1000...-2000 reserved for parallel simulation
};

/**
 * @brief Maximum number of partitions for parallel simulation.
 *
 * @ingroup ParsimBrief
 * @ingroup Parsim
 */
// Note: it cannot go to cparsimcomm.h, without causing unwanted dependency on sim/parsim
#define MAX_PARSIM_PARTITIONS  32768 // srcprocid in cMessage


/**
 * @brief The message class in \opp. cMessage objects may represent events,
 * messages, jobs or other entities in a simulation. To represent network
 * packets, use the cPacket subclass.
 *
 * Messages may be scheduled (to arrive back at the same module at a later
 * time), cancelled, sent out on a gate, or sent directly to another module;
 * all via methods of cSimpleModule.
 *
 * cMessage can be assigned a name (a property inherited from cNamedObject);
 * other attributes include message kind, priority, and time stamp.
 * Messages may be cloned with the dup() function. The control info field
 * facilitates modelling communication between protocol layers. The context
 * pointer field makes it easier to work with several timers (self-messages)
 * at a time. A message also stores information about its last sending,
 * including sending time, arrival time, arrival module and gate.
 *
 * Useful methods are isSelfMessage(), which tells apart self-messages from
 * messages received from other modules, and isScheduled(), which returns
 * whether a self-message is currently scheduled.
 *
 * Further fields can be added to cMessage via message declaration files (.msg)
 * which are translated into C++ classes. An example message declaration:
 *
 * ```
 * message Job
 * {
 *        string label;
 *        int color = -1;
 * }
 * ```
 *
 * @see cSimpleModule, cQueue, cPacket
 *
 * @ingroup SimProgr
 */
class SIM_API cMessage : public cEvent
{
  private:
    enum {
        FL_ISPRIVATECOPY = 4,
    };
    // note: fields are in an order that maximizes packing (minimizes sizeof(cMessage))
    short messageKind;              // message kind -- 0>= user-defined meaning, <0 reserved
    short srcProcId = -1;           // reserved for use by parallel execution: id of source partition
    cArray *parList = nullptr;      // ptr to list of parameters
    cObject *controlInfo = nullptr; // ptr to "control info"
    void *contextPointer = nullptr; // a stored pointer -- user-defined meaning, used with self-messages

    int senderModuleId = -1;   // sender module ID -- set internally
    int senderGateId = -1;     // source gate ID -- set internally
    int targetModuleId = -1;   // destination module ID -- set internally
    int targetGateId = -1;     // destination gate ID -- set internally
    simtime_t creationTime;    // creation time -- set be constructor
    simtime_t sendTime = 0;    // time of sending -- set internally
    simtime_t timestamp = 0;   // time stamp -- user-defined meaning

    msgid_t messageId;        // a unique message identifier assigned upon message creation
    msgid_t messageTreeId;    // a message identifier that is inherited by dup, if non dupped it is msgid
    static msgid_t nextMessageId; // the next unique message identifier to be assigned upon message creation

    // global variables for statistics
    static uint64_t totalMsgCount;
    static uint64_t liveMsgCount;

  private:
    // internal: create parlist
    void _createparlist();

    void copy(const cMessage& msg);

    // internal: used by LogBuffer for creating an *exact* copy of a message
    void setId(msgid_t id) {messageId = id;}

  public:
    // internal: create an exact clone (including msgid) that doesn't show up in the statistics
    cMessage *privateDup() const;

    // internal: called by the simulation kernel as part of the send(),
    // scheduleAt() calls to set the values returned by the
    // getSenderModuleId(), getSenderGate(), getSendingTime() methods.
    void setSentFrom(cModule *module, int gateId, simtime_t_cref t);

    // internal: used by the parallel simulation kernel.
    void setSrcProcId(int procId) {srcProcId = (short)procId;}

    // internal: used by the parallel simulation kernel.
    virtual int getSrcProcId() const override {return srcProcId;}

    // internal: returns the parameter list object, or nullptr if it hasn't been used yet
    cArray *getParListPtr()  {return parList;}

  protected: // hide cEvent methods from the cMessage API

    // overridden from cEvent: return true
    virtual bool isMessage() const override {return true;}

    // overridden from cEvent: return true of the target module is still alive and well
    virtual bool isStale() override;

    // overridden from cEvent: return the arrival module
    virtual cObject *getTargetObject() const override;

    // overridden from cEvent
    virtual void execute() override;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cMessage(const cMessage& msg);

    /**
     * Constructor.
     */
    explicit cMessage(const char *name=nullptr, short kind=0);

    /**
     * Destructor.
     */
    virtual ~cMessage();

    /**
     * Assignment operator. The data members NOT copied are: object name
     * (see cNamedObject::operator=() for more details) and message ID.
     * All other members, including creation time and message tree ID,
     * are copied.
     */
    cMessage& operator=(const cMessage& msg);
    //@}

    /**
     * Returns true if the current class is a subclass of cPacket.
     * The cMessage implementation returns false.
     */
    virtual bool isPacket() const override {return false;}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object, except for the
     * message ID (the clone is assigned a new ID). Note that the message
     * creation time is also copied, so clones of the same message object
     * have the same creation time. See cObject for more details.
     */
    virtual cMessage *dup() const override  {return new cMessage(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;

    /**
     * Serializes the object into an MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Message attributes. */
    //@{
    /**
     * Sets the message kind. Nonnegative values can be freely used by
     * the user; negative values are reserved by OMNeT++ for internal
     * purposes.
     */
    void setKind(short k)  {messageKind=k;}

    /**
     * Sets the message's time stamp to the current simulation time.
     */
    void setTimestamp() {timestamp=getSimulation()->getSimTime();}

    /**
     * Directly sets the message's time stamp.
     */
    void setTimestamp(simtime_t t) {timestamp=t;}

    /**
     * Sets the context pointer. This pointer may store an arbitrary value.
     * It is useful when managing several timers (self-messages): when
     * scheduling the message one can set the context pointer to the data
     * structure the timer corresponds to (e.g. the buffer whose timeout
     * the message represents), so that when the self-message arrives it is
     * easier to identify where it belongs.
     */
    void setContextPointer(void *p) {contextPointer=p;}

    /**
     * Attaches a "control info" structure (object) to the message.
     * This is most useful when passing packets between protocol layers
     * of a protocol stack: e.g. when sending down an IP datagram to Ethernet,
     * the attached "control info" can contain the destination MAC address.
     *
     * The "control info" object will be deleted when the message is deleted.
     * Only one "control info" structure can be attached (the second
     * setControlInfo() call throws an error).
     *
     * When the message is duplicated or copied, copies will have their
     * control info set to nullptr because the cObject interface
     * does not define dup/copy operations.
     * The assignment operator does not change control info.
     */
    virtual void setControlInfo(cObject *p);

    /**
     * Removes the "control info" structure (object) from the message
     * and returns its pointer. Returns nullptr if there was no control info
     * in the message.
     */
    cObject *removeControlInfo();

    /**
     * Returns the message kind.
     */
    short getKind() const  {return messageKind;}

    /**
     * Returns the message's time stamp.
     */
    simtime_t_cref getTimestamp() const {return timestamp;}

    /**
     * Returns the context pointer.
     */
    void *getContextPointer() const {return contextPointer;}

    /**
     * Returns pointer to the attached "control info".
     */
    cObject *getControlInfo() const {return controlInfo;}
    //@}

    /** @name Dynamically attaching objects. */
    //@{

    /**
     * Returns reference to the 'object list' of the message: a cArray
     * which is used to store parameter (cMsgPar) objects and other objects
     * attached to the message.
     *
     * One can use either getParList() combined with cArray methods,
     * or several convenience methods (addPar(), addObject(), par(), etc.)
     * to add, retrieve or remove cMsgPars and other objects.
     *
     * Note: Using the object list has alternatives which may better
     * suit your needs. For more information, see class description for discussion
     * about message subclassing vs dynamically attached objects.
     */
    virtual cArray& getParList()  {if (!parList) _createparlist(); return *parList;}

    /**
     * Add a new, empty parameter (cMsgPar object) with the given name
     * to the message's object list.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cMsgPar& addPar(const char *name)  {cMsgPar *p=new cMsgPar(name);getParList().add(p);return *p;}

    /**
     * Add a parameter object to the message's object list.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cMsgPar& addPar(cMsgPar *par)  {getParList().add(par); return *par;}

    /**
     * Returns the nth object in the message's object list, converting it to a cMsgPar.
     * If the object does not exist or it cannot be cast to cMsgPar (using dynamic_cast\<\>),
     * the method throws a cRuntimeError.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cMsgPar& par(int index);

    /**
     * Returns the object with the given name in the message's object list,
     * converting it to a cMsgPar.
     * If the object does not exist or it cannot be cast to cMsgPar (using dynamic_cast\<\>),
     * the method throws a cRuntimeError.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cMsgPar& par(const char *name);

    /**
     * Returns the index of the parameter with the given name in the message's
     * object list, or -1 if it could not be found.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::find() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual int findPar(const char *name) const;

    /**
     * Check if a parameter with the given name exists in the message's
     * object list.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::exist() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual bool hasPar(const char *name) const {return findPar(name)>=0;}

    /**
     * Add an object to the message's object list.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cObject *addObject(cObject *par)  {getParList().add(par); return par;}

    /**
     * Returns the object with the given name in the message's object list.
     * If the object is not found, it returns nullptr.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cObject *getObject(const char *name)  {return getParList().get(name);}

    /**
     * Check if an object with the given name exists in the message's object list.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::exist() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual bool hasObject(const char *name)  {return !parList ? false : parList->find(name)>=0;}

    /**
     * Remove the object with the given name from the message's object list, and
     * return its pointer. If the object does not exist, nullptr is returned.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::remove() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cObject *removeObject(const char *name)  {return getParList().remove(name);}

    /**
     * Remove the object with the given name from the message's object list, and
     * return its pointer. If the object does not exist, nullptr is returned.
     *
     * Note: This is a convenience function: one may use getParList() and
     * cArray::remove() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.
     *
     * @see getParList()
     */
    virtual cObject *removeObject(cObject *par)  {return getParList().remove(par);}
    //@}

    /** @name Sending/arrival information. */
    //@{

    /**
     * Return true if message was posted by scheduleAt().
     */
    bool isSelfMessage() const {return targetGateId==-1;}

    /**
     * Returns a pointer to the sender module. It returns nullptr if the message
     * has not been sent/scheduled yet, or if the sender module got deleted
     * in the meantime.
     */
    cModule *getSenderModule() const {return getSimulation()->getModule(senderModuleId);}

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. nullptr is returned for new (unsent) messages
     * and messages sent via scheduleAt().
     */
    cGate *getSenderGate() const;

    /**
     * Returns a pointer to the arrival module. It returns nullptr if the message
     * has not been sent/scheduled yet, or if the module was deleted
     * in the meantime.
     */
    cModule *getArrivalModule() const {return getSimulation()->getModule(targetModuleId);}

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. nullptr is returned for new (unsent) messages
     * and messages sent via scheduleAt().
     */
    cGate *getArrivalGate() const;

    /**
     * Returns the module ID of the sender module, or -1 if the
     * message has not been sent/scheduled yet.
     *
     * @see cModule::getId(), cSimulation::getModule()
     */
    int getSenderModuleId() const {return senderModuleId;}

    /**
     * Returns the gate ID of the gate in the sender module on which the
     * message was sent, or -1 if the message has not been sent/scheduled yet.
     * Note: this is not the same as the gate's index (cGate::getIndex()).
     *
     * @see cGate::getId(), cModule::gate(int)
     */
    int getSenderGateId() const   {return senderGateId;}

    /**
     * Returns the module ID of the receiver module, or -1 if the
     * message has not been sent/scheduled yet.
     *
     * @see cModule::getId(), cSimulation::getModule()
     */
    int getArrivalModuleId() const {return targetModuleId;}

    /**
     * Returns the gate ID of the gate in the receiver module on which the
     * message was received, or -1 if the message has not been sent/scheduled yet.
     * Note: this is not the same as the gate's index (cGate::getIndex()).
     *
     * @see cGate::getId(), cModule::gate(int)
     */
    int getArrivalGateId() const  {return targetGateId;}

    /**
     * Returns time when the message was created; for cloned messages, it
     * returns the creation time of the original message, not the time of the
     * dup() call.
     */
    simtime_t_cref getCreationTime() const {return creationTime;}

    /**
     * Returns time when the message was sent/scheduled or 0 if the message
     * has not been sent yet.
     */
    simtime_t_cref getSendingTime()  const {return sendTime;}

    /**
     * Returns time when the message arrived (or will arrive if it
     * is currently scheduled or is underway), or 0 if the message
     * has not been sent/scheduled yet.
     *
     * If the message is a packet that has nonzero length and traveled through
     * a channel with nonzero data rate, arrival time may represent either
     * the start or the end of the reception, as returned by the
     * isReceptionStart() method. By default it is the end of the reception;
     * it can be changed by calling setDeliverImmediately(true) on the
     * gate at receiving end of the channel that has the nonzero data rate.
     *
     * @see getDuration()
     */
    // note: overridden to provide more specific documentation
    simtime_t_cref getArrivalTime()  const {return arrivalTime;}

    /**
     * Return true if the message arrived through the given gate.
     */
    bool arrivedOn(int gateId) const {return gateId==targetGateId;}

    /**
     * Return true if the message arrived on the gate given with its name.
     * If it is a vector gate, the method returns true if the message arrived
     * on any gate in the vector.
     */
    bool arrivedOn(const char *gateName) const;

    /**
     * Return true if the message arrived through the given gate
     * in the named gate vector.
     */
    bool arrivedOn(const char *gateName, int gateIndex) const;

    /**
     * Returns a unique message identifier assigned upon message creation.
     */
    msgid_t getId() const {return messageId;}

    /**
     * Returns an identifier which is shared among a message object and all messages
     * created by copying it (i.e. by dup() or the copy constructor).
     */
    msgid_t getTreeId() const {return messageTreeId;}
    //@}

    /** @name Miscellaneous. */
    //@{
    /**
     * Override to define a display string for the message. Display string
     * affects message appearance in Qtenv. This default implementation
     * returns "".
     */
    virtual const char *getDisplayString() const;

    /**
     * For use by custom scheduler classes (see cScheduler): set the arrival
     * module and gate for messages inserted into the FES directly by the
     * scheduler. If you pass gateId=-1, the message will arrive as a
     * self-message.
     */
    void setArrival(int moduleId, int gateId) {targetModuleId = moduleId; targetGateId = gateId;}

    /**
     * Like setArrival(int moduleId, int gateId), but also sets the arrival
     * time for the message.
     */
    void setArrival(int moduleId, int gateId, simtime_t_cref t) {targetModuleId = moduleId; targetGateId = gateId; setArrivalTime(t);}
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of messages created since the last
     * reset (reset is usually called by user interfaces at the beginning
     * of each simulation run). The counter is incremented by cMessage constructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static uint64_t getTotalMessageCount() {return totalMsgCount;}

    /**
     * Returns the number of message objects that currently exist in the
     * program. The counter is incremented by cMessage constructor
     * and decremented by the destructor.
     * May be useful for profiling or debugging memory leaks caused by forgetting
     * to delete messages.
     */
    static uint64_t getLiveMessageCount() {return liveMsgCount;}

    /**
     * Reset counters used by getTotalMessageCount() and getLiveMessageCount().
     */
    static void resetMessageCounters()  {totalMsgCount=liveMsgCount=0;}
    //@}
};

}  // namespace omnetpp

#endif


