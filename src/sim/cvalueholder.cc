//==========================================================================
//   CVALUEHOLDER.CC  - part of
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

#include "omnetpp/cvalueholder.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

Register_Class(cValueHolder);

//TODO takeValue(), dropAndDeleteValue() and part of copy() are the same in cValueHolder, cValueArray, cValueMap, maybe also in cQueue/cArray. Factor them out?

void cValueHolder::copy(const cValueHolder& other)
{
    value = other.value;

    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            value.set(obj->dup());
        else if (obj->getOwner() == const_cast<cValueHolder*>(&other)) {
            cObject *obj2 = obj->dup();
            value.set(obj2);
            take(static_cast<cOwnedObject *>(obj2));
        }
    }
}

void cValueHolder::takeValue()
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == cOwnedObject::getOwningContext() && dynamic_cast<cComponent*>(obj) == nullptr)
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueHolder::dropAndDeleteValue()
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject*>(obj));
    }
}

cValueHolder& cValueHolder::operator=(const cValueHolder& other)
{
    if (this != &other) {
        dropAndDeleteValue();
        copy(other);
    }
    return *this;
}

void cValueHolder::forEachChild(cVisitor* v)
{
    if (value.containsObject())
        v->visit(value.objectValue());
}

void cValueHolder::parsimPack(cCommBuffer* buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimPack() not implemented");
#endif
}

void cValueHolder::parsimUnpack(cCommBuffer* buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    throw cRuntimeError(this, "parsimUnpack() not implemented");
#endif
}

}  // namespace omnetpp

