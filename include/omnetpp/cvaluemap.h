//==========================================================================
//   CVALUEMAP.H  - part of
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

#ifndef __OMNETPP_CVALUEMAP_H
#define __OMNETPP_CVALUEMAP_H

#include <string>
#include <map>
#include "cvalue.h"
#include "cownedobject.h"

namespace omnetpp {

/**
 * @brief Key-value container class based on std::map<std::string,cValue>.
 *
 * The added value is ownership handling of object values (cObject* pointers)
 * contained in cValue objects. Guidelines: owned objects are those that
 * subclass from cOwnedObject and getOwner() returns this object, and all
 * cObjects that are non-cOwnedObject.
 *
 * @ingroup SimSupport
 */
class SIM_API cValueMap : public cOwnedObject
{
  private:
    std::map<std::string,cValue> fields;

  private:
    void copy(const cValueMap& other);
    void takeValue(const cValue& value);
    void dropAndDeleteValue(const cValue& value);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects that are owned by cValueArray
     * will be duplicated so that the new cValueArray will have its own
     * copy of them.
     */
    cValueMap(const cValueMap& other) {copy(other);}

    /**
     * Constructor.
     */
    explicit cValueMap(const char *name=nullptr) : cOwnedObject(name) {}

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cValueMap();

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject's operator=() for more details.
     * Duplication and assignment work all right with cValueMap.
     * Contained objects that are owned by cValueMap will be duplicated
     * so that the new cValueMap will have its own copy of them.
     */
    cValueMap& operator=(const cValueMap& other);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Duplication and assignment are supported by cValueMap.
     * Contained objects that are owned by cValueMap will be duplicated
     * so that the new cValueMap will have its own copy of them.
     */
    virtual cValueMap *dup() const override  {return new cValueMap(*this);}

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
     * Returns the number of entries in the map.
     */
    virtual int size() const {return fields.size();}

    /**
     * Makes the container empty.
     */
    virtual void clear();

    /**
     * Adds or overwrites an entry in the map.
     */
    virtual void set(const char *key, const cValue& value);

    /**
     * Returns true if the given key is present in the map.
     */
    virtual bool containsKey(const char *key) const;

    /**
     * Returns the value with the given key.
     */
    virtual const cValue& get(const char *key) const;

    /**
     * Equivalent to get(const char *) const.
     */
    const cValue& operator[](const char *key) const  {return get(key);}

    /**
     * Erases the entry with the given key.
     */
    virtual void erase(const char *key);

    /**
     * Provides readonly access to the underlying std::map.
     */
    const std::map<std::string,cValue>& getFields() const {return fields;}
    //@}
};


}  // namespace omnetpp

#endif


