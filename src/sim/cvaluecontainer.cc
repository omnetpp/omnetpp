//==========================================================================
//   CVALUECONTAINER.CC  - part of
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

#include "omnetpp/cvaluecontainer.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

void cValueContainer::valueCloned(cValue& value, const cValueContainer& other)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            value.set(obj->dup());
        else if (obj->getOwner() == &other) {
            cObject *obj2 = obj->dup();
            value.set(obj2);
            take(static_cast<cOwnedObject *>(obj2));
        }
    }
}

void cValueContainer::takeValue(cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == cOwnedObject::getOwningContext() && dynamic_cast<cComponent*>(obj) == nullptr)
            take(static_cast<cOwnedObject*>(obj));
    }
}

void cValueContainer::dropValue(cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (obj->isOwnedObject() && obj->getOwner() == this)
            drop(static_cast<cOwnedObject*>(obj));
    }
}

void cValueContainer::dropAndDeleteValue(cValue& value)
{
    if (value.containsObject()) {
        cObject *obj = value.objectValue();
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject*>(obj));
    }
}

}  // namespace omnetpp

