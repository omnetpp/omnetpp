//========================================================================
//  CEVENT.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cevent.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp;

using std::ostream;

cEvent::cEvent(const cEvent& event) : cOwnedObject(event)
{
    heapIndex = -1;
    insertOrder = -1;
    previousEventNumber = -1;
    operator=(event);
}

cEvent::cEvent(const char *name) : cOwnedObject(name, false)
{
    // name pooling is off for messages by default, as unique names are quite common
    priority = 0;
    arrivalTime = 0;
    heapIndex = -1;
    insertOrder = -1;
    previousEventNumber = -1;
}

cEvent::~cEvent()
{
}

std::string cEvent::info() const
{
    if (arrivalTime == getSimulation()->getSimTime())
        return "(now)";
    if (arrivalTime < getSimulation()->getSimTime())
        return "(in the past)";

    std::stringstream out;
    out << "at T=" << arrivalTime << ", in dt=" << (arrivalTime - getSimulation()->getSimTime());
    if (getTargetObject())
        out << ", for " << getTargetObject()->getFullPath();
    return out.str();
}

void cEvent::forEachChild(cVisitor *v)
{
}

std::string cEvent::detailedInfo() const
{
    return "";  // all fields are available via reflection, no point in repeating them here
}

void cEvent::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    buffer->pack(priority);
    buffer->pack(arrivalTime);
    buffer->pack(heapIndex);
    buffer->pack(insertOrder);
#endif
}

void cEvent::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    buffer->unpack(priority);
    buffer->unpack(arrivalTime);
    buffer->unpack(heapIndex);
    buffer->unpack(insertOrder);
#endif
}

cEvent& cEvent::operator=(const cEvent& event)
{
    if (this == &event)
        return *this;

    cOwnedObject::operator=(event);

    priority = event.priority;
    arrivalTime = event.arrivalTime;

    return *this;
}

