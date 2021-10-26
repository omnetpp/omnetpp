//==========================================================================
//   CVALUEHOLDER.H  - part of
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

#ifndef __OMNETPP_CVALUEHOLDER_H
#define __OMNETPP_CVALUEHOLDER_H

#include <string>
#include <vector>
#include "cvalue.h"
#include "cownedobject.h"


namespace omnetpp {

/**
 * @brief cOwnedObject-based container/wrapper to hold a single cValue.
 *
 * The added value is ownership handling of object values (cObject* pointers)
 * contained in cValue objects. Guidelines: owned objects are those that
 * subclass from cOwnedObject and getOwner() returns this object, and all
 * cObjects that are non-cOwnedObject.
 *
 * @ingroup SimSupport
 */
class SIM_API cValueHolder : public cOwnedObject
{
  private:
    cValue value;

  private:
    void copy(const cValueHolder& other);
    void takeValue();
    void dropAndDeleteValue();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects will be duplicated so that the
     * new cValueHolder will have its own copy of them.
     *
     * Note that this behavior is different from most other container classes
     * in the simulation library like cQueue, which only duplicate the objects
     * they own, and leave externally owned objects alone.
     */
    cValueHolder(const cValueHolder& other) : cOwnedObject(other) {copy(other);}

    /**
     * Constructor.
     */
    explicit cValueHolder(const char *name=nullptr) : cOwnedObject(name) {}

    /**
     * Constructor.
     */
    explicit cValueHolder(const char *name, const cValue& value) : cOwnedObject(name), value(value) {takeValue();}

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cValueHolder() {dropAndDeleteValue();}

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
     * Contained objects that are owned by cValueHolder will be duplicated
     * so that the new cValueHolder will have its own copy of them.
     */
    cValueHolder& operator=(const cValueHolder& other);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object. Contained objects
     * will be duplicated so that the new cValueHolder will have its own copy
     * of them.
     *
     * Note that this behavior is different from most other container classes
     * in the simulation library like cQueue, which only duplicate the objects
     * they own, and leave externally owned objects alone.
     */
    virtual cValueHolder *dup() const override  {return new cValueHolder(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override {return value.str();}

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor* v) override;

    /**
     * Serializes the object into an MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimPack(cCommBuffer* buffer) const override;

    /**
     * Deserializes the object from an MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual void parsimUnpack(cCommBuffer* buffer) override;
    //@}

    /** @name Container functions. */
    //@{
    /**
     * Returns the stored value.
     */
    const cValue& get() const {return value;}

    /**
     * Replaces the stored value.
     */
    void set(const cValue& v) {dropAndDeleteValue(); value = v; takeValue();}
    //@}

};


}  // namespace omnetpp

#endif


