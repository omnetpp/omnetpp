//==========================================================================
//   CMESSAGE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cMessage : message and event object
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMESSAGE_H
#define __CMESSAGE_H

#include <time.h>
#include "cobject.h"
#include "carray.h"
#include "cpar.h"
#include "csimul.h"

//=== classes mentioned:
class cPar;
class cGate;
class cModule;
class cSimpleModule;
class cCompoundModule;
class cSimulation;
class cMessageHeap;

//=== classes declared here:
class  cMessage;

/**
 * Predefined message kind values (values for cMessage's kind(),
 * setKind() methods).
 *
 * Negative values are reserved for the OMNeT++ system and its
 * standard libraries. Zero and positive values can be freely used
 * by simulation models.
 */
enum eMessageKind {
  MK_STARTER = -1,  /// Starter message. Used by scheduleStart().
  MK_TIMEOUT = -2,  /// Internal timeout message. Used by wait(), etc.
  MK_PACKET  = -3,  /// Network packet. Used by cPacket.
  MK_INFO    = -4   /// Information packet. Used by cPacket.
};

//==========================================================================

/**
 * The message class in OMNeT++. cMessage objects may represent events,
 * messages, packets (frames, cells, etc) or other entities in a simulation.
 * cMessage can be assigned a name (a property inherited from cObject)
 * and it has other attributes, including message kind, length, priority,
 * error flag and time stamp.
 *
 * After being sent through a channel, cMessage also remembers
 * the sending and delivery times and its source module. cMessage
 * holds a cArray which means that a cMessage can
 * be attached any number of objects. These objects will typically
 * be of cPar type, but other types are also possible.
 */
class SIM_API cMessage : public cObject
{
    friend class cGate;
    friend class cModule;
    friend class cSimpleModule;
    friend class cCompoundModule;
    friend class cSimulation;
    friend class cMessageHeap;

  private:
    int msgkind;            // message kind -- meaning is user-defined
    int prior;              // priority -- used for scheduling msgs with equal times
    long len;               // length of message -- used for bit errors and transm.delay
    bool error;             // bit error occured during transmission
    simtime_t tstamp;       // time stamp -- user-defined meaning
    cArray *parlistp;       // ptr to list of parameters
    cMessage *encapmsg;     // ptr to encapsulated msg
    void *contextptr;       // a stored pointer -- user-defined meaning

    int frommod,fromgate;   // source module and gate IDs -- set internally
    int tomod,togate;       // dest. module and gate IDs -- set internally
    simtime_t created;      // creation time -- set be constructor
    simtime_t sent,delivd;  // time of sending & delivery -- set internally

    int heapindex;             // used by cMessageHeap (-1 if not on heap)
    unsigned long insertordr;  // used by cMessageHeap


    // helper function
    void _createparlist();

    // global variables for statistics
    static unsigned long total_msgs;
    static unsigned long live_msgs;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor, creates an exact copy of the argument msg.
     */
    cMessage(cMessage& msg);

    /**
     * Constructor.
     */
    explicit cMessage(const char *name=NULL, int k=0, long len=1, int pri=0, bool err=false);

    /**
     * Destructor.
     */
    virtual ~cMessage();

    /**
     * Duplication and the assignment operator work all right with cMessage.
     */
    cMessage& operator=(cMessage& msg);
    //@}

    /** @name Redefined cObject functions. */
    //@{

    /**
     * Returns pointer to the class name string, "cMessage".
     */
    virtual const char *className() const {return "cMessage";}

    /**
     * Duplication and the assignment operator work all right with cMessage.
     */
    virtual cObject *dup()  {return new cMessage(*this);}

    /**
     * MISSINGDOC: cMessage:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cMessage:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cMessageIFC";}

    /**
     * MISSINGDOC: cMessage:void forEach(ForeachFunc)
     */
    virtual void forEach( ForeachFunc do_fn );

    /**
     * MISSINGDOC: cMessage:void writeContents(ostream&)
     */
    virtual void writeContents(ostream& os);

    /**
     * MISSINGDOC: cMessage:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cMessage:int netUnpack()
     */
    virtual int netUnpack();
    //@}

    /** @name Message attributes. */
    //@{

    /**
     * Sets message kind.  The message kind member is not used by OMNeT++,
     * it can be used freely by the user.
     */
    void setKind(int k)     {msgkind=k;}

    /**
     * Sets message priority.  The priority member is used when the simulator
     * inserts messages in the message queue (FES) to order messages
     * with identical arrival time values.
     */
    void setPriority(int p) {prior=p;}

    /**
     * Sets message length. When the message is sent through a
     * channel, message length affects transmission delay
     * and the probability of setting the bit error flag.
     */
    void setLength(long l);

    /**
     * Change message length by the given value. The value may be negative, but
     * a negative resulting message length is an error. Useful for modeling
     * encapsulation/decapsulation. (See also encapsulate() and decapsulate().)
     */
    void addLength(long l);

    /**
     * Set bit error flag.
     */
    void setBitError(bool err) {error=err;}

    /**
     * Sets the message's time stamp to the current simulation time.
     */
    void setTimestamp() {tstamp=simulation.simTime();}

    /**
     * Directly sets the message's time stamp.
     */
    void setTimestamp(simtime_t t) {tstamp=t;}

    /**
     * Set context pointer.
     */
    void setContextPointer(void *p) {contextptr=p;}

    /**
     * Returns message kind.
     */
    int  kind()     {return msgkind;}

