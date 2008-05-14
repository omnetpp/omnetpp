//==========================================================================
//  CDEFAULTLIST.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cDefaultList : holds a set of cOwnedObjects
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CDEFAULTLIST_H
#define __CDEFAULTLIST_H

#include "cownedobject.h"

NAMESPACE_BEGIN


/**
 * Internal class, used as a base class for cModule. cDefaultList acts
 * as a "soft owner" (see object ownership discussion in cOwnedObject documentation).
 * Do not subclass your own classes from cDefaultList.
 *
 * @ingroup Containers
 */
class SIM_API cDefaultList : public cNoncopyableOwnedObject
{
  private:
    friend class cObject;
    friend class cOwnedObject;
    cOwnedObject **vect; // vector of objects
    int capacity;        // allocated size of vect[]
    int size;            // number of elements used in vect[] (0..size-1)
  public:
    static bool doGC; // whether garbage collection is necessary in destructor

  private:
    void construct();
    void doInsert(cOwnedObject *obj);
    virtual void ownedObjectDeleted(cOwnedObject *obj);
    virtual void yieldOwnership(cOwnedObject *obj, cObject *newOwner);

  public:
    // internal: called from module creation code in ctypes.cc
    void takeAllObjectsFrom(cDefaultList& other);

  protected:
    /** @name Redefined cObject member functions */
    //@{

    /**
     * Redefined.
     */
    void take(cOwnedObject *obj);

    /**
     * Redefined.
     */
    void drop(cOwnedObject *obj);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cDefaultList(const char *name=NULL);

    /**
     * Destructor. The contained objects will be deleted.
     */
    virtual ~cDefaultList();
    //@}

    /** @name Redefined cObject methods. */
    //@{
    /**
     * Returns true.
     */
    virtual bool isSoftOwner()  {return true;}

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
     * Packing and unpacking cannot be supported with this class.
     * This method raises an error.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Packing and unpacking cannot be supported with this class.
     * This method raises an error.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Container functions. */
    // Note: we need long method names here because cModule subclasses from this class
    //@{

    /**
     * Returns the number of elements stored.
     */
    int defaultListSize() const {return size;}

    /**
     * Get the element at the given position. k must be between 0 and
     * defaultListSize()-1 (inclusive). If the index is out of bounds,
     * NULL is returned.
     */
    cOwnedObject *defaultListGet(int k);

    /**
     * Returns true if the set contains the given object, false otherwise.
     */
    // Note: we need a long name here because cModule subclasses from this
    bool defaultListContains(cOwnedObject *obj) const;
    //@}
};

NAMESPACE_END


#endif

