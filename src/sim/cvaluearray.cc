//==========================================================================
//   CVALUEARRAY.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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

    for (cValue& value : array) {
        if (value.containsObject()) {
            cObject *obj = value.objectValue();
            if (!obj->isOwnedObject())
                value.set(obj->dup());
            else if (obj->getOwner() == const_cast<cValueArray *>(&other)) {
                cObject *obj2 = obj->dup();
                value.set(obj2);
                take(static_cast<cOwnedObject *>(obj2));
            }
        }
    }
}

void cValueArray::cannotCast(cObject *obj, const char *toClass) const
{
    throw cRuntimeError("Cannot cast '%s*' to '%s*')", obj->getClassName(), toClass);
}

void cValueArray::takeValue(const cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == cOwnedObject::getOwningContext() && dynamic_cast<cComponent*>(obj) == nullptr)
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueArray::dropAndDeleteValue(const cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject *>(obj));
    }
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

void cValueArray::parsimPack(cCommBuffer* buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimPack() not implemented");
#endif
}

void cValueArray::parsimUnpack(cCommBuffer* buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimUnpack() not implemented");
#endif
}

void cValueArray::clear()
{
    for (const cValue& value : array)
        dropAndDeleteValue(value);
    array.clear();
}

void cValueArray::add(const cValue& value)
{
    array.push_back(value);
    takeValue(value);
}

void cValueArray::insert(int k, const cValue& value)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "insert(): index %d is out of bounds", k);

    array.insert(array.begin() + k, value);
    takeValue(value);
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
    takeValue(value);
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
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].boolValue();
    return result;
}

std::vector<intval_t> cValueArray::asIntVector() const
{
    std::vector<intval_t> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].intValue();
    return result;
}

std::vector<intval_t> cValueArray::asIntVectorInUnit(const char *targetUnit) const
{
    std::vector<intval_t> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].intValueInUnit(targetUnit);
    return result;
}

std::vector<double> cValueArray::asDoubleVector() const
{
    std::vector<double> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].doubleValue();
    return result;
}

std::vector<double> cValueArray::asDoubleVectorInUnit(const char *targetUnit) const
{
    std::vector<double> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].doubleValueInUnit(targetUnit);
    return result;
}

std::vector<std::string> cValueArray::asStringVector() const
{
    std::vector<std::string> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].stringValue();
    return result;
}

}  // namespace omnetpp

