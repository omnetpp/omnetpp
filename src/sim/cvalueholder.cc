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


void cValueHolder::copy(const cValueHolder& other)
{
    value = other.value;

    // duplicate ALL contained objects, not only those owned by the cloned container
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        cObject *clone = obj->dup();
        value.set(clone);
        if (obj->isOwnedObject())
            take(static_cast<cOwnedObject*>(clone));
    }
}

void cValueHolder::takeValue()
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner()->isSoftOwner())
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
//TODO: putting this line back will break it: cPars may not contain non-cOwnedObject objects!
//    if (value.containsObject())
//        v->visit(value.objectValue());
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

