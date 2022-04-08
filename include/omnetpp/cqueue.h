//==========================================================================
//  CQUEUE.H - part of
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

#ifndef __OMNETPP_CQUEUE_H
#define __OMNETPP_CQUEUE_H

#include "cownedobject.h"

namespace omnetpp {


/**
 * @brief Queue class for objects derived from cObject.
 *
 * The default behavior of cQueue is a FIFO: you insert elements at the back
 * using insert(), and remove them at the front using pop().
 *
 * cQueue may be set up to act as a priority queue. This requires the user to
 * supply a comparison function.
 *
 * Ownership of cOwnedObjects may be controlled by invoking setTakeOwnership()
 * prior to inserting objects. Objects that cannot track their ownership
 * (cObject but not cOwnedObject) are always treated as owned. Whether an
 * object is owned or not affects the operation of the destructor, clean(),
 * the copy constructor and the dup() method.
 *
 * @see Iterator
 * @ingroup SimProgr
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
     * @brief Base class for object comparators, used by cQueue for
     * priority queuing.
     */
    class SIM_API Comparator
    {
      public:
        virtual ~Comparator() {}
        virtual Comparator *dup() const = 0;
        virtual bool less(cObject *a, cObject *b) = 0;
    };

    /**
     * @brief Function for comparing two cObjects, used by cQueue for
     * priority queuing.
     *
     * The return value should be:
     * - less than zero if a < b
     * - greater than zero if a > b
     * - zero if a == b
     *
     * @ingroup Misc
     */
    typedef int (*CompareFunc)(cObject *a, cObject *b);

    /**
     * @brief Walks along a cQueue.
     */
    class SIM_API Iterator
    {
      private:
        QElem *p;

      public:
        /**
         * Constructor. Iterator will walk on the queue passed as argument.
         * The iterator can be initialized for forward (front-to-back, using
         * <tt>++</tt>) or reverse (back-to-front, using <tt>--</tt>) iteration.
         */
        Iterator(const cQueue& q, bool reverse=false) { init(q, reverse);}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cQueue& q, bool reverse=false) {p = reverse ? q.backp : q.frontp;}

        /**
         * Returns the current object.
         */
        cObject *operator*() const {return p ? p->obj : nullptr;}

        /**
         * Returns true if the iterator has reached either end of the queue.
         */
        bool end() const {return p == nullptr;}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next object
         * in the queue. It has no effect if the iterator has reached either
         * end of the queue.
         */
        Iterator& operator++() {if (!end()) p = p->next; return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next object
         * in the queue, and returns the iterator's previous state. It has
         * no effect if the iterator has reached either end of the queue.
         */
        Iterator operator++(int) {Iterator tmp(*this); if (!end()) p = p->next; return tmp;}

        /**
         * Prefix decrement operator (--it). Moves the iterator to the previous object
         * in the queue. It has no effect if the iterator has reached either
         * end of the queue.
         */
        Iterator& operator--() {if (!end()) p = p->prev; return *this;}

        /**
         * Postfix decrement operator (it--). Moves the iterator to the previous object
         * in the queue, and returns the iterator's previous state. It has
         * no effect if the iterator has reached either end of the queue.
         */
        Iterator operator--(int) {Iterator tmp(*this); if (!end()) p = p->prev; return tmp;}
    };

    friend class Iterator;

  private:
    bool takeOwnership = true;
    QElem *frontp = nullptr, *backp = nullptr;  // front and back pointers
    int len = 0;  // number of items in the queue
    Comparator *comparator = nullptr; // comparison functor; nullptr for FIFO

  private:
    void copy(const cQueue& other);

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
     * Constructor. When comparator argument is nullptr, the queue will
     * act as FIFO, otherwise as priority queue.
     */
    cQueue(const char *name=nullptr, Comparator *cmp=nullptr);

    /**
     * Constructor. Sets up cQueue as a priority queue.
     */
    cQueue(const char *name, CompareFunc cmp);

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
     * Assignment operator. The name member is not copied; see
     * cNamedObject::operator=() for details. Contained objects that are
     * owned by the queue will be duplicated so that the new queue will have
     * its own copy of them.
     */
    cQueue& operator=(const cQueue& queue);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * Contained objects that are owned by the queue will be duplicated
     * so that the new queue will have its own copy of them.
     */
    virtual cQueue *dup() const override  {return new cQueue(*this);}

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

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Setup, insertion and removal functions. */
    //@{
    /**
     * Sets the comparator. This only affects future insertions,
     * i.e. the queue's current content will not be re-sorted.
     */
    virtual void setup(Comparator *cmp);

    /**
     * Sets the comparator function. This only affects future insertions,
     * i.e. the queue's current content will not be re-sorted.
     */
    virtual void setup(CompareFunc cmp);

    /**
     * Adds an element to the back of the queue. Trying to insert nullptr
     * is an error (throws cRuntimeError).
     */
    virtual void insert(cObject *obj);

    /**
     * Inserts exactly before the given object. If the given position
     * does not exist or if you try to insert nullptr, a cRuntimeError
     * is thrown.
     */
    virtual void insertBefore(cObject *where, cObject *obj);

    /**
     * Inserts exactly after the given object. If the given position
     * does not exist or if you try to insert nullptr, a cRuntimeError
     * is thrown.
     */
    virtual void insertAfter(cObject *where, cObject *obj);

    /**
     * Unlinks and returns the object given. If the object is not in the
     * queue, nullptr is returned.
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
     * This is the element to be returned by pop().
     * Returns nullptr if the queue is empty.
     */
    virtual cObject *front() const;

    /**
     * Returns pointer to the last (back) element in the queue.
     * This is the element most recently added by insert().
     * Returns nullptr if the queue is empty.
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
     * DEPRECATED. Use getLength() instead.
     */
    [[deprecated("use getLength() instead")]] int length() const {return getLength();}

    /**
     * DEPRECATED. Use isEmpty() instead.
     */
    [[deprecated("use isEmpty() instead")]] bool empty() const {return isEmpty();}

    /**
     * Returns the ith element in the queue, or nullptr if i is out of range.
     * get(0) returns the front element. This method performs linear
     * search.
     */
    virtual cObject *get(int i) const;

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
    void setTakeOwnership(bool tk) {takeOwnership=tk;}

    /**
     * Returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it. See setTakeOwnedship() for more details.
     */
    bool getTakeOwnership() const {return takeOwnership;}
    //@}
};

// for backward compatibility
typedef cQueue::CompareFunc CompareFunc;

}  // namespace omnetpp


#endif