    /**
     * Returns message priority.
     */
    int  priority() {return prior;}

    /**
     * Returns message length.
     */
    long length()   {return len;}

    /**
     * Returns true if bit error flag is set, false otherwise.
     */
    bool hasBitError() {return error;}

    /**
     * Returns the message's time stamp.
     */
    simtime_t timestamp() {return tstamp;}

    /**
     * FIXME: INTERNAL: Used by cMessageHeap.
     */
    unsigned long insertOrder() {return insertordr;}

    /**
     * Returns the context pointer.
     */
    void *contextPointer() {return contextptr;}
    //@}

    /** @name Parameter list. */
    //@{

    /**
     * Returns the cArray member of the message which holds
     * the parameters and other attached objects. Parameters can be inserted,
     * retrieved, looked up or deleted through cArray's member
     * functions.
     */
    cArray& parList()
        {if (!parlistp) _createparlist(); return *parlistp;}

    /**
     * Add a parameter to the message's parameter list. Convenience function.
     */
    cPar& addPar(const char *s)  {cPar *p=new cPar(s);parList().add(p);return *p;}

    /**
     * Add a parameter to the message's parameter list. Convenience function.
     */
    cPar& addPar(cPar *p)  {parList().add(p); return *p;}

    /**
     * Add a parameter to the message's parameter list. DEPRECATED.
     */
    cPar& addPar(cPar& p)  {parList().add(&p); return p;}

    /**
     * Returns the indexth object in the message's parameter list,
     * converting it to a cPar. Convenience function.
     */
    cPar& par(int n);

    /**
     * Returns the object with the given name in the message's parameter list,
     * converting it to a cPar. Convenience function.
     */
    cPar& par(const char *s);

    /**
     * Returns the index of the parameter with the given name in the message's
     * parameter list, or -1 if it could not be found. Convenience function.
     */
    int findPar(const char *s) const;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) {return findPar(s)>=0;}
    //@}

    /** @name Message encapsulation. */
    //@{

    /**
     * Encapsulates msg in the message. msg->length()
     * will be added to the length of the message.
     */
    void encapsulate(cMessage *msg);

    /**
     * Decapsulates a message from the message object. The length of
     * the message will be decreased accordingly, except if it was zero.
     * If the length would become negative, an error occurs.
     */
    cMessage *decapsulate();

    /**
     * Returns a pointer to the encapsulated message, or NULL.
     */
    cMessage *encapsulatedMsg() {return encapmsg;}
    //@}

    /** @name Sending/arrival information. */
    //@{

    /**
     * Return true if message was posted by scheduleAt().
     */
    bool isSelfMessage() {return togate==-1;}

    /**
     * Return true if message is among future events.
     */
    bool isScheduled() {return heapindex!=-1;}

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *senderGate() const;

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *arrivalGate() const;

    /**
     * Returns sender module's index in the module vector or -1 if the
     * message hasn't been sent/scheduled yet.
     */
    int senderModuleId() const {return frommod;}

    /**
     * Returns index of gate sent through in the sender module or -1
     * if the message hasn't been sent/scheduled yet.
     */
    int senderGateId() const   {return fromgate;}

    /**
     * Returns receiver module's index in the module vector or -1 if
     * the message hasn't been sent/scheduled yet.
     */
    int arrivalModuleId() const {return tomod;}

    /**
     * Returns index of gate the message arrived on in the sender module
     * or -1 if the message hasn't sent/scheduled yet.
     */
    int arrivalGateId() const  {return togate;}


    /**
     * Returns time when the message was created.
     */
    simtime_t creationTime() const {return created;}

    /**
     * Returns time when the message was sent/scheduled or 0 if the message
     * hasn't been sent yet.
     */
    simtime_t sendingTime()  const {return sent;}

    /**
     * Returns time when the message has arrived or 0 if the message
     * hasn't been sent/scheduled yet.
     */
    simtime_t arrivalTime()  const {return delivd;}


    /**
     * Return true if the message has arrived through gate g.
     */
    bool arrivedOn(int g) const {return g==togate;}

    /**
     * Return true if the message has arrived through the gate
     * given with its name and index (if multiple gate).
     */
    bool arrivedOn(const char *s, int g=0);
    //@}

    /** @name Miscellaneous. */
    //@{

    /**
     * Override to define a display string for the message. Display string
     * affects message appearance in Tkenv.
     * This default implementation returns "".
     */
    virtual const char *displayString();

    /**
     * Static function that compares two messages by their delivery times,
     * then by their priorities. Usable as cQeueue CompareFunc.
     */
    static int cmpbydelivtime(cObject *one, cObject *other);

    /**
     * Static function that compares two messages by their priority.
     * It can be used to sort messages in a priority queue.
     * Usable as cQeueue CompareFunc.
     */
    static int cmpbypriority(cObject *one, cObject *other);

    /**
     * Returns the total number of messages created so far during the
     * current simulation run. May be useful for debugging, profiling, etc.
     */
    static unsigned long totalMessageCount() {return total_msgs;}

    /**
     * Returns the total number of messages that currently exist in the
     * simulation. May be useful for detecting memory leaks caused
     * by forgetting to delete messages.
     */
    static unsigned long liveMessageCount()  {return live_msgs;}

    /**
     * Reset counters used by totalMessageCount() and liveMessageCount().
     */
    static void resetMessageCounters()  {total_msgs=live_msgs=0L;}
    //@}
};

#endif


