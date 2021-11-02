//==========================================================================
//   CVALUEMAP.CC  - part of
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
#include "omnetpp/cvaluemap.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

Register_Class(cValueMap);

cValueMap::~cValueMap()
{
    clear();
}

void cValueMap::copy(const cValueMap& other)
{
    fields = other.fields;

    for (auto& entry : fields) {
        cValue& value = entry.second;
        if (value.containsObject()) {
            cObject *obj = value.objectValue();
            if (!obj->isOwnedObject())
                value.set(obj->dup());
            else if (obj->getOwner() == const_cast<cValueMap*>(&other)) {
                cObject *obj2 = obj->dup();
                value.set(obj2);
                take(static_cast<cOwnedObject *>(obj2));
            }
        }
    }
}

void cValueMap::takeValue(const cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == cOwnedObject::getOwningContext() && dynamic_cast<cComponent*>(obj) == nullptr)
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueMap::dropAndDeleteValue(const cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject*>(obj));
    }
}

cValueMap& cValueMap::operator=(const cValueMap& other)
{
    if (this != &other) {
        clear();
        copy(other);
    }
    return *this;
}

std::string cValueMap::str() const
{
    if (fields.empty())
        return std::string("{}");

    // note: do not truncate the output, because it ruins editing JSON parameters in Qtenv
    std::stringstream out;
    out << "{";
    int i = 0;
    for (auto& p : fields) {
        if (i++ != 0) out << ", ";
        out << p.first << ": " << p.second.str();
    }
    out << "}";
    return out.str();
}

void cValueMap::forEachChild(cVisitor* v)
{
    for (auto entry : fields)
        if (entry.second.containsObject())
            if (cObject *child = entry.second.objectValue())
                if (!v->visit(child))
                    return;
}

void cValueMap::parsimPack(cCommBuffer* buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimPack() not implemented");
#endif
}

void cValueMap::parsimUnpack(cCommBuffer* buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimUnpack() not implemented");
#endif
}

void cValueMap::clear()
{
    for (auto entry : fields)
        dropAndDeleteValue(entry.second);
    fields.clear();
}

void cValueMap::set(const char *key, const cValue& value)
{
    auto it = fields.find(key);
    if (it == fields.end()) {
        fields[key] = value;
    }
    else {
        dropAndDeleteValue(it->second);
        it->second = value;
    }
    takeValue(value);
}

bool cValueMap::containsKey(const char *key) const
{
    return fields.find(key) != fields.end();
}

const cValue& cValueMap::get(const char *key) const
{
    auto it = fields.find(key);
    if (it == fields.end())
        throw cRuntimeError(this, "get(): No such key: \"%s\"", key);
    return it->second;
}

void cValueMap::erase(const char *key)
{
    auto it = fields.find(key);
    if (it != fields.end()) {
        dropAndDeleteValue(it->second);
        fields.erase(it);
    }
}

const cValueMap::Entry& cValueMap::getEntry(int k) const
{
    for (auto& entry : fields) //TODO cache
        if (k-- == 0)
            return entry;
    throw cRuntimeError(this, "getEntry(): index out of bounds");
}


}  // namespace omnetpp

