//=========================================================================
//  COWNEDOBJECT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "omnetpp/cownedobject.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/simutil.h"
#include "omnetpp/csoftowner.h"
#include "omnetpp/cclassdescriptor.h"
#include "common/commonutil.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

#ifdef DEVELOPER_DEBUG
#include <set>
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cOwnedObject);

#ifdef DEVELOPER_DEBUG

std::set<cOwnedObject *> objectlist;
void printAllObjects()
{
    for (std::set<cOwnedObject *>::iterator it = objectlist.begin(); it != objectlist.end(); ++it) {
        printf(" %p (%s)%s\n", (*it), (*it)->getClassName(), (*it)->getName());
    }
}

#endif

// static class members
cSoftOwner *cOwnedObject::owningContext = &globalOwningContext;
long cOwnedObject::totalObjectCount = 0;
long cOwnedObject::liveObjectCount = 0;

cSoftOwner globalOwningContext("globalOwningContext", false, (internal::Void*)nullptr);

cOwnedObject::cOwnedObject()
{
    owningContext->doInsert(this); // sets owner pointer too

    // statistics
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const char *name, bool namepooling) : cNamedObject(name, namepooling)
{
    owningContext->doInsert(this);

    // statistics
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

// for constructing the global cSoftOwner instance globalOwningContext which has no owner
cOwnedObject::cOwnedObject(const char *name, bool namepooling, internal::Void *dummy) : cNamedObject(name, namepooling)
{
    owner = nullptr;
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const cOwnedObject& obj) : cNamedObject(obj)
{
    owningContext->doInsert(this);
    copy(obj);

    // statistics
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::~cOwnedObject()
{
#ifdef DEVELOPER_DEBUG
    objectlist.erase(this);
#endif

    if (owner)
        owner->ownedObjectDeleted(this);

    // statistics
    liveObjectCount--;
}

void cOwnedObject::removeFromOwnershipTree()
{
    // set ownership of this object to null
    if (owner)
        owner->yieldOwnership(this, nullptr);
}

void cOwnedObject::setOwningContext(cSoftOwner *list)
{
    ASSERT(list != nullptr);
    owningContext = list;
}

cSoftOwner *cOwnedObject::getOwningContext()
{
    return owningContext;
}

void cOwnedObject::copy(const cOwnedObject& obj)
{
    // Not too much to do:
    // - ownership not affected
    // - name string is NOT copied from other object
    // - flags are taken over, except for FL_NAMEPOOLING which is preserved
    unsigned short namePooling = flags & FL_NAMEPOOLING;
    flags = (obj.flags & ~FL_NAMEPOOLING) | namePooling;
}

cOwnedObject& cOwnedObject::operator=(const cOwnedObject& obj)
{
    cNamedObject::operator=(obj);
    copy(obj);
    return *this;
}

void cOwnedObject::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cNamedObject::parsimPack(buffer);
#endif
}

void cOwnedObject::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cNamedObject::parsimUnpack(buffer);
#endif
}

//-----

cNoncopyableOwnedObject *cNoncopyableOwnedObject::dup() const
{
    throw cRuntimeError(this, E_CANTDUP);
}

//----

static bool staticFlag;  // set to true while in main()
static bool exitingFlag; // set on getting a TERM or INT signal (Windows)

cStaticFlag::cStaticFlag()
{
    common::__insidemain = true;
    staticFlag = true;
}

cStaticFlag::~cStaticFlag()
{
    common::__insidemain = false;
    staticFlag = false;
}

bool cStaticFlag::insideMain()
{
    return staticFlag;
}

bool cStaticFlag::isExiting()
{
    return exitingFlag;
}

void cStaticFlag::setExiting()
{
    common::__exiting = true;
    exitingFlag = true;
}

}  // namespace omnetpp

