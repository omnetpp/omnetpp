//==========================================================================
//   CMESSAGE.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cMessage : message and event object
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMESSAGE_H
#define __CMESSAGE_H

#include "cownedobject.h"
#include "carray.h"
#include "cmsgpar.h"
#include "csimulation.h"

NAMESPACE_BEGIN

class cMsgPar;
class cGate;
class cChannel;
class cModule;
class cSimpleModule;
class cSimulation;
class cMessageHeap;
class LogBuffer;


/**
 * Predefined message kind values (values for cMessage's getKind(),
 * setKind() methods).
 *
 * Negative values are reserved for the \opp system and its
 * standard libraries. Zero and positive values can be freely used
 * by simulation models.
 */
enum eMessageKind
{
  MK_STARTER = -1,  /// Starter message. Used by scheduleStart().
  MK_TIMEOUT = -2,  /// Internal timeout message. Used by wait(), etc.
  MK_PACKET  = -3,  /// Obsolete
  MK_INFO    = -4,  /// Obsolete

  MK_PARSIM_BEGIN = -1000  /// values -1000...-2000 reserved for parallel simulation
};

/**
 * Maximum number of partitions for parallel simulation.
 *
 * @ingroup ParsimBrief
 * @ingroup Parsim
 */
// Note: it cannot go to cparsimcomm.h, without causing unwanted dependency on sim/parsim
#define MAX_PARSIM_PARTITIONS  32768 // srcprocid in cMessage


#ifdef WITHOUT_CPACKET
#undef cMessage
#endif

/**
 * The message class in \opp. cMessage objects may represent events,
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
 * \code
 * message Job
 * {
 *    fields:
 *        string label;
 *        int color = -1;
 * }
 * \endcode
 *
 * @see cSimpleModule, cQueue, cPacket
 *
 * @ingroup SimCore
 */
//XXX note: sizeof(cMessage) could be made a little smaller: the fields created, frommod, fromgate are rarely used by models (note: sendtime and tstamp *do* get used in practice)
class SIM_API cMessage : public cOwnedObject
{
    friend class cMessageHeap;
    friend class LogBuffer;  // for setMessageId()

  private:
    // note: fields are in an order that maximizes packing (minimizes sizeof(cMessage))
    short msgkind;             // message kind -- 0>= user-defined meaning, <0 reserved
    short prior;               // priority -- used for scheduling msgs with equal times
    short srcprocid;           // reserved for use by parallel execution: id of source partition
    cArray *parlistp;          // ptr to list of parameters
    cObject *ctrlp;            // ptr to "control info"
    void *contextptr;          // a stored pointer -- user-defined meaning, used with self-messages

    int frommod, fromgate;     // source module and gate IDs -- set internally
    int tomod, togate;         // dest. module and gate IDs -- set internally
    simtime_t created;         // creation time -- set be constructor
    simtime_t sent,delivd;     // time of sending & delivery -- set internally
    simtime_t tstamp;          // time stamp -- user-defined meaning

    int heapindex;             // used by cMessageHeap (-1 if not on heap; all other values, including negative ones, means "on the heap")
    unsigned long insertordr;  // used by cMessageHeap

    eventnumber_t prev_event_num; // event number of the last time envir was notified about this message (e.g. creating/cloning/sending/scheduling/deleting this message)

    long msgid;                // a unique message identifier assigned upon message creation
    long msgtreeid;            // a message identifier that is inherited by dup, if non dupped it is msgid
    static long next_id;       // the next unique message identifier to be assigned upon message creation

    // global variables for statistics
    static long total_msgs;
    static long live_msgs;

  private:
    // internal: create parlist
    void _createparlist();

    void copy(const cMessage& msg);

    // internal: used by LogBuffer for creating an *exact* copy of a message
    void setId(long id) {msgid = id;}

  public:
    // internal: returns the event number which scheduled this event, or the event in which
    // this message was last delivered to a module. Stored for recording into the event log file.
    eventnumber_t getPreviousEventNumber() const {return prev_event_num;}

