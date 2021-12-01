//==========================================================================
//   CVALUEHOLDER.CC  - part of
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

#include "omnetpp/cvalueholder.h"
#include "omnetpp/checkandcast.h"

namespace omnetpp {

Register_Class(cValueHolder);

void cValueHolder::copy(const cValueHolder& other)
{
    value = other.value;
    valueCloned(value, other);
}

cValueHolder& cValueHolder::operator=(const cValueHolder& other)
{
    if (this != &other) {
        dropAndDeleteValue(value);
        copy(other);
    }
    return *this;
}

void cValueHolder::forEachChild(cVisitor* v)
{
    if (value.containsObject())
        v->visit(value.objectValue());
}

}  // namespace omnetpp

