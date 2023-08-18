//==========================================================================
//   CEVENT.H  -  header for
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

#ifndef __OMNETPP_CEVENT_H
#define __OMNETPP_CEVENT_H

#include "cownedobject.h"

namespace omnetpp {

class cMessage;
class cPacket;
class cEventHeap;

/**
 * @brief Represents an event in the discrete event simulator.
 *
 * When events are scheduled, they are inserted into the future events set (FES)
 * where they are (conceptually) stored in timestamp (="arrival time") order.
 * Events are removed from the FES one by one, and processed by invoking their
 * execute() methods. execute() should be overridden in subclasses to carry out
 * the actions associated with the event.
 *
 * If several events have identical timestamp values (arrival times), further fields
 * decide their ordering: first, scheduling priority and then, insertion order.
 *
 * Pure event objects (cEvent) are normally of little interest to the user. Instead,
 * one should use messages and packets (cMessage, cPacket). They are subclassed from
 * cEvent, and their execute() methods automatically delivers the message/packet
 * to the target module.
 *
 * @ingroup SimCore
 */
class SIM_API cEvent : public cOwnedObject
{
    friend class cMessage;     // getArrivalTime()
    friend class cEventHeap;   // heapIndex

  private:
    simtime_t arrivalTime;  // time of delivery -- set internally
    short priority = 0;     // priority -- used for scheduling events with equal arrival times
    int heapIndex = -1;     // used by cEventHeap (-1 if not on heap; all other values, including negative ones, means "on the heap")
    eventnumber_t insertOrder = -1; // used by the FES to keep order of events with equal time and priority
    eventnumber_t previousEventNumber = -1; // most recent event number when envir was notified about this event object (e.g. creating/cloning/sending/scheduling/deleting of this event object)

  public:
    // internal: returns the event number which scheduled this event object, or the event
    // number in which this event object was last executed (e.g. delivered to a module);
    // stored for recording into the event log file.
    eventnumber_t getPreviousEventNumber() const {return previousEventNumber;}

    // internal: sets previousEventNumber.
    void setPreviousEventNumber(eventnumber_t num) {previousEventNumber = num;}

    // internal: used by cEventHeap.
    eventnumber_t getInsertOrder() const {return insertOrder;}

    // internal: called by the simulation kernel to set the value returned
    // by the getArrivalTime() method; must not be called while the event is
    // in the FES, because it would break ordering.
    void setArrivalTime(simtime_t t) {ASSERT(!isScheduled()); arrivalTime = t;}

    // internal: used by the parallel simulation kernel.
    virtual int getSrcProcId() const {return -1;}

    // internal: utility function
    static int compareBySchedulingOrder(const cEvent *a, const cEvent *b);

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cEvent(const cEvent& event) : cOwnedObject(event) {operator=(event);}

    /**
     * Constructor.
     */
    explicit cEvent(const char *name) : cOwnedObject(name, false) {}  // note: name pooling is off by default, as unique message names are quite common

    /**
     * Destructor.
     */
    virtual ~cEvent() {}

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
     */
    cEvent& operator=(const cEvent& event);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Redefined to override return type. See cObject for more details.
     */
    virtual cEvent *dup() const override = 0;

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

    /** @name Event attributes. */
    //@{

    /**
     * Sets the scheduling priority of this event. Scheduling priority is
     * used when the simulator inserts messages into the future events set
     * (FES), to order events with identical arrival time values.
     *
     * The scheduling priority must not be modified while the event is
     * scheduled, i.e. is owned by the FES.
     */
    void setSchedulingPriority(short p);

    /**
     * Returns the scheduling priority of this event.
     */
    short getSchedulingPriority() const  {return priority;}
    //@}

    /** @name Arrival information. */
    //@{

    /**
     * Returns true if this event is in the future events set (FES).
     */
    bool isScheduled() const  {return heapIndex!=-1;}

    /**
     * Returns the simulation time this event object has been last scheduled for
     * (regardless whether it is currently scheduled), or zero if the event
     * hasn't been scheduled yet.
     */
    simtime_t_cref getArrivalTime() const  {return arrivalTime;}

    /**
     * Return the object that this event will be delivered to or act upon,
     * or nullptr if there is no such object. For messages and packets this will
     * be the destination module. This method is not used by the simulation
     * kernel for other than informational purposes, e.g. logging.
     *
     * @see cMessage::getArrivalModule()
     */
    virtual cObject *getTargetObject() const = 0;
    //@}

    /** @name Methods to be used by the simulation kernel and the scheduler. */
    //@{
    /**
     * Returns true if the object is a subclass of cMessage. This method is a more efficient alternative of dynamic_cast.
     */
    virtual bool isMessage() const {return false;}

    /**
     * Return true if the object is a subclass of cPacket. This method is a more efficient alternative of dynamic_cast.
     */
    virtual bool isPacket() const {return false;}

    /**
     * Returns true if this event is stale. An event might go stale while
     * staying in the future events set (FES), for example due to its target
     * object being deleted. Stale events are discarded by the scheduler.
     */
    virtual bool isStale() {return false;}

    /**
     * Returns true if this event precedes the given one in scheduling order,
     * and false otherwise. Scheduling order is defined by the arrival time
     * first, the scheduling priority second, and the scheduling order (a.k.a
     * FES insertion order) third. Note that this method generally only
     * produces useful results for events inserted in the FES.
     */
    bool shouldPrecede(const cEvent *event) const;

    /**
     * This method performs the action associated with the event. When a
     * scheduled event makes it to the front of the FES, it is removed
     * from the FES and its execute() method is invoked. In cMessage,
     * execute() ends up calling the handleMessage() method of the destination
     * module, or switches to the coroutine of its activity() method.
     */
    virtual void execute() = 0;
    //@}
};

}  // namespace omnetpp

#endif

