//==========================================================================
//  CQUEUE.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cQueue : queue of cObject descendants
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CQUEUE_H
#define __CQUEUE_H

#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * Type for comparison functions for cObject. Return value should be:
 * - less than zero if a < b
 * - greater than zero if a > b
 *
 * @ingroup EnumsTypes
 */
typedef int (*CompareFunc)(cObject *a, cObject *b);


/**
 * Queue class for objects derived from cObject. The default behaviour of
 * cQueue is a FIFO: you insert elements at the back using insert(), and
 * remove them at the front using pop().
 *
 * cQueue may be set up to act as a priority queue. This requires the user to specify
 * a comparison function.
 *
 * By default, cQueue's destructor deletes all contained objects. This behaviour
 * can be changed by calling setTakeOwnership(false) before inserting objects.
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
        cObject *obj; // the contained object
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
         * <tt>++</tt>) or reverse (back-to-front, using <tt>--</tt>) iteration.
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
        cObject *operator()()  {return p ? p->obj : NULL;}

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
        cObject *operator++(int)  {if (!p) return NULL; cObject *r=p->obj; p=p->next; return r;}

        /**
         * Returns the current object, then moves the iterator to the previous item
         * (towards the front of the queue). If the iterator has previously
         * reached either end of the queue, nothing happens, and one has to
         * call init() to restart iterating.
         */
        cObject *operator--(int)  {if (!p) return NULL; cObject *r=p->obj; p=p->prev; return r;}
    };

    friend class Iterator;

  private:
    bool tkownership;
    QElem *frontp, *backp;  // inserting at back(), removal at front()
    int n;  // number of items in the queue
    CompareFunc compare;   // comparison function; NULL for FIFO

  protected:
    // internal functions
    QElem *find_qelem(cObject *obj) const;
    void insbefore_qelem(QElem *p, cObject *obj);
    void insafter_qelem(QElem *p, cObject *obj);
    cObject *remove_qelem(QElem *p);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor. When comparison function argument is NULL, the queue will
     * act as FIFO, otherwise as priority queue.
     */
    cQueue(const char *name = NULL, CompareFunc cmp=NULL);

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
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Setup, insertion and removal functions. */
    //@{
    /**
     * Sets the comparator function. This only affects future insertions,
     * i.e. the queue's current content will not be re-sorted.
     */
    virtual void setup(CompareFunc cmp);

    /**
     * Adds an element to the back of the queue. Trying to insert a
     * NULL pointer is an error (throws cRuntimeError).
     */
    virtual void insert(cObject *obj);

    /**
     * Inserts exactly before the given object. If the given position
     * does not exist or if you try to insert a NULL pointer,
     * cRuntimeError is thrown.
     */
    virtual void insertBefore(cObject *where, cObject *obj);

    /**
     * Inserts exactly after the given object. If the given position
     * does not exist or if you try to insert a NULL pointer,
     * cRuntimeError is thrown.
     */
    virtual void insertAfter(cObject *where, cObject *obj);

    /**
     * Unlinks and returns the object given. If the object is not in the
     * queue, NULL pointer is returned.
     */
    virtual cObject *remove(cObject *obj);

    /**
     * Unlinks and returns the front element in the queue. If the queue
     * was empty, cRuntimeError is thrown.
     */
    virtual cObject *pop();

    /**
     * Empties the container. Contained objects that were owned by the
     * queue (see getTakeOwnership()) will be deleted.
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
    virtual cObject *front() const;

    /**
     * Returns pointer to the last (back) element in the queue.
     * This is the element most recently added by insert().
     * Returns NULL if the queue is empty.
     */
    virtual cObject *back() const;

    /**
     * Returns the number of objects contained in the queue.
     */
    virtual int getLength() const;

    /**
     * Returns true if the queue is empty.
     */
    bool isEmpty() const {return getLength()==0;}

    /**
     * Alias for getLength().
     */
    int length() const {return getLength();}

    /**
     * Alias for isEmpty().
     */
    bool empty() const {return isEmpty();}

    /**
     * Returns the ith element in the queue, or NULL if i is out of range.
     * get(0) returns the front element. This method performs linear
     * search.
     */
    cObject *get(int i) const;

    /**
     * Returns true if the queue contains the given object.
     */
    virtual bool contains(cObject *obj) const;
    //@}

    /** @name Ownership control flag. */
    //@{

    /**
     * Sets the flag which determines whether the container object should
     * automatically take ownership of the objects that are inserted into it.
     * It does not affect objects already in the queue. When an inserted
     * object is owned by the queue, that means it will be deleted when
     * the queue object is deleted or cleared, and will be duplicated when
     * the queue object is duplicated or copied.
     *
     * Setting the flag to false does not affect the treatment of objects
     * that are NOT cOwnedObject. Since they do not support the ownership
     * protocol, they will always be treated by the queue as owned objects.
     */
    void setTakeOwnership(bool tk) {tkownership=tk;}

    /**
     * Returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it. See setTakeOwnedship() for more details.
     */
    bool getTakeOwnership() const {return tkownership;}
    //@}
};

NAMESPACE_END


#endif

