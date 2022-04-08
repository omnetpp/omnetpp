//==========================================================================
//  CSOFTOWNER.H - part of
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

#ifndef __OMNETPP_CSOFTOWNER_H
#define __OMNETPP_CSOFTOWNER_H

#include "cownedobject.h"

namespace omnetpp {


/**
 * @brief Internal class, used as a base class for modules and channels. It is not
 * intended for subclassing outside the simulation kernel.
 *
 * cSoftOwner acts as a "soft owner" (see object ownership discussion
 * in cOwnedObject documentation).
 *
 * @ingroup Fundamentals
 */
class SIM_API cSoftOwner : public cNoncopyableOwnedObject
{
    friend class cObject;
    friend class cOwnedObject;
    friend class cChannelType;

  private:
    cOwnedObject **objs = nullptr; // array of owned objects
    int numObjs = 0;         // number of elements used in objects[] (0..num-1)
    int capacity = 0;        // allocated size of objs[]

#ifdef SIMFRONTEND_SUPPORT
  private:
    int64_t lastChangeSerial;
#endif

  private:
    void doInsert(cOwnedObject *obj);
    virtual void ownedObjectDeleted(cOwnedObject *obj) override;
    virtual void yieldOwnership(cOwnedObject *obj, cObject *newOwner) override;
  protected:
    void reportUndisposed();
    virtual void objectStealingOnDeletion(cOwnedObject *obj);

  public:
    // internal: constructor for the global cSoftOwner instance globalOwningContext which has no owner
    explicit cSoftOwner(const char *name, bool namepooling, internal::Void *dummy) : cNoncopyableOwnedObject(name, namepooling, dummy) {}

#ifdef SIMFRONTEND_SUPPORT
    // internal: used by the UI to optimize refreshes
    void updateLastChangeSerial()  {lastChangeSerial = changeCounter++;}
    virtual bool hasChangedSince(int64_t lastRefreshSerial);
#endif

  protected:
    /** @name Redefined cObject member functions */
    //@{

    /**
     * Redefined.
     */
    virtual void take(cOwnedObject *obj) override;

    /**
     * Redefined.
     */
    virtual void drop(cOwnedObject *obj) override;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    explicit cSoftOwner(const char *name=nullptr, bool namepooling=true);

    /**
     * Destructor. The contained objects will be reported as undisposed.
     */
    virtual ~cSoftOwner();
    //@}

    /** @name Redefined cObject methods. */
    //@{
    /**
     * Returns true.
     */
    virtual bool isSoftOwner() const override {return true;}

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
    //@}

    /** @name Container functions. */
    // Note: we need long method names here because cModule subclasses from this class
    //@{

    /**
     * Returns the number of owned objects.
     */
    int getNumOwnedObjects() const {return numObjs;}

    /**
     * Returns the kth owned object. k must be between 0 and getNumOwnedObjects()-1
     * (inclusive). If the index is out of bounds, nullptr is returned.
     */
    cOwnedObject *getOwnedObject(int k);
    //@}
};

}  // namespace omnetpp


#endif

