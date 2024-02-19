//==========================================================================
//   CVALUEARRAY.CC  - part of
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

#include <algorithm>
#include "omnetpp/cvaluearray.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

Register_Class(cValueArray);

cValueArray::~cValueArray()
{
    clear();
}

void cValueArray::copy(const cValueArray& other)
{
    array = other.array;

    for (cValue& value : array)
        valueCloned(value, other);
}

void cValueArray::cannotCast(cObject *obj, const char *toClass) const
{
    throw cRuntimeError("Cannot cast '%s*' to '%s*')", obj->getClassName(), toClass);
}

cValueArray& cValueArray::operator=(const cValueArray& other)
{
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}

std::string cValueArray::str() const
{
    if (array.empty())
        return std::string("[]");

    // note: do not truncate the output, because it ruins editing JSON parameters in Qtenv
    std::stringstream out;
    int length = array.size();
    out << "[";
    for (int i = 0; i < length; i++) {
        if (i != 0) out << ", ";
        out << array[i].str();
    }
    out << "]";
    return out.str();
}

void cValueArray::forEachChild(cVisitor* v)
{
    for (const cValue& value : array)
        if (value.containsObject())
            if (cObject *child = value.objectValue())
                if (!v->visit(child))
                    return;
}

void cValueArray::clear()
{
    for (cValue& value : array)
        dropAndDeleteValue(value);
    array.clear();
}

void cValueArray::add(const cValue& v)
{
    array.push_back(v);
    takeValue(array.back());
}

void cValueArray::insert(int k, const cValue& value)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "insert(): index %d is out of bounds", k);

    array.insert(array.begin() + k, value);
    takeValue(array[k]);
}

const cValue& cValueArray::get(int k) const
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "get(): index %d is out of bounds", k);
    return array.at(k);
}

void cValueArray::set(int k, const cValue& value)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "set(): index %d is out of bounds", k);
    dropAndDeleteValue(array[k]);
    array[k] = value;
    takeValue(array[k]);
}

void cValueArray::erase(int k)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "erase(): index %d is out of bounds", k);
    dropAndDeleteValue(array[k]);
    array.erase(array.begin() + k);
}

cValue cValueArray::remove(int k)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "remove(): index %d is out of bounds", k);
    if (array[k].containsObject()) {
        cObject *obj = array[k].objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == this)
            drop(static_cast<cOwnedObject*>(obj));
    }
    cValue result = std::move(array[k]);
    array[k] = cValue();
    return result;
}

std::vector<bool> cValueArray::asBoolVector() const
{
    std::vector<bool> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].boolValue();
    return result;
}

std::vector<intval_t> cValueArray::asIntVector() const
{
    std::vector<intval_t> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].intValue();
    return result;
}

std::vector<intval_t> cValueArray::asIntVectorInUnit(const char *targetUnit) const
{
    std::vector<intval_t> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].intValueInUnit(targetUnit);
    return result;
}

std::vector<double> cValueArray::asDoubleVector() const
{
    std::vector<double> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].doubleValue();
    return result;
}

std::vector<double> cValueArray::asDoubleVectorInUnit(const char *targetUnit) const
{
    std::vector<double> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].doubleValueInUnit(targetUnit);
    return result;
}

std::vector<std::string> cValueArray::asStringVector() const
{
    std::vector<std::string> result;
    result.resize(array.size());
    for (int i = 0; i < (int)array.size(); i++)
        result[i] = array[i].stdstringValue();
    return result;
}

}  // namespace omnetpp

