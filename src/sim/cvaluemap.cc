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

#include "omnetpp/cvaluemap.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

cValueMap::~cValueMap()
{
    clear();
}

void cValueMap::copy(const cValueMap& other)
{
    fields = other.fields;

    // dup() those objects that were owned by the other container
    for (auto entry : fields) {
        cValue& value = entry.second;
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

void cValueMap::takeValue(const cValue& value)
{
    if (value.getType() == cValue::OBJECT) {
        cObject *obj = value.objectValue();
        if (obj && obj->isOwnedObject() && obj->getOwner()->isSoftOwner())
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueMap::dropAndDeleteValue(const cValue& value)
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
        return std::string("empty");
    std::stringstream out;
    out << "size=" << fields.size();
    return out.str();
}

void cValueMap::forEachChild(cVisitor* v)
{
    for (auto entry : fields)
        if (entry.second.getType() == cValue::OBJECT)
            v->visit(entry.second.objectValue());
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


}  // namespace omnetpp

