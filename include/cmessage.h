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

//=== predefined packet kind values
//
// NOTE:
// - zero and positive values can be freely used
// - negative values are reserved for the OMNeT++ system and standard libs
//
enum {
  // internal
  MK_STARTER = -1,  // used by scheduleStart()
  MK_TIMEOUT = -2,  // used by wait() etc

  // cPacket
  MK_PACKET  = -3,  // network packet
  MK_INFO    = -4   // information packet
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


    /**
     * MISSINGDOC: cMessage:void _createparlist()
     */
    void _createparlist();

    // global variables for statistics
    static unsigned long total_msgs;
    static unsigned long live_msgs;

  public:

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

    // redefined functions

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
     * Duplication and the assignment operator work all right with cMessage.
     */
    cMessage& operator=(cMessage& msg);


    /**
     * MISSINGDOC: cMessage:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cMessage:int netUnpack()
     */
    virtual int netUnpack();

    // new functions

    /**
     * Sets message kind.  The message kind member is not used by OMNeT++,
     * it can be used freely by the user.
     */
    void setKind(int k)     {msgkind=k;}    // set msg kind

    /**
     * Sets message priority.  The priority member is used when the simulator
     * inserts messages in the message queue (FES) to order messages
     * with identical arrival time values.
     */
    void setPriority(int p) {prior=p;}      // set priority

    /**
     * Sets message length. When the message is transmitted through a
     * channel, its error flag will be set with a probability depending
     * on the length of the message and the channel's bit error rate.
     */
    void setLength(long l);                 // set message length

    /**
     * MISSINGDOC: cMessage:void addLength(long)
     */
    void addLength(long l);                 // change message length

    /**
     * MISSINGDOC: cMessage:void setBitError(bool)
     */
    void setBitError(bool err) {error=err;} // set error flag

    /**
     * Sets the message's time stamp to the current simulation time.
     */
    void setTimestamp() {tstamp=simulation.simTime();} // set time stamp to current time

    /**
     * Directly sets the message's time stamp.
     */
    void setTimestamp(simtime_t t) {tstamp=t;}  // set time stamp to given value

    /**
     * MISSINGDOC: cMessage:void setContextPointer(void*)
     */
    void setContextPointer(void *p) {contextptr=p;} // set context pointer


    /**
     * Returns message kind. The message kind member is not used by OMNeT++,
     * it can be used freely by the user.
     */
    int  kind()     {return msgkind;}       // get msg kind

    /**
     * Returns message priority.  The priority member is not used by
     * OMNeT++, it can be used freely by the user.
     */
    int  priority() {return prior;}         // get priority

    /**
     * Returns message length.
     */
    long length()   {return len;}           // get message length

    /**
     * Returns true if error flag is set, false otherwise.
     */
    bool hasBitError() {return error;}      // bit error occurred or not

    /**
     * Returns the message's time stamp.
     */
    simtime_t timestamp() {return tstamp;}  // get time stamp

    /**
     * MISSINGDOC: cMessage:unsigned long insertOrder()
     */
    unsigned long insertOrder() {return insertordr;} // used by cMessageHeap

    /**
     * MISSINGDOC: cMessage:void*contextPointer()
     */
    void *contextPointer() {return contextptr;} // get context pointer


    /**
     * MISSINGDOC: cMessage:bool isSelfMessage()
     */
    bool isSelfMessage() {return togate==-1;}  // tell if message was posted by scheduleAt()

    /**
     * MISSINGDOC: cMessage:bool isScheduled()
     */
    bool isScheduled() {return heapindex!=-1;} // tell if message is among future events

    // parameter list

    /**
     * Returns the cArray member of the message which holds
     * the parameters and other attached objects. Parameters can be inserted,
     * retrieved, looked up or deleted through cArray's member
     * functions.
     */
    cArray& parList()
        {if (!parlistp) _createparlist(); return *parlistp;}

    /**
     * Convenience functions, add a parameter to the message's parameter
     * list.
     */
    cPar& addPar(const char *s)  {cPar *p=new cPar(s);parList().add(p);return *p;}

