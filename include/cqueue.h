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

//
// Used internally by cQueue and cQueueIterator.
//
struct sQElem
{
    cObject *obj;
    sQElem *prev, *next;
};

//-------------------------------------------------------------------------

/**
 * Queue class. cQueue is a container class that can hold objects derived
 * from cObject. cQueue acts as a priority queue.
 * The user must provide a function that can compare two objects.
 * If no such function is given, cQueue implements a FIFO.
 * Order (ascending or descending) can be specified.
 *
 * Ownership of contained objects (responsibility of deletion) can
 * be specified per-object basis (see cObject::takeOwnership()).
 * Default is that cQueue takes the ownership of each object
 * inserted (that is, takeOwnership(true)).
 *
 * The sorting function should look like:
 * int CompareFunc(cObject& a, cObject& b);
 *
 * They must return a negative value if a&lt;b, 0 if a==b and a positive value
 * if a&gt;b.
 *
 * @ingroup Containers
 * @see cQueueIterator
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
    // internal functions
    sQElem *find_qelem(cObject *obj);
    void insbefore_qelem(sQElem *p, cObject *obj);
    void insafter_qelem(sQElem *p, cObject *obj);
    cObject *remove_qelem(sQElem *p);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects that are owned by the queue
     * will be duplicated so that the new queue will have its own copy
     * of them.
     */
    cQueue(cQueue& queue);

    /**
     * Constructor. It accepts the object name, the address of the comparing
     * function and the sorting order (ascending=true, descending=false).
     */
    explicit cQueue(const char *name=NULL, CompareFunc cmp=NULL, bool a=false);

    /**
     * Destructor. Deletes all contained objects that were owned by it.
     */
    virtual ~cQueue();

    /**
     * Assignment operator. Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     *
     * The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cQueue& operator=(cQueue& queue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cQueue".
     */
    virtual const char *className() const {return "cQueue";}

    /**
     * Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    virtual cObject *dup()  {return new cQueue(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cQueueIFC";}

    /**
     * Calls the given function for each contained
     * object.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Setup, insertion and removal functions. */
    //@{

    /**
     * Changes the sort function and the sorting order. Doesn't re-sort
     * the contents of the queue!
     */
    virtual void setup(CompareFunc cmp, bool a=false);

    /**
     * Inserts the given object into the queue, maintaining the sorting
     * order. Trying to insert a NULL pointer is an error.
     */
    virtual void insert(cObject *obj);

    /**
     * Inserts exactly before the given object. If the given position
     * does not exist, an error is raised. Trying to insert a NULL pointer
     * is also an error.
     */
    virtual void insertBefore(cObject *where, cObject *obj);

    /**
     * Inserts exactly after the given object.  If the given position
     * does not exist, an error is raised. Trying to insert a NULL pointer
     * is also an error.
     */
    virtual void insertAfter(cObject *where, cObject *obj);

    /**
     * Unlinks and returns the object given. If the object is not in the
     * queue, a warning is issued and the same pointer is returned.
     */
    virtual cObject *remove(cObject *obj);

    /**
     * Unlinks and returns the last (tail) object in the queue. If the queue
     * was empty, an error is raised.
     */
    virtual cObject *pop();

    /**
     * As a result, the container will be empty. Contained objects that
     * were owned by the queue will be deleted.
     */
    virtual void clear();
    //@}

    /** @name Query functions. */
    //@{

    /**
     * Returns pointer to the object at the head of the queue.
     * Returns NULL if the queue is empty.
     */
    virtual cObject *head();

    /**
     * Returns pointer to the last (tail) object in the queue.
     * Returns NULL if the queue is empty.
     */
    virtual cObject *tail();

    /**
     * Returns the number of objects contained in the queue.
     */
    virtual int length();

    /**
     * Returns true if the queue is empty.
     */
    bool empty() {return length()==0;}

    /**
     * Returns true if the queue contains the passed object.
     */
    virtual bool contains(cObject *obj);
    //@}
};

//==========================================================================

/**
 * Walks along a cQueue.
 *
 * NOTE: not a cObject descendant.
 */
class SIM_API cQueueIterator
{
  private:
    sQElem *p;

  public:
    /**
     * Constructor. cQueueIterator will walk on the queue passed
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
     * OBSOLETE. Use operator () instead.
     */
    cObject& operator[](int)  {return *p->obj;}

    /**
     * Returns the current object.
     */
    cObject *operator()()     {return p->obj;}

    /**
     * Returns true if the iterator has reached either end of the queue.
     */
    bool end()                {return (bool)(p==NULL);}

    /**
     * Returns the current object, then moves the iterator to the next item.
     * If the iterator has reached either end of the queue, nothing happens;
     * you have to call init() again to restart iterating.
     */
    cObject *operator++(int)  {sQElem *t=p; if(p) p=p->next; return t->obj;} // FIXME: fails if p=NULL!!!

    /**
     * Returns the current object, then moves the iterator to the previous item.
     * If the iterator has reached either end of the queue, nothing happens;
     * you have to call init() again to restart iterating.
     */
    cObject *operator--(int)  {sQElem *t=p; if(p) p=p->prev; return t->obj;} // FIXME: fails if p=NULL!!!
};

#endif

