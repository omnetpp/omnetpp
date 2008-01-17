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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy, strlen etc.
#include "cnamedobject.h"
#include "cownedobject.h"
#include "globals.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;

Register_Class(cNamedObject);


// static class members
cStringPool cNamedObject::stringPool;

cNamedObject::cNamedObject()
{
    namep = NULL;
    flags = FL_NAMEPOOLING;
}

cNamedObject::cNamedObject(const char *name, bool namepooling)
{
    flags = namepooling ? FL_NAMEPOOLING : 0;
    if (!name)
        namep = NULL;
    else if (namepooling)
        namep = stringPool.get(name);
    else
        namep  = opp_strdup(name);
}

cNamedObject::cNamedObject(const cNamedObject& obj)
{
    flags = obj.flags; // copy all flags, incl namepooling
    namep = NULL;
    setName(obj.name());
    operator=(obj);
}

cNamedObject::~cNamedObject()
{
    if (namep)
    {
        if (flags & FL_NAMEPOOLING)
            stringPool.release(namep);
        else
            delete [] namep;
    }
}

cNamedObject& cNamedObject::operator=(const cNamedObject& obj)
{
    // Not too much to do:
    // - ownership not affected
    // - name string is NOT copied from other object
    // - flags are taken over, except for FL_NAMEPOOLING which is preserved
    unsigned short namePooling = flags & FL_NAMEPOOLING;
    flags = (obj.flags & ~FL_NAMEPOOLING) | namePooling;
    return *this;
}

void cNamedObject::setName(const char *s)
{
    // release name string
    if (namep)
    {
        if (flags & FL_NAMEPOOLING)
            stringPool.release(namep);
        else
            delete [] namep;
    }

    // set new string
    if (!s)
        namep = NULL;
    else if (flags & FL_NAMEPOOLING)
        namep = stringPool.get(s);
    else
        namep = opp_strdup(s);
}

void cNamedObject::setNamePooling(bool pooling)
{
    if ((flags & FL_NAMEPOOLING) == pooling)
        return;
    if (pooling)
    {
        // turn on
        flags |= FL_NAMEPOOLING;
        if (namep)
        {
            const char *oldname = namep;
            namep = stringPool.get(oldname);
            delete [] oldname;
        }
    }
    else
    {
        // turn off
        flags &= ~FL_NAMEPOOLING;
        if (namep)
        {
            const char *oldname = namep;
            namep = opp_strdup(oldname);
            stringPool.release(oldname);
        }
    }
}

void cNamedObject::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    buffer->pack(name());
#endif
}

void cNamedObject::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    opp_string tmp;
    buffer->unpack(tmp);
    setName(tmp.buffer());
#endif
}