    /**
     * MISSINGDOC: cMessage:cPar&addPar(cPar*)
     */
    cPar& addPar(cPar *p)  {parList().add(p); return *p;}

    /**
     * Convenience functions, add a parameter to the message's parameter
     * list.
     */
    cPar& addPar(cPar& p)  {parList().add(&p); return p;} // DEPRECATED

    /**
     * Convenience function, returns the indexth object in the
     * message's parameter list, converting it to a cPar.
     */
    cPar& par(int n);                                   // get parameter by index

    /**
     * Convenience function, returns the object with the given name in
     * the message's parameter list, converting it to a cPar.
     */
    cPar& par(const char *s);                           // get parameter by name

    /**
     * Convenience function, returns the index of the parameter with
     * the given name in the message's parameter list, or -1 if it could
     * not be found.
     */
    int findPar(const char *s) const;                   // get index of parameter, -1 if doesn't exist

    /**
     * MISSINGDOC: cMessage:bool hasPar(char*)
     */
    bool hasPar(const char *s) {return findPar(s)>=0;}  // check if parameter exists

    // message encapsulation

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

    // sending/arrival information

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *senderGate() const;                // return NULL if scheduled

    /**
     * Returns pointers to the gate from which the message was sent and
     * on which gate it arrived. A NULL pointer is returned
     * for new (unsent) messages and messages sent via scheduleAt().
     */
    cGate *arrivalGate() const;               //    or unsent message


    /**
     * Returns sender module's index in the module vector or -1 if the
     * message hasn't been sent/scheduled yet.
     */
    int senderModuleId() const {return frommod;}   // source module

    /**
     * Returns index of gate sent through in the sender module or -1
     * if the message hasn't been sent/scheduled yet.
     */
    int senderGateId() const   {return fromgate;}  //    "   gate

    /**
     * Returns receiver module's index in the module vector or -1 if
     * the message hasn't been sent/scheduled yet.
     */
    int arrivalModuleId() const {return tomod;}    // destination

    /**
     * Returns index of gate the message arrived on in the sender module
     * or -1 if the message hasn't sent/scheduled yet.
     */
    int arrivalGateId() const  {return togate;}    //    "   gate


    /**
     * Returns time when the message was created.
     */
    simtime_t creationTime() const {return created;} // creation time

    /**
     * Returns time when the message was sent/scheduled or 0 if the message
     * hasn't been sent yet.
     */
    simtime_t sendingTime()  const {return sent;}    // sending time

    /**
     * Returns time when the message has arrived or 0 if the message
     * hasn't been sent/scheduled yet.
     */
    simtime_t arrivalTime()  const {return delivd;}  // delivery time


    /**
     * Return true if the message has arrived through gate g.
     */
    bool arrivedOn(int g) const {return g==togate;}  // arrived on gate g?

    /**
     * Return true if the message has arrived through the gate
     * given with its name and index (if multiple gate).
     */
    bool arrivedOn(const char *s, int g=0);    // arrived on gate s[g]?

    // message appearance in Tkenv

    /**
     * FIXME: message appearance in Tkenv
     */
    virtual const char *displayString();       // returns ""; redefine to get custom appearance


    /**
     * Static function that compares two messages by their delivery times,
     * then by their priorities.
     */
    static int cmpbydelivtime(cObject *one, cObject *other); // compare delivery times

    /**
     * Static function that compares two messages by their priority.
     * It can be used to sort messages in a priority queue.
     */
    static int cmpbypriority(cObject *one, cObject *other);  // compare priorities

    // statistics

    /**
     * FIXME: statistics
     */
    static unsigned long totalMessageCount() {return total_msgs;}

    /**
     * MISSINGDOC: cMessage:static unsigned long liveMessageCount()
     */
    static unsigned long liveMessageCount()  {return live_msgs;}

    /**
     * MISSINGDOC: cMessage:static void resetMessageCounters()
     */
    static void resetMessageCounters()  {total_msgs=live_msgs=0L;}
};

#endif


