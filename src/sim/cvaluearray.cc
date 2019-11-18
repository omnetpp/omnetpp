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

#include "omnetpp/cvaluearray.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

cValueArray::~cValueArray()
{
    clear();
}

void cValueArray::copy(const cValueArray& other)
{
    array = other.array;

    // dup() those objects that were owned by the other container
    for (cValue& value : array) {
        if (value.getType() == cValue::OBJECT) {
            cObject *obj = value.objectValue();
            if (obj && (!obj->isOwnedObject() || obj->getOwner() == &other)) {
                cObject *clone = obj->dup();
                value.set(clone);
                if (obj->isOwnedObject())
                    take(static_cast<cOwnedObject*>(clone));
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
    if (value.getType() == cValue::OBJECT) {
        cObject *obj = value.objectValue();
        if (obj && obj->isOwnedObject() && obj->getOwner()->isSoftOwner())
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueArray::dropAndDeleteValue(const cValue& value)
{
    if (value.getType() == cValue::OBJECT) {
        cObject *obj = value.objectValue();
        if (!obj)
            ; // nop
        else if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject*>(obj));
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
    std::stringstream out;
    int length = array.size();
    int printed = std::min(8, length);
    out << "[";
    for (int i = 0; i < printed; i++) {
        if (i != 0) out << ", ";
        out << array[i].str();
    }
    if (length > printed)
        out << ", ... and " << (length-printed) << " more";
    out << "]";
    return out.str();
}

void cValueArray::forEachChild(cVisitor* v)
{
    for (const cValue& value : array)
        if (value.getType() == cValue::OBJECT)
            if (cObject *child = value.objectValue())
                v->visit(child);
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
        throw cRuntimeError(this, "insert(): index %s is out of bounds", k);

    array.insert(array.begin() + k, value);
    takeValue(value);
}

const cValue& cValueArray::get(int k) const
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "get(): index %s is out of bounds", k);
    return array.at(k);
}

void cValueArray::set(int k, const cValue& value)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "set(): index %s is out of bounds", k);
    dropAndDeleteValue(array[k]);
    array[k] = value;
    takeValue(value);
}

void cValueArray::erase(int k)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "erase(): index %s is out of bounds", k);
    dropAndDeleteValue(array[k]);
    array.erase(array.begin() + k);
}

cValue cValueArray::remove(int k)
{
    if (k < 0 || k >= (int)array.size())
        throw cRuntimeError(this, "remove(): index %s is out of bounds", k);
    if (array[k].getType() == cValue::OBJECT) {
        cObject *obj = array[k].objectValue();
        if (obj && obj->getOwner() == this)
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

std::vector<double> cValueArray::asDoubleVector() const
{
    std::vector<double> result;
    result.resize(array.size());
    for (int i = 0; i < array.size(); i++)
        result[i] = array[i].intValue();
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

