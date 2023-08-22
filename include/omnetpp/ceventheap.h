//==========================================================================
//  CEVENTHEAP.H - part of
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

#ifndef __OMNETPP_CEVENTHEAP_H
#define __OMNETPP_CEVENTHEAP_H

#include "cfutureeventset.h"

namespace omnetpp {

/**
 * @brief The default, binary heap based implementation of the future event set.
 *
 * Using binary heap as the underlying data structure provides reliable
 * performance for most workloads. A worst case for heap is insertion at the
 * front (i.e. for the current simulation time), which is actually quite common,
 * due to the abundance of zero-delay links in models. This case is optimized
 * by employing an additional circular buffer specifically for storing events
 * inserted scheduled for the current simulation time.
 *
 * @ingroup SimCore
 */
class SIM_API cEventHeap : public cFutureEventSet
{
  private:
    // heap data structure
    cEvent **heap = nullptr;       // heap array  (heap[0] always empty)
    int heapLength = 0;            // number of elements on the heap
    int heapCapacity = 0;          // allocated size of the heap[] array
    eventnumber_t insertCount = 0; // counts insertions; needed because heap's insert is not stable (does not keep order)

    // circular buffer for events scheduled for the current simtime (quite frequent); acts as FIFO
    cEvent **cb = nullptr;        // size of the circular buffer
    int cbsize = 4;               // always power of 2
    int cbhead = 0, cbtail = 0;   // cbhead is inclusive, cbtail is exclusive
    bool useCb = true;            // for disabling cb

  private:
    void copy(const cEventHeap& other);

    // internal: restore heap
    void shiftup(int from=1);

    int cblength() const  {return (cbtail-cbhead) & (cbsize-1);}
    cEvent *cbget(int k)  {return cb[(cbhead+k) & (cbsize-1)];}
    void cbgrow();

    void heapInsert(cEvent *event);
    void cbInsert(cEvent *event);
    void flushCb();

  public:
    // internal:
    bool getUseCb() const {return useCb;}
    void setUseCb(bool b) {ASSERT(cbhead==cbtail); useCb = b;}

    // utility function for checking heap sanity
    virtual void checkHeap();

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cEventHeap(const cEventHeap& msgq);

    /**
     * Constructor.
     */
    cEventHeap(const char *name=nullptr, int initialCapacity=128);

    /**
     * Destructor.
     */
    virtual ~cEventHeap();

    /**
     * Assignment operator. The name member is not copied;
     * see cOwnedObject's operator=() for more details.
     */
    cEventHeap& operator=(const cEventHeap& other);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cEventHeap *dup() const override  {return new cEventHeap(*this);}

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

    // no parsimPack() and parsimUnpack()
    //@}

    /** @name Simulation-related operations. */
    //@{
    /**
     * Insert an event into the FES.
     */
    virtual void insert(cEvent *event) override;

    /**
     * Peek the first event in the FES (the one with the smallest timestamp.)
     * If the FES is empty, it returns nullptr.
     */
    virtual cEvent *peekFirst() const override;

    /**
     * Removes and return the first event in the FES (the one with the
     * smallest timestamp.) If the FES is empty, it returns nullptr.
     */
    virtual cEvent *removeFirst() override;

    /**
     * Undo for removeFirst(): it puts back an event to the front of the FES.
     */
    virtual void putBackFirst(cEvent *event) override;

    /**
     * Removes and returns the given event in the FES. If the event is
     * not in the FES, returns nullptr.
     */
    virtual cEvent *remove(cEvent *event) override;

    /**
     * Returns true if the FES is empty.
     */
    virtual bool isEmpty() const override {return cbhead==cbtail && heapLength==0;}

    /**
     * Deletes all events in the FES.
     */
    virtual void clear() override;
    //@}

    /** @name Random access. */
    //@{

    /**
     * Returns the number of events in the FES.
     */
    virtual int getLength() const override {return cblength() + heapLength;}

    /**
     * Returns the kth event in the FES if 0 <= k < getLength(), and nullptr
     * otherwise. Note that iteration does not necessarily return events
     * in increasing timestamp (getArrivalTime()) order unless you called
     * sort() before.
     */
    virtual cEvent *get(int k) override;

    /**
     * Sorts the contents of the FES. This is only necessary if one wants
     * to iterate through in the FES in strict timestamp order.
     */
    virtual void sort() override;
};

}  // namespace omnetpp


#endif

