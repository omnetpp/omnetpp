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
  Copyright (C) 1992,99 Andras Varga
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

class cMessageHeap : public cObject
{
        friend class cMessageHeapIterator;
    private:
        cMessage **h;             // pointer to the 'heap'  (h[0] always empty)
        int n;                    // number of elements in the heap
        int size;                 // size of allocated h array
        unsigned long insertcntr; // counts insertions

        void shiftup(int from=1);

    public:
        cMessageHeap(cMessageHeap& msgq);
        cMessageHeap(char *namestr=NULL, int size=128);
        virtual ~cMessageHeap();

        // redefined functions
        virtual char *isA()  {return "cMessageHeap";}
        virtual cObject *dup()  {return new cMessageHeap(*this);}
        virtual void info(char *buf);
        virtual char *inspectorFactoryName() {return "cMessageHeapIFC";}
        virtual void forEach(ForeachFunc f);
        cMessageHeap& operator=(cMessageHeap& msgqueue);
        // no netPack() and netUnpack()

        // new functions
        void insert(cMessage *event);
        cMessage *peekFirst();
        cMessage *getFirst();
        cMessage *get(cMessage *event);
        void sort();
        void clear();
        int length() {return n;}
        bool empty() {return n==0;}
};

//==========================================================================
//  cMessageHeapIterator : walks along a cMessageHeap
//   Objects are not necessarily ordered by arrival time!
//   Use msgheap->sort() if necessary before using the iterator.

class cMessageHeapIterator
{
   private:
      cMessageHeap *q;
      int pos;
   public:
      cMessageHeapIterator(cMessageHeap& mh)  {q=&mh;pos=1;}
      void init(cMessageHeap& mh) {q=&mh;pos=1;}
      cMessage *operator()()      {return q->h[pos];}
      cMessage *operator++(int)   {return pos<=q->n ? q->h[++pos] : NO(cMessage);}
      bool end()                  {return (bool)(pos>q->n);}
};

#endif
