//==========================================================================
//   CDEFAULTLIST.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDefaultList : holds a set of cObjects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDEFAULTLIST_H
#define __CDEFAULTLIST_H

#include "cobject.h"


/**
 * Container object that holds objects derived from cObject.
 * cDefaultList stores the pointers of the objects inserted instead of making copies.
 * cDefaultList works as an array, but if it gets full, it grows automatically by
 * a specified delta.
 *
 * cDefaultList owns all objects inserted.
 *
 * @ingroup Containers
 */
class SIM_API cDefaultList : public cObject
{
  private:
    friend class cObject;
    cObject **vect;   // vector of objects
    int size;         // size of vector
    int delta;        // if needed, grows by delta
    int count;        // number of elements stored

  private:
    void construct();
    void doInsert(cObject *obj);
    virtual void ownedObjectDeleted(cObject *obj);
    virtual void yieldOwnership(cObject *obj, cObject *newOwner);

  public:
    // internal: called from module creation code in ctypes.cc
    void takeAllObjectsFrom(cDefaultList& other);

  protected:
    /** @name Redefined cObject member functions */
    //@{

    /**
     * Redefined.
     */
    void take(cObject *obj);

    /**
     * Redefined.
     */
    void drop(cObject *obj);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Throws exception.
     */
    cDefaultList(const cDefaultList& list);

    /**
     * Constructor.
     */
    explicit cDefaultList(const char *name=NULL);

    /**
     * Destructor. The contained objects will be deleted.
     */
    virtual ~cDefaultList();

    /**
     * Assignment operator. Throws exception.
     */
    cDefaultList& operator=(const cDefaultList& list);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Duplication. Throws exception.
     */
    virtual cObject *dup() const  {return new cDefaultList(*this);}

    /**
     * Returns true.
     */
    virtual bool isSoftOwner()  {return true;}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Calls the given function for each object contained.
     */
    virtual void forEach(ForeachFunc f);

    /**
     * Packing and unpacking cannot be supported with this class.
     * This methods raises an error.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Packing and unpacking cannot be supported with this class.
     * This methods raises an error.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Container functions. */
    // Note: we need long method names here because cModule subclasses from this class
    //@{

    /**
     * Returns the number of elements stored.
     */
    int defaultListItems() const {return count;}

    /**
     * Get the element at the given position. k must be between 0 and
     * items()-1 (inclusive). Note that elements may move as the result
     * of add() and remove() operations. If the index is out of bounds,
     * NULL is returned.
     */
    cObject *defaultListGet(int k);

    /**
     * Returns true if the set contains the given object, false otherwise.
     */
    // Note: we need a long name here because cModule subclasses from this
    bool defaultListContains(cObject *obj) const;
    //@}
};

#endif

