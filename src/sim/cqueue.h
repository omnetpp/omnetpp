//==========================================================================
//   CQUEUE.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cQueue        : (optionally) sorted queue of cObjects
//    cQueueIterator: walks along a queue
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CQUEUE_H
#define __CQUEUE_H

#include "cobject.h"

//=== classes declared here
class  cQueue;
class  cQueueIterator;

//==========================================================================
// sQElem : used internally by cQueue and cQueueIterator
struct sQElem
{
    cObject *obj;
    sQElem *prev, *next;
};

//-------------------------------------------------------------------------
// cQueue: a sorted queue of cObjs

class SIM_API cQueue : public cObject
{
    friend class cQueueIterator;
  private:
    sQElem *headp, *tailp;          // inserting at head, removal at tail
    int n;                          // number of items in queue
    CompareFunc compare;            // compare function
    bool asc;                       // order: true=ascending
  protected:
    sQElem *find_qelem(cObject *obj);
    void insbefore_qelem(sQElem *p, cObject *obj);
    void insafter_qelem(sQElem *p, cObject *obj);
    cObject *remove_qelem(sQElem *p);

  public:
    cQueue(cQueue& queue);
    explicit cQueue(const char *name=NULL, CompareFunc cmp=NULL, bool a=false);
    virtual ~cQueue();

    // redefined functions
    virtual const char *className()  {return "cQueue";}
    virtual cObject *dup()  {return new cQueue(*this);}
    virtual void info(char *buf);
    virtual const char *inspectorFactoryName() {return "cQueueIFC";}
    virtual void forEach(ForeachFunc f);
    virtual int netPack();
    virtual int netUnpack();
    cQueue& operator=(cQueue& queue);

    // new functions
    virtual void setup(CompareFunc cmp=NULL, bool a=false);  // reconfig (without reordering!)

    virtual void insert(cObject *obj);                       // insert from head using cmpfunc
    virtual void insertBefore(cObject *where, cObject *obj); // insert at specific place
    virtual void insertAfter(cObject *where, cObject *obj);  // insert at specific place
    virtual cObject *head()                                  // peek head item
    virtual cObject *tail()                                  // peek tail item
    virtual cObject *remove(cObject *obj);                   // remove item from queue
    virtual cObject *pop();                                  // remove item at tail

    virtual int length()                    // number of items
    bool empty() {return length()==0;}      // check whether queue is empty
    virtual bool contains(cObject *obj)     // check whether queue contains an object
    // bool isEnqueued(cObject *obj)        -- obsolete, use contains() instead!
    virtual void clear();                   // delete whole contents
};

//==========================================================================
//  cQueueIterator : walks along a cQueue
//   NOTE: not a cObject descendant!

class SIM_API cQueueIterator
{
  private:
    sQElem *p;
  public:
    cQueueIterator(cQueue& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sQElem);}
    void init(cQueue& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sQElem);}
    cObject& operator[](int)  {return *p->obj;} //OBSOLETE
    cObject *operator()()     {return p->obj;}
    bool end()                {return (bool)(p==NULL);}
    cObject *operator++(int)  {sQElem *t=p; if(p) p=p->next; return t->obj;}
    cObject *operator--(int)  {sQElem *t=p; if(p) p=p->prev; return t->obj;}
};

#endif

