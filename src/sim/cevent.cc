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

#include <stdio.h>  // sprintf
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/cevent.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cenvir.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;


cEvent::cEvent(const cEvent& event) : cOwnedObject(event)
{
    heapindex = -1;
    prev_event_num = -1;
    operator=(event);
}

cEvent::cEvent(const char *name) : cOwnedObject(name, false)
{
    // name pooling is off for messages by default, as unique names are quite common
    prior = 0;
    delivd = 0;
    heapindex = -1;
    prev_event_num = -1;
}

cEvent::~cEvent()
{
}

std::string cEvent::info() const
{
    if (delivd == simulation.getSimTime())
        return "(now)";
    if (delivd < simulation.getSimTime())
        return "(in the past)";

    std::stringstream out;
    out << "at T=" << delivd << ", in dt=" << (delivd - simulation.getSimTime());
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
    throw cRuntimeError(this,E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    buffer->pack(prior);
    buffer->pack(delivd);
    buffer->pack(heapindex);
    buffer->pack(insertordr);
#endif
}

void cEvent::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);

    buffer->unpack(prior);
    buffer->unpack(delivd);
    buffer->unpack(heapindex);
    buffer->unpack(insertordr);
#endif
}

cEvent& cEvent::operator=(const cEvent& event)
{
    if (this==&event) return *this;

    cOwnedObject::operator=(event);

    prior = event.prior;
    delivd = event.delivd;

    return *this;
}


