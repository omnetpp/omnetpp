//=========================================================================
//  CNAMEDOBJECT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf
#include <cstring>  // strcpy, strlen etc.
#include "omnetpp/cnamedobject.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/globals.h"
#include "omnetpp/opp_string.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cNamedObject);

// static class members
cStringPool cNamedObject::stringPool("cNamedObject::stringPool");

cNamedObject::cNamedObject()
{
    name = nullptr;
    flags = FL_NAMEPOOLING;
}

cNamedObject::cNamedObject(const char *s, bool namepooling)
{
    flags = namepooling ? FL_NAMEPOOLING : 0;
    if (!s)
        name = nullptr;
    else if (namepooling)
        name = stringPool.get(s);
    else
        name = opp_strdup(s);
}

cNamedObject::cNamedObject(const cNamedObject& obj) : cObject(obj)
{
    name = nullptr;
    flags = obj.flags & FL_NAMEPOOLING;
    setName(obj.getName());
    copy(obj);
}

cNamedObject::~cNamedObject()
{
    if (name) {
        if (flags & FL_NAMEPOOLING)
            stringPool.release(name);
        else
            delete[] name;
    }
}

void cNamedObject::copy(const cNamedObject& obj)
{
    // Not much to do:
    // - ownership not affected
    // - name string is NOT copied from other object
    // - flags are taken over, except for FL_NAMEPOOLING which is preserved
    unsigned short namePooling = flags & FL_NAMEPOOLING;
    flags = (obj.flags & ~FL_NAMEPOOLING) | namePooling;
}

cNamedObject& cNamedObject::operator=(const cNamedObject& obj)
{
    copy(obj);
    return *this;
}

void cNamedObject::setName(const char *s)
{
    // release name string
    if (name) {
        if (flags & FL_NAMEPOOLING)
            stringPool.release(name);
        else
            delete[] name;
    }

    // set new string
    if (!s)
        name = nullptr;
    else if (flags & FL_NAMEPOOLING)
        name = stringPool.get(s);
    else
        name = opp_strdup(s);
}

void cNamedObject::setNamePooling(bool pooling)
{
    if ((flags & FL_NAMEPOOLING) == pooling)
        return;
    if (pooling) {
        // turn on
        flags |= FL_NAMEPOOLING;
        if (name) {
            const char *oldname = name;
            name = stringPool.get(oldname);
            delete[] oldname;
        }
    }
    else {
        // turn off
        flags &= ~FL_NAMEPOOLING;
        if (name) {
            const char *oldname = name;
            name = opp_strdup(oldname);
            stringPool.release(oldname);
        }
    }
}

void cNamedObject::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    buffer->pack(getName());
    buffer->pack(flags);
#endif
}

void cNamedObject::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    opp_string tmp;
    buffer->unpack(tmp);
    setName(tmp.buffer());
    buffer->unpack(flags);
#endif
}

}  // namespace omnetpp

