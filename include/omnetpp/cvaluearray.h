//==========================================================================
//   CVALUEARRAY.H  - part of
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

#ifndef __OMNETPP_CVALUEARRAY_H
#define __OMNETPP_CVALUEARRAY_H

#include <string>
#include <vector>
#include "cvalue.h"
#include "cvaluecontainer.h"


namespace omnetpp {

/**
 * @brief Container class based on std::vector<cValue> objects.
 *
 * The added value is ownership handling of object values (cObject* pointers)
 * contained in cValue objects. Guidelines: owned objects are those that
 * subclass from cOwnedObject and getOwner() returns this object, and all
 * cObjects that are non-cOwnedObject.
 *
 * @ingroup Expressions
 */
class SIM_API cValueArray : public cValueContainer
{
  private:
    std::vector<cValue> array;

  private:
    void cannotCast(cObject *obj, const char *toClass) const;
    void copy(const cValueArray& other);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Copy constructor. Contained objects will be duplicated so that the
     * new cValueArray will have its own copy of them.
     *
     * Note that this behavior is different from most other container classes
     * in the simulation library like cQueue, which only duplicate the objects
     * they own, and leave externally owned objects alone.
     */
    cValueArray(const cValueArray& other) : cValueContainer(other) {copy(other);}

    /**
     * Constructor. The initial capacity of the container and the delta
     * (by which the capacity will grow if it gets full) can be specified.
     */
    explicit cValueArray(const char *name=nullptr, int capacity=0) : cValueContainer(name), array(capacity) {}

    /**
     * Destructor. The contained objects that were owned by the container
     * will be deleted.
     */
    virtual ~cValueArray();

    /**
     * Assignment operator. The name member is not copied;
     * see cNamedObject::operator=() for details.
     * Contained objects that are owned by cValueArray will be duplicated
     * so that the new cValueArray will have its own copy of them.
     */
    cValueArray& operator=(const cValueArray& other);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object. Contained objects
     * will be duplicated so that the new cValueArray will have its own copy
     * of them.
     *
     * Note that this behavior is different from most other container classes
     * in the simulation library like cQueue, which only duplicate the objects
     * they own, and leave externally owned objects alone.
     */
    virtual cValueArray *dup() const override  {return new cValueArray(*this);}

    /**
     * Produces a one-line description of the object's contents.
     * See cObject for more details.
     */
    virtual std::string str() const override;

    /**
     * Calls v->visit(this) for each contained object.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor* v) override;
    //@}

    /** @name Container functions. */
    //@{

    /**
     * Returns the number of elements.
     */
    virtual int size() const {return array.size();}

    /**
     * Makes the container empty.
     */
    virtual void clear();

    /**
     * Returns number of elements the array can hold without having to
     * reallocate the underlying storage.
     */
    virtual int getCapacity() const {return array.capacity();}

    /**
     * Reserves place for the given number of elements without changing
     * the contents of the array.
     */
    virtual void setCapacity(int capacity) {array.reserve(capacity);}

    /**
     * Adds the value at the end of the array.
     */
    virtual void add(const cValue& value);

    /**
     * Inserts the value at the given position. Subsequent elements will be
     * shifted up.
     */
    virtual void insert(int k, const cValue& value);

    /**
     * Returns the kth element in the array.
     */
    const virtual cValue& get(int k) const;

    /**
     * Replaces the kth element in the array.
     */
    void set(int k, const cValue& value);

    /**
     * Equivalent to get(int) const.
     */
    const cValue& operator[](int k) const  {return get(k);}

    /**
     * Erases the value given with its index. Subsequent elements will be
     * shifted down.
     */
    virtual void erase(int k);

    /**
     * Removes the kth element from the array, and replaces its slot with the
     * undefined value.
     */
    virtual cValue remove(int k);

    /**
     * Provides readonly access to the underlying std::vector.
     */
    const std::vector<cValue>& getArray() const {return array;}
    //@}

    /** @name Convenience getters. */
    //@{

    /**
     * Converts each element to bool, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<bool> asBoolVector() const;

    /**
     * Converts each element to integer, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<intval_t> asIntVector() const;

    /**
     * Converts each element to integer, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<intval_t> asIntVectorInUnit(const char *targetUnit) const;

    /**
     * Converts each element to double, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<double> asDoubleVector() const;

    /**
     * Converts each element to double, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<double> asDoubleVectorInUnit(const char *targetUnit) const;

    /**
     * Converts each element to string, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    std::vector<std::string> asStringVector() const;

    /**
     * Converts each element to object of the given type, and returns the result.
     * If an element cannot be converted (see cValue API), an exception is thrown.
     */
    template<typename T> std::vector<T*> asObjectVector() const;
    //@}
};


template<typename T>
std::vector<T*> cValueArray::asObjectVector() const
{
    std::vector<T*> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++) {
        cObject *obj = array[i].objectValue();
        result[i] = dynamic_cast<T*>(obj);
        if (result[i] == nullptr && obj != nullptr)
            cannotCast(obj, opp_typename(typeid(T)));
    }
    return result;
}

}  // namespace omnetpp

#endif


