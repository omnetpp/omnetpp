//==========================================================================
//   CMSGHEAP.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cMessageHeap : future event set, implemented as heap
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CMSGHEAP_H
#define __CMSGHEAP_H

#include "cobject.h"

class cMessage;

//==========================================================================

/**
 * Stores future event set. The underlying data structure is heap;
 * the array used to store the heap expands as needed.
 *
 * @ingroup Internals
 * @see cMessageHeapIterator
 */
class SIM_API cMessageHeap : public cObject
{
    friend class cMessageHeapIterator;
  private:
    cMessage **h;             // pointer to the 'heap'  (h[0] always empty)
    int n;                    // number of elements in the heap
    int size;                 // size of allocated h array
    unsigned long insertcntr; // counts insertions

    // internal
    void shiftup(int from=1);

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
    cMessageHeap(const char *name=NULL, int size=128);

    /**
     * Destructor.
     */
    virtual ~cMessageHeap();

    /**
     * Assignment operator. The name member doesn't get copied;
     * see cObject's operator=() for more details.
     */
    cMessageHeap& operator=(const cMessageHeap& msgqueue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cMessageHeap".
     */
    virtual const char *className() const {return "cMessageHeap";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup() const  {return new cMessageHeap(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cMessageHeapIFC";}

    /**
     * Call the passed function for each contained object.
     * See cObject for more details.
     */
    virtual void forEach(ForeachFunc f);

    // no netPack() and netUnpack()
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Insert a new message into the heap.
     */
    void insert(cMessage *event);

    /**
     * Peek the first message in the heap (the one with the smallest timestamp.)
     */
    cMessage *peekFirst() const;

    /**
     * Remove and return the first message in the heap (the one
     * with the smallest timestamp.)
     */
    cMessage *getFirst();

    /**
     * Remove and return the given message in the heap.
     */
    cMessage *get(cMessage *event);

    /**
     * Sort the contents of the heap. This is only necessary if one wants
     * to iterate through in the FES in strict timestamp order.
     */
    void sort();

    /**
     * Deletes all messages in the heap.
     */
    void clear();

    /**
     * Returns the number of messages in the heap.
     */
    int length() const {return n;}

    /**
     * Returns true if the heap is empty.
     */
    bool empty() const {return n==0;}
    //@}
};

//==========================================================================

/**
 * Walks along a cMessageHeap. Note that objects in cMessageHeap are not
 * necessarily iterated ordered by arrival time. Use msgheap->sort()
 * if necessary before using the iterator.
 */
class SIM_API cMessageHeapIterator
{
  private:
    cMessageHeap *q;
    int pos;
  public:

    /**
     * Constructor.
     */
    cMessageHeapIterator(const cMessageHeap& mh)  {q=const_cast<cMessageHeap*>(&mh);pos=1;} //FIXME: not correct?

    /**
     * Reinitializes the iterator object.
     */
    void init(const cMessageHeap& mh) {q=const_cast<cMessageHeap*>(&mh);pos=1;}

    /**
     * Returns the current object.
     */
    cMessage *operator()()      {return q->h[pos];}

    /**
     * Returns the current object, then moves the iterator to the next item.
     * If the iterator has reached the end of the list, NULL is returned.
     */
    cMessage *operator++(int)   {return pos<=q->n ? q->h[++pos] : NO(cMessage);}

    /**
     * Returns true if the iterator has reached the end of the list.
     */
    bool end() const                  {return (bool)(pos>q->n);}
};

#endif

