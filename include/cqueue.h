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
  Copyright (C) 1992-2001 Andras Varga
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

/**
 * FIXME: //=== classes declared here
 * 
 * sQElem : used internally by cQueue and cQueueIterator
 */
struct sQElem
{
    cObject *obj;
    sQElem *prev, *next;
};

//-------------------------------------------------------------------------
// cQueue: a sorted queue of cObjs


/**
 * FIXME: 
 * cQueue: a sorted queue of cObjs
 */
class SIM_API cQueue : public cObject
{
    friend class cQueueIterator;
  private:
    sQElem *headp, *tailp;          // inserting at head, removal at tail
    int n;                          // number of items in queue
    CompareFunc compare;            // compare function
    bool asc;                       // order: true=ascending
  protected:

    /**
     * MISSINGDOC: cQueue:sQElem*find_qelem(cObject*)
     */
    sQElem *find_qelem(cObject *obj);

    /**
     * MISSINGDOC: cQueue:void insbefore_qelem(sQElem*,cObject*)
     */
    void insbefore_qelem(sQElem *p, cObject *obj);

    /**
     * MISSINGDOC: cQueue:void insafter_qelem(sQElem*,cObject*)
     */
    void insafter_qelem(sQElem *p, cObject *obj);

    /**
     * MISSINGDOC: cQueue:cObject*remove_qelem(sQElem*)
     */
    cObject *remove_qelem(sQElem *p);

  public:

    /**
     * Copy constructor. Contained objects that are owned by the queue
     * will be duplicated so that the new queue will have its own copy
     * of them.
     */
    cQueue(cQueue& queue);

    /**
     * Constructor. It accepts the object name, the address of the comparing
     * function, the sorting order (ascending=true, descending=false)
     * and the list the object will join.
     */
    explicit cQueue(const char *name=NULL, CompareFunc cmp=NULL, bool a=false);

    /**
     * Destructor. Deletes all contained objects that were owned by it.
     */
    virtual ~cQueue();

    // redefined functions

    /**
     * Returns pointer to the class name string,"cQueue".
     */
    virtual const char *className() const {return "cQueue";}

    /**
     * Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    virtual cObject *dup()  {return new cQueue(*this);}

    /**
     * Redefined.
     */
    virtual void info(char *buf);

    /**
     * Redefined.
     */
    virtual const char *inspectorFactoryName() const {return "cQueueIFC";}

    /**
     * Calls the given function for each object contained.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * MISSINGDOC: cQueue:int netPack()
     */
    virtual int netPack();

    /**
     * MISSINGDOC: cQueue:int netUnpack()
     */
    virtual int netUnpack();

    /**
     * Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    cQueue& operator=(cQueue& queue);

    // new functions

    /**
     * Changes the sort function and the sorting order. Doesn't re-sort
     * the contents of the queue!
     */
    virtual void setup(CompareFunc cmp=NULL, bool a=false);  // reconfig (without reordering!)


    /**
     * Inserts the given object into the queue, maintaining the sorting
     * order.
     */
    virtual void insert(cObject *obj);                       // insert from head using cmpfunc

    /**
     * Inserts exactly before and after the given object.
     */
    virtual void insertBefore(cObject *where, cObject *obj); // insert at specific place

    /**
     * Inserts exactly before and after the given object.
     */
    virtual void insertAfter(cObject *where, cObject *obj);  // insert at specific place

    /**
     * MISSINGDOC: cQueue:cObject*head()
     */
    virtual cObject *head();                                 // peek head item

    /**
     * Returns pointer to the last object in the queue or NULL
     * if the queue is empty.
     */
    virtual cObject *tail();                                 // peek tail item

    /**
     * Unlinks and returns the object given.
     */
    virtual cObject *remove(cObject *obj);                   // remove item from queue

    /**
     * Unlinks and returns the last object in the queue.
     */
    virtual cObject *pop();                                  // remove item at tail


    /**
     * Returns the number of objects contained in the queue.
     */
    virtual int length();                   // number of items

    /**
     * Returns true if the queue is empty.
     */
    bool empty() {return length()==0;}      // check whether queue is empty

    /**
     * MISSINGDOC: cQueue:bool contains(cObject*)
     */
    virtual bool contains(cObject *obj);    // check whether queue contains an object
    // bool isEnqueued(cObject *obj)        -- obsolete, use contains() instead!

    /**
     * As a result, the container will be empty. Contained objects that
     * were owned by the queue will be deleted.
     */
    virtual void clear();                   // delete whole contents
};

//==========================================================================
//  cQueueIterator : walks along a cQueue
//   NOTE: not a cObject descendant!


/**
 * FIXME: 
 *  cQueueIterator : walks along a cQueue
 *   NOTE: not a cObject descendant!
 */
class SIM_API cQueueIterator
{
  private:
    sQElem *p;
  public:

    /**
     * Constructor, cIterator will walk on the queue passed
     * as argument. The current object will be the first (if a==1) or
     * the last (a==0) object in the queue.
     */
    cQueueIterator(cQueue& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sQElem);}

    /**
     * Reinitializes the iterator object.
     */
    void init(cQueue& q, int athead=1)
            {p=&q ? (athead ? q.headp : q.tailp) : NO(sQElem);}

    /**
     * MISSINGDOC: cQueueIterator:cObject&operator[]()
     */
    cObject& operator[](int)  {return *p->obj;} //OBSOLETE

    /**
     * Returns the current object.
     */
    cObject *operator()()     {return p->obj;}

    /**
     * Returns true if we have reached either end of the queue.
     */
    bool end()                {return (bool)(p==NULL);}

    /**
     * MISSINGDOC: cQueueIterator:cObject*operator++()
     */
    cObject *operator++(int)  {sQElem *t=p; if(p) p=p->next; return t->obj;}

    /**
     * MISSINGDOC: cQueueIterator:cObject*operator--()
     */
    cObject *operator--(int)  {sQElem *t=p; if(p) p=p->prev; return t->obj;}
};

#endif
