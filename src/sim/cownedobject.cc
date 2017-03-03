//=========================================================================
//  COWNEDOBJECT.CC - part of
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
#include "omnetpp/cownedobject.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cdefaultlist.h"
#include "omnetpp/cclassdescriptor.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

bool cStaticFlag::staticFlag;
bool cStaticFlag::exitingFlag;

Register_Class(cOwnedObject);

#ifdef DEVELOPER_DEBUG
#include <set>

std::set<cOwnedObject *> objectlist;
void printAllObjects()
{
    for (std::set<cOwnedObject *>::iterator it = objectlist.begin(); it != objectlist.end(); ++it) {
        printf(" %p (%s)%s\n", (*it), (*it)->getClassName(), (*it)->getName());
    }
}

#endif

// static class members
cDefaultList *cOwnedObject::defaultOwner = &defaultList;
long cOwnedObject::totalObjectCount = 0;
long cOwnedObject::liveObjectCount = 0;

cDefaultList defaultList;

cOwnedObject::cOwnedObject()
{
    //TODO: in DEBUG mode, assert that this is not a static member / global variable! (using cStaticFlag)
    defaultOwner->doInsert(this);

    // statistics
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const char *name, bool namepooling) : cNamedObject(name, namepooling)
{
    defaultOwner->doInsert(this);

    // statistics
    totalObjectCount++;
    liveObjectCount++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const cOwnedObject& obj) : cNamedObject(obj)
{
    defaultOwner->doInsert(this);
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

void cOwnedObject::setDefaultOwner(cDefaultList *list)
{
    ASSERT(list != nullptr);
    defaultOwner = list;
}

cDefaultList *cOwnedObject::getDefaultOwner()
{
    return defaultOwner;
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
    throw cRuntimeError(this, "dup(): %s subclasses from cNoncopyableOwnedObject, "
                              "and does not support dup()", getClassName());
}

void cNoncopyableOwnedObject::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(this, "parsimPack(): %s subclasses from cNoncopyableOwnedObject, and "
                              "does not support pack/unpack operations", getClassName());
}

void cNoncopyableOwnedObject::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "parsimUnpack(): %s subclasses from cNoncopyableOwnedObject, and "
                              "does not support pack/unpack operations", getClassName());
}

//-----

ostream& operator<<(ostream& os, const cOwnedObject *p)
{
    if (!p)
        return os << "(nullptr)";
    return os << "(" << p->getClassName() << ")" << p->getFullName();
}

ostream& operator<<(ostream& os, const cOwnedObject& o)
{
    return os << "(" << o.getClassName() << ")" << o.getFullName();
}

//-----

/* Debug code:
static struct X {
    ~X() {if (cStaticFlag::isSet()) printf("<!> Warning: cStaticFlag flag still set while shutting down! Make sure it always gets cleared at latest when exiting main().\n");}
} x;
*/

}  // namespace omnetpp

