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
// cMessageHeap: object to store future event set
//    underlying data structure is heap; vector expands as needed


/**
 * FIXME: 
 * cMessageHeap: object to store future event set
 *    underlying data structure is heap; vector expands as needed
 */
class SIM_API cMessageHeap : public cObject
{
    friend class cMessageHeapIterator;
  private:
    cMessage **h;             // pointer to the 'heap'  (h[0] always empty)
    int n;                    // number of elements in the heap
    int size;                 // size of allocated h array
    unsigned long insertcntr; // counts insertions


    /**
     * MISSINGDOC: cMessageHeap:void shiftup(int)
     */
    void shiftup(int from=1);

  public:

    /**
     * MISSINGDOC: cMessageHeap:cMessageHeap(cMessageHeap&)
     */
    cMessageHeap(cMessageHeap& msgq);

    /**
     * MISSINGDOC: cMessageHeap:cMessageHeap(char*,int)
     */
    cMessageHeap(const char *name=NULL, int size=128);

    /**
     * MISSINGDOC: cMessageHeap:~cMessageHeap()
     */
    virtual ~cMessageHeap();

    // redefined functions

    /**
     * FIXME: redefined functions
     */
    virtual const char *className() const {return "cMessageHeap";}

    /**
     * MISSINGDOC: cMessageHeap:cObject*dup()
     */
    virtual cObject *dup()  {return new cMessageHeap(*this);}

    /**
     * MISSINGDOC: cMessageHeap:void info(char*)
     */
    virtual void info(char *buf);

    /**
     * MISSINGDOC: cMessageHeap:char*inspectorFactoryName()
     */
    virtual const char *inspectorFactoryName() const {return "cMessageHeapIFC";}

    /**
     * MISSINGDOC: cMessageHeap:void forEach(ForeachFunc)
     */
    virtual void forEach(ForeachFunc f);

    /**
     * MISSINGDOC: cMessageHeap:cMessageHeap&operator=(cMessageHeap&)
     */
    cMessageHeap& operator=(cMessageHeap& msgqueue);
    // no netPack() and netUnpack()

    // new functions

    /**
     * FIXME: no netPack() and netUnpack()
     * new functions
     */
    void insert(cMessage *event);

    /**
     * MISSINGDOC: cMessageHeap:cMessage*peekFirst()
     */
    cMessage *peekFirst();

    /**
     * MISSINGDOC: cMessageHeap:cMessage*getFirst()
     */
    cMessage *getFirst();

    /**
     * MISSINGDOC: cMessageHeap:cMessage*get(cMessage*)
     */
    cMessage *get(cMessage *event);

    /**
     * MISSINGDOC: cMessageHeap:void sort()
     */
    void sort();

    /**
     * MISSINGDOC: cMessageHeap:void clear()
     */
    void clear();

    /**
     * MISSINGDOC: cMessageHeap:int length()
     */
    int length() {return n;}

    /**
     * MISSINGDOC: cMessageHeap:bool empty()
     */
    bool empty() {return n==0;}
};

//==========================================================================
//  cMessageHeapIterator : walks along a cMessageHeap
//   Objects are not necessarily ordered by arrival time!
//   Use msgheap->sort() if necessary before using the iterator.


/**
 * FIXME: 
 *  cMessageHeapIterator : walks along a cMessageHeap
 *   Objects are not necessarily ordered by arrival time!
 *   Use msgheap->sort() if necessary before using the iterator.
 */
class SIM_API cMessageHeapIterator
{
  private:
    cMessageHeap *q;
    int pos;
  public:

    /**
     * MISSINGDOC: cMessageHeapIterator:cMessageHeapIterator(cMessageHeap&)
     */
    cMessageHeapIterator(cMessageHeap& mh)  {q=&mh;pos=1;}

    /**
     * MISSINGDOC: cMessageHeapIterator:void init(cMessageHeap&)
     */
    void init(cMessageHeap& mh) {q=&mh;pos=1;}

    /**
     * MISSINGDOC: cMessageHeapIterator:cMessage*operator()()
     */
    cMessage *operator()()      {return q->h[pos];}

    /**
     * MISSINGDOC: cMessageHeapIterator:cMessage*operator++()
     */
    cMessage *operator++(int)   {return pos<=q->n ? q->h[++pos] : NO(cMessage);}

    /**
     * MISSINGDOC: cMessageHeapIterator:bool end()
     */
    bool end()                  {return (bool)(pos>q->n);}
};

#endif