    // internal: sets previousEventNumber.
    void setPreviousEventNumber(eventnumber_t num) {prev_event_num = num;}

    // internal: used by cMessageHeap.
    unsigned long getInsertOrder() const {return insertordr;}

    // internal: called by the simulation kernel as part of the send(),
    // scheduleAt() calls to set the values returned by the
    // getSenderModuleId(), getSenderGate(), getSendingTime() methods.
    void setSentFrom(cModule *module, int gateId, simtime_t_cref t);

    // internal: called by the simulation kernel as part of processing
    // the send(), scheduleAt() calls to set the values returned
    // by the getArrivalModuleId(), getArrivalGate() methods.
    void setArrival(cModule *module, int gateId);

    // internal: called by the simulation kernel as part of processing
    // the send(), scheduleAt() calls to set the values returned
    // by the getArrivalModuleId(), getArrivalGate(), getArrivalTime() methods.
    void setArrival(cModule *module, int gate, simtime_t_cref t);

    // internal: called by the simulation kernel to set the value returned
    // by the getArrivalTime() method
    void setArrivalTime(simtime_t t);

    // internal: used by the parallel simulation kernel.
    void setSrcProcId(int procId) {srcprocid = (short)procId;}

    // internal: used by the parallel simulation kernel.
    int getSrcProcId() const {return srcprocid;}

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
    explicit cMessage(const char *name=NULL, short kind=0);

    /**
     * Destructor.
     */
    virtual ~cMessage();

    /**
     * Assignment operator. The data members NOT copied are: object name
     * (see cNamedObject's operator=() for more details) and message ID.
     * All other members, including creation time and message tree ID,
     * are copied.
     */
    cMessage& operator=(const cMessage& msg);
    //@}

