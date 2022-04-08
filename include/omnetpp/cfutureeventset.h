//==========================================================================
//  CFUTUREEVENTSET.H - part of
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

#ifndef __OMNETPP_CFUTUREEVENTSET_H
#define __OMNETPP_CFUTUREEVENTSET_H

#include "cownedobject.h"

namespace omnetpp {

class cEvent;

/**
 * @brief Abstract base class for the future event set (FES), a central data structure
 * for discrete event simulation. FES is also known as FEL (future event list).
 *
 * @see cSimulation::getFES()
 * @ingroup SimCore
 */
class SIM_API cFutureEventSet : public cOwnedObject
{
  public:
    /** @name Constructors, destructor, assignment */
    //@{
    /**
     * Constructor.
     */
    cFutureEventSet(const char *name=nullptr) : cOwnedObject(name, false) {}

    /**
     * Destructor.
     */
    virtual ~cFutureEventSet() {}

    /**
     * Assignment operator. The name member is not copied;
     * see cOwnedObject's operator=() for more details.
     */
    cFutureEventSet& operator=(const cFutureEventSet& other);
    //@}

    /** @name Redefined cObject member functions. */
    //@{
    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;
    //@}

    /** @name Simulation-related operations. */
    //@{

    /**
     * Insert an event into the FES.
     */
    virtual void insert(cEvent *event) = 0;

    /**
     * Peek the first event in the FES (the one with the smallest timestamp.)
     * If the FES is empty, it returns nullptr.
     */
    virtual cEvent *peekFirst() const = 0;

    /**
     * Removes and return the first event in the FES (the one with the
     * smallest timestamp.) If the FES is empty, it returns nullptr.
     */
    virtual cEvent *removeFirst() = 0;

    /**
     * Undo for removeFirst(): it puts back an event to the front of the FES.
     */
    virtual void putBackFirst(cEvent *event) = 0;

    /**
     * Removes and returns the given event in the FES. If the event is
     * not in the FES, returns nullptr.
     */
    virtual cEvent *remove(cEvent *event) = 0;

    /**
     * Returns true if the FES is empty.
     */
    virtual bool isEmpty() const = 0;

    /**
     * Deletes all events in the FES.
     */
    virtual void clear() = 0;
    //@}

    /** @name Random access. */
    //@{
    /**
     * Returns the number of events in the FES.
     */
    virtual int getLength() const = 0;

    /**
     * Returns the kth event in the FES if 0 <= k < getLength(), and nullptr
     * otherwise. Note that iteration does not necessarily return events
     * in increasing timestamp (getArrivalTime()) order unless sort() has been
     * called since the last mutating operation.
     *
     * get(k) is very often called in a loop that goes from 0 to getLength()-1,
     * so implementations that employ a data structure that is ill-suited for
     * random access (e.g. list or tree-based) are advised to cache the information
     * used to look up the kth item, and reuse it for a subsequent get(k+1) call.
     */
    virtual cEvent *get(int k) = 0;

    /**
     * Sorts the contents of the FES. This is only necessary if one wants
     * to iterate through events in the FES in strict timestamp order.
     */
    virtual void sort() = 0;
    //@}
};

}  // namespace omnetpp

#endif

