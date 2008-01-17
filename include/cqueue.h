//==========================================================================
//  CQUEUE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cQueue        : (optionally) sorted queue of cOwnedObjects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CQUEUE_H
#define __CQUEUE_H

#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * Queue class for objects derived from cOwnedObject. The default behaviour of
 * cQueue is a FIFO: you insert elements at the back using insert(), and
 * remove them at the front using pop().
 *
 * By default, cQueue's destructor deletes all contained objects. This behaviour
 * can be changed by calling takeOwnership(false) before inserting objects.
 * More precisely, the behaviour can be controlled per-object: the
 * insertion-time state of the <i>takeOwnership</i> flag will determine
 * whether the inserted object will be deleted by the cQueue destructor or not.
 *
 * @see Iterator
 * @ingroup Containers
 */
class SIM_API cQueue : public cOwnedObject
{
  private:
    struct QElem
    {
        cOwnedObject *obj; // contained object
        QElem *prev;  // element towards the front of the queue
        QElem *next;  // element towards the back of the queue
    };

  public:
    /**
     * Walks along a cQueue.
     */
    class Iterator
    {
      private:
        QElem *p;

      public:
        /**
         * Constructor. Iterator will walk on the queue passed as argument.
         * The iterator can be initialized for forward (front-to-back, using
         * <tt>++</rr>) or reverse (back-to-front, using <tt>--</tt>) iteration.
         */
        Iterator(const cQueue& q, bool reverse=false)
                {p=&q ? (reverse ? q.backp : q.frontp) : NULL;}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cQueue& q, bool reverse=false)
                {p=&q ? (reverse ? q.backp : q.frontp) : NULL;}

        /**
         * Returns the current object.
         */
        cOwnedObject *operator()()  {return p ? p->obj : NULL;}

        /**
         * Returns true if the iterator has reached either end of the queue.
         */
        bool end() const   {return (bool)(p==NULL);}

        /**
         * Returns the current object, then moves the iterator to the next item
         * (towards the back of the queue). If the iterator has previously
         * reached either end of the queue, nothing happens, and one has to
         * call init() to restart iterating.
         */
        cOwnedObject *operator++(int)  {if (!p) return NULL; cOwnedObject *r=p->obj; p=p->next; return r;}

        /**
         * Returns the current object, then moves the iterator to the previous item
         * (towards the front of the queue). If the iterator has previously
         * reached either end of the queue, nothing happens, and one has to
         * call init() to restart iterating.
         */
        cOwnedObject *operator--(int)  {if (!p) return NULL; cOwnedObject *r=p->obj; p=p->prev; return r;}
    };

    friend class Iterator;

  private:
    bool tkownership;
    QElem *frontp, *backp;  // inserting at back(), removal at front()
    int n;  // number of items in the queue

  protected:
    // internal functions
    QElem *find_qelem(cOwnedObject *obj) const;
    void insbefore_qelem(QElem *p, cOwnedObject *obj);
    void insafter_qelem(QElem *p, cOwnedObject *obj);
    cOwnedObject *remove_qelem(QElem *p);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cQueue(const char *name = NULL);

    /**
     * Copy constructor. Contained objects that are owned by the queue
     * will be duplicated so that the new queue will have its own copy
     * of them.
     */
    cQueue(const cQueue& queue);

    /**
     * Destructor. Deletes all contained objects that were owned by it.
     */
    virtual ~cQueue();

    /**
     * Assignment operator. Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     *
     * The name member doesn't get copied; see cNamedObject's operator=() for more details.
     */
    cQueue& operator=(const cQueue& queue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Duplication and assignment work all right with cQueue.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    virtual cQueue *dup() const  {return new cQueue(*this);}

    /**
     * Produces a one-line description of object contents.
     * See cObject for more details.
     */
    virtual std::string info() const;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v);

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Setup, insertion and removal functions. */
    //@{
    /**
     * Adds an element to the back of the queue. Trying to insert a
     * NULL pointer is an error (throws cRuntimeError).
     */
    virtual void insert(cOwnedObject *obj);

    /**
     * Inserts exactly before the given object. If the given position
     * does not exist or if you try to insert a NULL pointer,
     * cRuntimeError is thrown.
     */
    virtual void insertBefore(cOwnedObject *where, cOwnedObject *obj);

    /**
     * Inserts exactly after the given object. If the given position
     * does not exist or if you try to insert a NULL pointer,
     * cRuntimeError is thrown.
     */
    virtual void insertAfter(cOwnedObject *where, cOwnedObject *obj);

    /**
     * Unlinks and returns the object given. If the object is not in the
     * queue, NULL pointer is returned.
     */
    virtual cOwnedObject *remove(cOwnedObject *obj);

    /**
     * Unlinks and returns the front element in the queue. If the queue
     * was empty, cRuntimeError is thrown.
     */
    virtual cOwnedObject *pop();

    /**
     * Empties the container. Contained objects that were owned by the
     * queue (see takeOwnership()) will be deleted.
     */
    virtual void clear();
    //@}

    /** @name Query functions. */
    //@{
    /**
     * Returns pointer to the object at the front of the queue.
     * This is the element to be return by pop().
     * Returns NULL if the queue is empty.
     */
    virtual cOwnedObject *front() const;

    /**
     * Returns pointer to the last (back) element in the queue.
     * This is the element most recently added by insert().
     * Returns NULL if the queue is empty.
     */
    virtual cOwnedObject *back() const;

    /**
     * Returns the number of objects contained in the queue.
     */
    virtual int length() const;

    /**
     * Returns true if the queue is empty.
     */
    bool empty() const {return length()==0;}

    /**
     * Returns the ith element in the queue, or NULL if i is out of range.
     * get(0) returns the front element. This method performs linear
     * search.
     */
    cOwnedObject *get(int i) const;

    /**
     * Returns true if the queue contains the passed object.
     */
    virtual bool contains(cOwnedObject *obj) const;
    //@}

    /** @name Ownership control flag. */
    //@{

    /**
     * Sets the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    void takeOwnership(bool tk) {tkownership=tk;}

    /**
     * Returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it.
     */
    bool takeOwnership() const   {return tkownership;}
    //@}
};

NAMESPACE_END


#endif

