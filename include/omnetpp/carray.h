//==========================================================================
//  CARRAY.H - part of
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

#ifndef __OMNETPP_CARRAY_H
#define __OMNETPP_CARRAY_H

#include "cownedobject.h"

namespace omnetpp {


/**
 * @brief Vector-like container for objects derived from cObject.
 *
 * cArray stores pointers of objects. When the array is full, the its capacity
 * grows automatically by a delta.
 *
 * Ownership of cOwnedObjects may be controlled by invoking setTakeOwnership()
 * prior to inserting objects. Objects that cannot track their ownership
 * (cObject but not cOwnedObject) are always treated as owned. Whether an
 * object is owned or not affects the operation of the destructor, clean(),
 * the copy constructor and the dup() method.
 *
 * @ingroup Misc
 */
class SIM_API cArray : public cOwnedObject
{
  public:
    /**
     * @brief Iterates through elements in a cArray, skipping holes (slots containing
     * nullptr).
     *
     * Usage:
     * ```
     * for (cArray::Iterator it(array); !it.end(); ++it) {
     *     cObject *item = *it;
     *     ...
     * }
     * ```
     */
    class Iterator
    {
      private:
        cArray *array;
        int k;

     private:
        void advance();
        void retreat();

      public:
        /**
         * Constructor. Iterator will walk on the array passed as argument.
         * The starting object will be the first (if atHead==true) or
         * the last (atHead==false) object in the array, not counting empty slots.
         */
        Iterator(const cArray& a, bool atHead=true)  {init(a, atHead);}

        /**
         * Reinitializes the iterator object.
         */
        void init(const cArray& a, bool atHead=true);

        /**
         * Returns a pointer to the current object, or nullptr if
         * the iterator is not at a valid position.
         */
        cObject *operator*() const {return array->get(k);}

        /**
         * Returns true if the iterator has reached either end of the array.
         */
        bool end() const   {return k<0 || k>=array->size();}

        /**
         * Prefix increment operator (++it). Moves the iterator to the next
         * non-empty slot in the array. It has no effect if the iterator has
         * reached either end of the array.
         */
        Iterator& operator++() {if (!end()) advance(); return *this;}

        /**
         * Postfix increment operator (it++). Moves the iterator to the next
         * non-empty slot in the array, and returns the iterator's previous state.
         * It has no effect if the iterator has reached either end of the array.
         */
        Iterator operator++(int) {Iterator tmp(*this); if (!end()) advance(); return tmp;}

        /**
         * Prefix decrement operator (--it). Moves the iterator to the previous
         * non-empty slot in the array. It has no effect if the iterator has
         * reached either end of the array.
         */
        Iterator& operator--() {if (!end()) retreat(); return *this;}

        /**
         * Postfix decrement operator (it--). Moves the iterator to the previous
         * non-empty slot in the array, and returns the iterator's previous state.
         * It has no effect if the iterator has reached either end of the array.
         */
        Iterator operator--(int) {Iterator tmp(*this); if (!end()) retreat(); return tmp;}
    };

  private:
    enum {FL_TKOWNERSHIP = 4};
    cObject **vect = nullptr; // vector of objects
    int capacity = 0;         // allocated size of vect[]
    int delta;                // if needed, grows by delta
    int firstfree = 0;        // first free position in vect[]
    int last = -1;            // last used position