    /**
     * Returns whether the current class is subclass of cPacket.
     * The cMessage implementation returns false.
     */
    virtual bool isPacket() const {return false;}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object, except for the
     * message ID (the clone is assigned a new ID). Note that the message
     * creation time is also copied, so clones of the same message object
     * have the same creation time. See cObject for more details.
     */
    virtual cMessage *dup() const  {return new cMessage(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Produces a multi-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into an MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Message attributes. */
    //@{

    /**
     * Sets the message kind. Nonnegative values can be freely used by
     * the user; negative values are reserved by OMNeT++ for internal
     * purposes.
     */
    void setKind(short k)  {msgkind=k;}

    /**
     * Sets event scheduling priority of the message. The priority member is
     * used when the simulator inserts messages into the future events set
     * (FES), to order messages with identical arrival time values.
     */
    void setSchedulingPriority(short p)  {prior=p;}

    /**
     * Sets the message's time stamp to the current simulation time.
     */
    void setTimestamp() {tstamp=simulation.getSimTime();}

    /**
     * Directly sets the message's time stamp.
     */
    void setTimestamp(simtime_t t) {tstamp=t;}

    /**
     * Sets the context pointer. This pointer may store an arbitrary value.
     * It is useful when managing several timers (self-messages): when
     * scheduling the message one can set the context pointer to the data
     * structure the timer corresponds to (e.g. the buffer whose timeout
     * the message represents), so that when the self-message arrives it is
     * easier to identify where it belongs.
     */
    void setContextPointer(void *p) {contextptr=p;}

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
     * control info set to NULL because the cObject interface
     * does not define dup/copy operations.
     * The assignment operator does not change control info.
     */
    void setControlInfo(cObject *p);

    /**
     * Removes the "control info" structure (object) from the message
     * and returns its pointer. Returns NULL if there was no control info
     * in the message.
     */
    cObject *removeControlInfo();

    /**
     * Returns the message kind.
     */
    short getKind() const  {return msgkind;}

    /**
     * Returns the event scheduling priority.
     */
    short getSchedulingPriority() const  {return prior;}

    /**
     * Returns the message's time stamp.
     */
    simtime_t_cref getTimestamp() const {return tstamp;}

    /**
     * Returns the context pointer.
     */
    void *getContextPointer() const {return contextptr;}

    /**
     * Returns pointer to the attached "control info".
     */
    cObject *getControlInfo() const {return ctrlp;}
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
     * <i>NOTE: using the object list has alternatives which may better
     * suit your needs. For more information, see class description for discussion
     * about message subclassing vs dynamically attached objects.</i>
     */
    virtual cArray& getParList()  {if (!parlistp) _createparlist(); return *parlistp;}

    /**
     * Add a new, empty parameter (cMsgPar object) with the given name
     * to the message's object list.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cMsgPar& addPar(const char *s)  {cMsgPar *p=new cMsgPar(s);getParList().add(p);return *p;}

    /**
     * Add a parameter object to the message's object list.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cMsgPar& addPar(cMsgPar *p)  {getParList().add(p); return *p;}

    /**
     * DEPRECATED! Use addPar(cMsgPar *p) instead.
     */
    _OPPDEPRECATED cMsgPar& addPar(cMsgPar& p)  {return addPar(&p);}

    /**
     * Returns the nth object in the message's object list, converting it to a cMsgPar.
     * If the object does not exist or it cannot be cast to cMsgPar (using dynamic_cast\<\>),
     * the method throws a cRuntimeError.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cMsgPar& par(int n);

    /**
     * Returns the object with the given name in the message's object list,
     * converting it to a cMsgPar.
     * If the object does not exist or it cannot be cast to cMsgPar (using dynamic_cast\<\>),
     * the method throws a cRuntimeError.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cMsgPar& par(const char *s);

    /**
     * Returns the index of the parameter with the given name in the message's
     * object list, or -1 if it could not be found.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::find() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual int findPar(const char *s) const;

    /**
     * Check if a parameter with the given name exists in the message's
     * object list.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::exist() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual bool hasPar(const char *s) const {return findPar(s)>=0;}

    /**
     * Add an object to the message's object list.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::add() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cObject *addObject(cObject *p)  {getParList().add(p); return p;}

    /**
     * Returns the object with the given name in the message's object list.
     * If the object is not found, it returns NULL.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::get() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cObject *getObject(const char *s)  {return getParList().get(s);}

    /**
     * Check if an object with the given name exists in the message's object list.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::exist() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual bool hasObject(const char *s)  {return !parlistp ? false : parlistp->find(s)>=0;}

    /**
     * Remove the object with the given name from the message's object list, and
     * return its pointer. If the object does not exist, NULL is returned.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::remove() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cObject *removeObject(const char *s)  {return getParList().remove(s);}

    /**
     * Remove the object with the given name from the message's object list, and
     * return its pointer. If the object does not exist, NULL is returned.
     *
     * <i>NOTE: This is a convenience function: one may use getParList() and
     * cArray::remove() instead. See also class description for discussion about
     * message subclassing vs dynamically attached objects.</i>
     *
     * @see getParList()
     */
    virtual cObject *removeObject(cObject *p)  {return getParList().remove(p);}
    //@}

    /** @name Sending/arrival information. */
    //@{

    /**
     * Return true if message was posted by scheduleAt().
     */
    bool isSelfMessage() const {return togate==-1;}

    /**
     * Return true if message is among future events.
     */
    bool isScheduled() const {return heapindex!=-1;}

    /**
     * Returns a pointer to the sender module. It returns NULL if the message
     * has not been sent/scheduled yet, or if the sender module got deleted
     * in the meantime.
     */
    cModule *getSenderModule() const {return simulation.getModule(frommod);}

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *getSenderGate() const;

    /**
     * Returns a pointer to the arrival module. It returns NULL if the message
     * has not been sent/scheduled yet, or if the module was deleted
     * in the meantime.
     */
    cModule *getArrivalModule() const {return simulation.getModule(tomod);}

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *getArrivalGate() const;

    /**
     * Returns the module ID of the sender module, or -1 if the
     * message has not been sent/scheduled yet.
     *
     * @see cModule::getId(), cSimulation::getModule()
     */
    int getSenderModuleId() const {return frommod;}

    /**
     * Returns the gate ID of the gate in the sender module on which the
     * message was sent, or -1 if the message has not been sent/scheduled yet.
     * Note: this is not the same as the gate's index (cGate::getIndex()).
     *
     * @see cGate::getId(), cModule::gate(int)
     */
    int getSenderGateId() const   {return fromgate;}

    /**
     * Returns the module ID of the receiver module, or -1 if the
     * message has not been sent/scheduled yet.
     *
     * @see cModule::getId(), cSimulation::getModule()
     */
    int getArrivalModuleId() const {return tomod;}

    /**
     * Returns the gate ID of the gate in the receiver module on which the
     * message was received, or -1 if the message has not been sent/scheduled yet.
     * Note: this is not the same as the gate's index (cGate::getIndex()).
     *
     * @see cGate::getId(), cModule::gate(int)
     */
    int getArrivalGateId() const  {return togate;}

    /**
     * Returns time when the message was created; for cloned messages, it
     * returns the creation time of the original message, not the time of the
     * dup() call.
     */
    simtime_t_cref getCreationTime() const {return created;}

    /**
     * Returns time when the message was sent/scheduled or 0 if the message
     * has not been sent yet.
     */
    simtime_t_cref getSendingTime()  const {return sent;}

    /**
     * Returns time when the message arrived (or will arrive if it
     * is currently scheduled or is underway), or 0 if the message
     * has not been sent/scheduled yet.
     *
     * When the message has nonzero length and it travelled though a
     * channel with nonzero data rate, arrival time may represent either
     * the start or the end of the reception, as returned by the
     * isReceptionStart() method. By default it is the end of the reception;
     * it can be changed by calling setDeliverOnReceptionStart(true) on the
     * gate at receiving end of the channel that has the nonzero data rate.
     *
     * @see getDuration()
     */
    simtime_t_cref getArrivalTime()  const {return delivd;}

    /**
     * Return true if the message arrived through the given gate.
     */
    bool arrivedOn(int gateId) const {return gateId==togate;}

    /**
     * Return true if the message arrived on the gate given with its name.
     * If it is a vector gate, the method returns true if the message arrived
     * on any gate in the vector.
     */
    bool arrivedOn(const char *gatename) const;

    /**
     * Return true if the message arrived through the given gate
     * in the named gate vector.
     */
    bool arrivedOn(const char *gatename, int gateindex) const;

    /**
     * Returns a unique message identifier assigned upon message creation.
     */
    long getId() const {return msgid;}

    /**
     * Returns an identifier which is shared among a message object and all messages
     * created by copying it (i.e. by dup() or the copy constructor).
     */
    long getTreeId() const {return msgtreeid;}
    //@}

    /** @name Miscellaneous. */
    //@{

    /**
     * Override to define a display string for the message. Display string
     * affects message appearance in Tkenv. This default implementation
     * returns "".
     */
    virtual const char *getDisplayString() const;
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of messages created since the last
     * reset (reset is usually called my user interfaces at the beginning
     * of each simulation run). The counter is incremented by cMessage constructor.
     * Counter is <tt>signed</tt> to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long getTotalMessageCount() {return total_msgs;}

    /**
     * Returns the number of message objects that currently exist in the
     * program. The counter is incremented by cMessage constructor
     * and decremented by the destructor.
     * May be useful for profiling or debugging memory leaks caused by forgetting
     * to delete messages.
     */
    static long getLiveMessageCount() {return live_msgs;}

    /**
     * Reset counters used by getTotalMessageCount() and getLiveMessageCount().
     */
    static void resetMessageCounters()  {total_msgs=live_msgs=0;}
    //@}
};


/**
 * A subclass of cMessage that can be used to represent packets (frames,
 * datagrams, application messages, etc). cPacket adds length (measured in
 * bits or bytes), bit error flag, and encapsulation capability to cMessage.
 * Length and bit error flag are significant when the packet travels through
 * a cDatarateChannel or another channel that supports data rate and/or
 * error modelling.
 *
 * cPacket is rarely used "as is". Typically the user wants to subclass it
 * to create specific packet types for various protocols being modelled.
 * The most convenient way to do that are via message declaration files
 * (.msg), which are translated into C++ classes.
 * An example message declaration:
 *
 * \code
 * packet Datagram
 * {
 *     int destAddr = -1; // destination address
 *     int srcAddr = -1;  // source address
 *     int ttl =  32;     // time to live
 * }
 * \endcode
 *
 * @see cSimpleModule, cDatarateChannel, cPacketQueue
 *
 * @ingroup SimCore
 */
class SIM_API cPacket : public cMessage
{
  private:
    enum {
        FL_ISRECEPTIONSTART = 4,
        FL_BITERROR = 8,
    };
    int64 len;            // length of the packet in bits -- used for bit error and transmission delay modeling
    simtime_t duration;   // transmission duration on last channel with datarate
    cPacket *encapmsg;    // ptr to the encapsulated message
    unsigned short sharecount; // num of messages MINUS ONE that have this message encapsulated.
                               // 0: not shared (not encapsulated or encapsulated in one message);
                               // 1: shared once (shared among two messages);
                               // 2: shared twice (shared among three messages); etc.
                               // on reaching max sharecount a new packet gets created

  private:
    void copy(const cPacket& packet);

  public:
    // internal: sets the message duration; called by channel objects and sendDirect
    void setDuration(simtime_t d) {duration = d;}

    // internal: sets the isReceptionStart() flag
    void setReceptionStart(bool b) {setFlag(FL_ISRECEPTIONSTART, b);}

    // internal convenience method: returns the getId() of the innermost encapsulated message,
    // or itself if there is no encapsulated message
    long getEncapsulationId() const;

    // internal convenience method: returns getTreeId() of the innermost encapsulated message,
    // or itself if there is no encapsulated message
    long getEncapsulationTreeId() const;

    // internal: if encapmsg is shared (sharecount>0), creates a private copy for this packet,
    // and in any case it sets encapmsg's owner to be this object. This method
    // has to be called before any operation on encapmsg, to prevent trouble
    // that may arise from accessing shared message instances. E.g. without calling
    // _detachEncapMsg(), encapmsg's ownerp is unpredictable (may be any previous owner,
    // possibly not even existing any more) which makes even a call to its getFullPath()
    // method dangerous.
    void _detachEncapMsg();

    // internal: delete encapmsg, paying attention to its sharecount (assumes encapmsg!=NULL)
    void _deleteEncapMsg();

    // internal: only to be used by test cases
    int getShareCount() const {return sharecount;}

  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Copy constructor.
     */
    cPacket(const cPacket& packet);

    /**
     * Constructor. It takes the packet name, message kind value, and the
     * packet length in bits; all optional.
     */
    explicit cPacket(const char *name=NULL, short kind=0, int64 bitLength=0);

    /**
     * Destructor
     */
    virtual ~cPacket();

    /**
     * Assignment operator. Duplication and the assignment operator work all right with cPacket.
     * The name member is not copied; see cNamedObject's operator=() for more details.
     */
    cPacket& operator=(const cPacket& packet);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cPacket *dup() const  {return new cPacket(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Produces a multi-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string detailedInfo() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into an MPI send buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);

    /**
     * Returns true.
     */
    virtual bool isPacket() const {return true;}
    //@}

    /** @name Length and bit error flag */
    //@{
    /**
     * Sets packet length (in bits). When the packet is sent through a
     * channel, packet length affects the transmission duration and the
     * probability of setting the bit error flag.
     */
    virtual void setBitLength(int64 l);

    /**
     * Sets packet length (bytes). This is just a convenience function which
     * invokes setBitLength() with 8*l as argument. The caller must take care
     * that the result does not overflow (i.e. fits into an int64).
     */
    void setByteLength(int64 l)  {setBitLength(l<<3);}

    /**
     * Changes packet length by the given value (bits). This is useful for
     * modeling encapsulation/decapsulation. (See also encapsulate() and
     * decapsulate().) The caller must take care that the result does not
     * overflow (i.e. fits into an int64).
     *
     * The value may be negative (packet length may be decreased too).
     * If the resulting length would be negative, the method throws a
     * cRuntimeError.
     */
    virtual void addBitLength(int64 delta);

    /**
     * Changes packet length by the given value (bytes). This is just a
     * convenience function which invokes addBitLength() with 8*l as argument.
     * The caller must take care that the result does not overflow (i.e.
     * fits into an int64).
     */
    void addByteLength(int64 delta)  {addBitLength(delta<<3);}

    /**
     * Returns the packet length (in bits).
     */
    virtual int64 getBitLength() const  {return len;}

    /**
     * Returns the packet length in bytes, that is, bitlength/8. If bitlength
     * is not a multiple of 8, the result is rounded up.
     */
    int64 getByteLength() const  {return (len+7)>>3;}

    /**
     * Sets the bit error flag.
     */
    virtual void setBitError(bool e) {setFlag(FL_BITERROR,e);}

    /**
     * Returns the bit error flag.
     */
    virtual bool hasBitError() const {return flags&FL_BITERROR;}
    //@}

    /** @name Message encapsulation. */
    //@{

    /**
     * Encapsulates packet in the packet. The packet length gets increased
     * by the length of the encapsulated packet.
     *
     * IMPORTANT NOTE: IT IS FORBIDDEN TO KEEP A POINTER TO A MESSAGE
     * AFTER IT WAS ENCAPSULATED. For performance reasons, encapsulated
     * packets are reference counted, meaning that the encapsulated
     * packet is not duplicated when you duplicate a packet, but rather,
     * both (all) copies share the same packet instance. Any change done
     * to the encapsulated packet would affect other packets as well.
     * Decapsulation (and even calling getEncapsulatedPacket()) will create an
     * own (non-shared) copy of the packet.
     */
    virtual void encapsulate(cPacket *packet);

    /**
     * Decapsulates a packet from the packet object. The length of
     * this packet will be decreased by the length of the encapsulated
     * packet, except if it was zero. If the length would become
     * negative, cRuntimeError is thrown. If there is no encapsulated
     * packet, the method returns NULL.
     */
    virtual cPacket *decapsulate();

    /**
     * Returns a pointer to the encapsulated packet, or NULL if there
     * is no encapsulated packet.
     *
     * IMPORTANT: see notes at encapsulate() about reference counting
     * of encapsulated packets.
     */
    virtual cPacket *getEncapsulatedPacket() const;

    /**
     * DEPRECATED. getEncapsulatedMsg() was renamed to getEncapsulatedPacket(),
     * this method was left for backward compatibility.
     */
    _OPPDEPRECATED cPacket *getEncapsulatedMsg() const {return getEncapsulatedPacket();}

    /**
     * Returns true if the packet contains an encapsulated packet, and false
     * otherwise. This method is potentially more efficient than
     * <tt>getEncapsulatedPacket()!=NULL</tt>, because it does not need to
     * unshare a shared encapsulated packet (see note at encapsulate()).
     */
    virtual bool hasEncapsulatedPacket() const;
    //@}

    /** @name Transmission state */
    //@{
    /**
     * Returns the transmission duration after the packet was sent through
     * a channel with data rate.
     *
     * @see isReceptionStart(), getArrivalTime(), cDatarateChannel
     */
    simtime_t_cref getDuration() const {return duration;}

    /**
     * Tells whether this packet represents the start or the end of the
     * reception, provided the packet has nonzero length and it travelled
     * through a channel with nonzero data rate. This can be configured
     * on the receiving gate (see cGate::setDeliverOnReceptionStart()).
     *
     * @see getArrivalTime(), getDuration(), cDatarateChannel
     */
    bool isReceptionStart() const {return flags & FL_ISRECEPTIONSTART;}
    //@}
};

#ifdef WITHOUT_CPACKET
#define cMessage cPacket /* restore #define in simkerneldefs.h */
#endif

NAMESPACE_END

#endif


