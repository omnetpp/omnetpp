//==========================================================================
//  CMESSAGEHEAP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cMessageHeap : future event set, implemented as heap
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CMESSAGEHEAP_H
#define __OMNETPP_CMESSAGEHEAP_H

#include "cownedobject.h"

NAMESPACE_BEGIN

class cEvent;


/**
 * Stores the future event set. The underlying data structure is heap;
 * the array used to store the heap expands as needed.
 *
 * @see Iterator
 * @ingroup Internals
 */
class SIM_API cMessageHeap : public cOwnedObject
{
  public:
    /**
     * Walks along a cMessageHeap. Note that objects in cMessageHeap are not
     * necessarily iterated ordered by arrival time. Use msgheap->sort()
     * if necessary before using the iterator.
     */
    class Iterator
    {
      private:
        cMessageHeap *q;
        int pos;

      public:
        /**
         * Constructor.
         */
        Iterator(const cMessageHeap& mh)  {q=const_cast<cMessageHeap*>(&mh);pos=0;}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cMessageHeap& mh) {q=const_cast<cMessageHeap*>(&mh);pos=0;}

        /**
         * Returns the current object.
         */
        cEvent *operator()()  {return q->peek(pos);}

        /**
         * Returns the current object, then moves the iterator to the next item.
         * If the iterator has reached the end of the list, NULL is returned.
         */
        cEvent *operator++(int)  {return end() ? nullptr : q->peek(pos++);}

        /**
         * Returns true if the iterator has reached the end of the list.
         */
        bool end() const  {return pos>=q->getLength();}
    };

    friend class Iterator;

  private:
    // heap data structure
    cEvent **h;               // pointer to the 'heap'  (h[0] always empty)
    int n;                    // number of elements in the heap
    int size;                 // size of allocated h array
    unsigned long insertcntr; // counts insertions

    // circular buffer for events scheduled for the current simtime (quite frequent)
    cEvent **cb;              // size of the circular buffer
    int cbsize;               // always power of 2
    int cbhead, cbtail;       // cbhead is inclusive, cbtail is exclusive

  private:
    void copy(const cMessageHeap& other);

    // internal: restore heap
    void shiftup(int from=1);

    int cblength() const  {return (cbtail-cbhead) & (cbsize-1);}
    cEvent *cbget(int k)  {return cb[(cbhead+k) & (cbsize-1)];}
    void cbgrow();

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Copy constructor.
     */
    cMessageHeap(const cMessageHeap& msgq);

    /**
     * Constructor.
     */
    cMessageHeap(const char *name=nullptr, int size=128);

    /**
     * Destructor.
     */
    virtual ~cMessageHeap();

    /**
     * Assignment operator. The name member is not copied;
     * see cOwnedObject's operator=() for more details.
     */
    cMessageHeap& operator=(const cMessageHeap& msgqueue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cMessageHeap *dup() const override  {return new cMessageHeap(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string info() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;

    // no parsimPack() and parsimUnpack()
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Insert a new event into the heap.
     */
    void insert(cEvent *event);

    /**
     * Peek the first event in the heap (the one with the smallest timestamp.)
     * If the heap is empty, it returns NULL.
     */
    cEvent *peekFirst() const;

    /**
     * Returns the mth event in the heap if 0 <= m < getLength(), and NULL
     * otherwise. Note that iteration does not necessarily return events
     * in increasing timestamp (getArrivalTime()) order unless you called
     * sort() before.
     */
    cEvent *peek(int m);

    /**
     * Removes and return the first event in the heap (the one with the
     * smallest timestamp.) If the heap is empty, it returns NULL.
     */
    cEvent *removeFirst();

    /**
     * Undo for removeFirst(): it puts back an event to the front of the FES.
     */
    void putBackFirst(cEvent *event);

    /**
     * Removes and returns the given event in the heap. If the event is
     * not in the heap, returns NULL.
     */
    cEvent *remove(cEvent *event);

    /**
     * Sorts the contents of the heap. This is only necessary if one wants
     * to iterate through in the FES in strict timestamp order.
     */
    void sort();

    /**
     * Deletes all events in the heap.
     */
    void clear();

    /**
     * Returns the number of events in the heap.
     */
    int getLength() const {return cblength() + n;}

    /**
     * Returns true if the heap is empty.
     */
    bool isEmpty() const {return cbhead==cbtail && n==0;}

    /**
     * Alias for getLength().
     */
    int length() const {return getLength();}

    /**
     * Alias for isEmpty().
     */
    bool empty() const {return isEmpty();}
    //@}
};

NAMESPACE_END


#endif