  private:
    void copy(const cArray& other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects that are owned by cArray
     * will be duplicated so that the new cArray will have its own
     * copy of them.
     */
    cArray(const cArray& list);

    /**
     * Constructor. The initial capacity of the container and the delta
     * (by which the capacity will grow if it gets full) can be specified.
     */
    explicit cArray(const char *name=nullptr, int capacity=0, int delta=10);

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cArray();

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    cArray& operator=(const cArray& list);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     * Contained objects that are owned by cArray will be duplicated
     * so that the new cArray will have its own copy of them.
     */
    virtual cArray *dup() const override  {return new cArray(*this);}

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

    /** @name Container functions. */
    //@{

    /**
     * Returns the index of last used position+1. This only equals the
     * number of contained objects if there are no "holes" (nullptr elements)
     * in the array.
     */
    virtual int size() const {return last+1;}

    /**
     * Makes the container empty. Contained objects that were owned by the
     * container will be deleted.
     */
    virtual void clear();

    /**
     * Returns the allocated size of the underlying array.
     */
    virtual int getCapacity() const {return capacity;}

    /**
     * Resizes the the underlying array, without changing the contents of
     * this array. The specified capacity cannot be less than size().
     */
    virtual void setCapacity(int capacity);

    /**
     * Inserts the object into the array. Only the pointer of the object
     * will be stored. The return value is the object's index in the
     * array.
     */
    virtual int add(cObject *obj);

    /**
     * Inserts the object into the array at the given position. If
     * the position is occupied, the function throws a cRuntimeError.
     * The return value is the object's index in the array.
     */
    virtual int addAt(int m, cObject *obj);

    /**
     * Inserts the object into the array. If the array already contains
     * an object with the same name, it will be replaced (hashtable-like
     * behavior.) The replaced object, if it was owned by the container,
     * is deleted using discard().
     * The return value is the object's index in the array.
     */
    virtual int set(cObject *obj);

    /**
     * Searches the array for the pointer of the object passed and returns
     * the index of the first match. If the object was not found, -1 is
     * returned.
     */
    virtual int find(cObject *obj) const;

    /**
     * Returns the index of the first item in the array that has the
     * name pointed to by s (cObject::isName() is used.)
     * If no such item was found, -1 is returned.
     */
    virtual int find(const char *objname) const;

    /**
     * Returns reference to the mth object in the array. Returns nullptr
     * if the mth position is not used.
     */
    virtual cObject *get(int m);

    /**
     * Returns reference to the first object in the array with name s.
     * Returns nullptr if no object with the given name was found.
     */
    virtual cObject *get(const char *objname);

    /**
     * Returns reference to the mth object in the array. Returns nullptr
     * if the mth position is not used.
     */
    virtual const cObject *get(int m) const;

    /**
     * Returns reference to the first object in the array with name s.
     * Returns nullptr if no object with the given name was found.
     */
    virtual const cObject *get(const char *objname) const;

    /**
     * The same as get(int). With the indexing operator,
     * cArray can be used as a vector.
     */
    cObject *operator[](int m)  {return get(m);}

    /**
     * The same as get(const char *). With the indexing operator,
     * cArray can be used as a vector.
     */
    cObject *operator[](const char *objname)  {return get(objname);}

    /**
     * The same as get(int). With the indexing operator,
     * cArray can be used as a vector.
     */
    const cObject *operator[](int m) const  {return get(m);}

    /**
     * The same as get(const char *). With the indexing operator,
     * cArray can be used as a vector.
     */
    const cObject *operator[](const char *objname) const  {return get(objname);}

    /**
     * Returns true if position m is used in the array, otherwise false.
     */
    virtual bool exist(int m) const  {return m>=0 && m<=last && vect[m]!=nullptr;}

    /**
     * Returns true if the array contains an object with the given name,
     * otherwise false.
     */
    virtual bool exist(const char *objname) const  {return find(objname)!=-1;}

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    virtual cObject *remove(int m);

    /**
     * Removes the object given with its index/name/pointer from the
     * container. (If the object was owned by the container, drop()
     * is called.)
     */
    virtual cObject *remove(const char *objname);

    /**
     * Removes the object given with its index/name/pointer from the
     * container, and returns the same pointer. If the object was not
     * found, nullptr is returned. (If the object was owned by the container, drop()
     * is called.)
     */
    virtual cObject *remove(cObject *obj);
    //@}

    /** @name Ownership control flag.
     *
     * The ownership control flag is to be used by derived container classes.
     * If the flag is set, the container should take() any object that is
     * inserted into it.
     */
    //@{

    /**
     * Sets the flag which determines whether the container object should
     * automatically take ownership of the objects that are inserted into it.
     * It does not affect objects already in the queue. When an inserted
     * object is owned by the container, that means it will be deleted when
     * the container object is deleted or cleared, and will be duplicated when
     * the container object is duplicated or copied.
     *
     * Setting the flag to false does not affect the treatment of objects
     * that are NOT cOwnedObject. Since they do not support the ownership
     * protocol, they will always be treated by the container.
     */
    void setTakeOwnership(bool tk) {setFlag(FL_TKOWNERSHIP,tk);}

    /**
     * Returns the flag which determines whether the container object
     * should automatically take ownership of the objects that are inserted
     * into it. See setTakeOwnedship() for more details.
     */
    bool getTakeOwnership() const   {return flags&FL_TKOWNERSHIP;}
    //@}
};

}  // namespace omnetpp


#endif

