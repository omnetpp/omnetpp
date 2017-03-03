//=========================================================================
//  CDEFAULTLIST.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cDefaultList : stores a set of cOwnedObjects
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

#include <cstring>  // memcpy
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cdefaultlist.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cwatch.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {


Register_Class(cDefaultList);


cDefaultList::cDefaultList(const char *name) : cNoncopyableOwnedObject(name)
{
    // careful: if we are a global variable (ctor called before main()),
    // then insert() may get called before constructor and it invoked
    // construct() already.
    if (cStaticFlag::insideMain() || capacity == 0)
        construct();

    // if we're invoked before main, then we are a global variable (dynamic
    // instances of cDefaultList are not supposed to be created
    // before main()) --> remove ourselves from ownership tree because
    // we shouldn't be destroyed via operator delete
    if (!cStaticFlag::insideMain())
        removeFromOwnershipTree();
}

void cDefaultList::construct()
{
    capacity = 2;
    numObjs = 0;
    objs = new cOwnedObject *[capacity];
    for (int i = 0; i < capacity; i++)
        objs[i] = nullptr;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = 0;
#endif
}

cDefaultList::~cDefaultList()
{
    if (getPerformFinalGC()) {
        // delete all owned objects. One place we make use of this is behavior is
        // when a simple module gets deleted -- there we have to delete all dynamically
        // allocated objects held by the module. But: deletion has dangers,
        // i.e. if we try to delete objects embedded in other objects/structs or
        // arrays, it will crash mysteriously to the user -- so consider not deleting.
        while (numObjs > 0)
            delete objs[0];
        delete[] objs;
    }
    else {
        // experimental: do not delete objects (except cWatches), just print their names
        for (int i = 0; i < numObjs; i++) {
            if (dynamic_cast<cWatchBase *>(objs[i]))
                delete objs[i--];  // "i--" used because delete will move last item to position i
            else
                getEnvir()->undisposedObject(objs[i]);
        }

        // we can free up the pointer array itself though
        delete[] objs;
    }
}

void cDefaultList::doInsert(cOwnedObject *obj)
{
    ASSERT(obj != this || this == &defaultList);

    if (numObjs >= capacity) {
        if (capacity == 0) {
            // this is if we're invoked before main, before our ctor run
            construct();
        }
        else {
            // must allocate bigger vector (grow 25% but at least 2)
            capacity += (capacity < 8) ? 2 : (capacity >> 2);
            cOwnedObject **v = new cOwnedObject *[capacity];
            memcpy(v, objs, sizeof(cOwnedObject *) * numObjs);
            delete[] objs;
            objs = v;
        }
    }

    obj->owner = this;
    objs[obj->pos = numObjs++] = obj;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = changeCounter++;
#endif
}

void cDefaultList::ownedObjectDeleted(cOwnedObject *obj)
{
    ASSERT(obj && obj->owner == this);

    // move last object to obj's old position
    int pos = obj->pos;
    (objs[pos] = objs[--numObjs])->pos = pos;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = changeCounter++;
#endif
}

void cDefaultList::yieldOwnership(cOwnedObject *obj, cObject *newowner)
{
    ASSERT(obj && obj->owner == this && numObjs > 0);

    // give object to its new owner
    obj->owner = newowner;

    // move last object to obj's old position
    int pos = obj->pos;
    (objs[pos] = objs[--numObjs])->pos = pos;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = changeCounter++;
#endif
}

void cDefaultList::takeAllObjectsFrom(cDefaultList& other)
{
    while (other.defaultListSize() > 0)
        take(other.defaultListGet(0));
}

std::string cDefaultList::str() const
{
    std::stringstream out;
    out << "n=" << numObjs;
    return out.str();
}

void cDefaultList::forEachChild(cVisitor *v)
{
    for (int i = 0; i < numObjs; i++)
        v->visit(objs[i]);
}

void cDefaultList::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    if (numObjs > 0)
        throw cRuntimeError(this, "parsimPack() not supported (makes no sense)");
#endif
}

void cDefaultList::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);
    if (numObjs > 0)
        throw cRuntimeError(this, "parsimUnpack(): Can only unpack into empty object");
#endif
}

void cDefaultList::take(cOwnedObject *obj)
{
    // ask current owner to release it -- if it's a cDefaultList, it will.
    obj->owner->yieldOwnership(obj, this);
    doInsert(obj);
}

void cDefaultList::drop(cOwnedObject *obj)
{
    if (obj->owner != this)
        throw cRuntimeError(this, "drop(): Not owner of object (%s)%s",
                obj->getClassName(), obj->getFullPath().c_str());
    // the following 2 lines are actually the same as defaultOwner->take(obj);
    yieldOwnership(obj, defaultOwner);
    defaultOwner->doInsert(obj);
}

cOwnedObject *cDefaultList::defaultListGet(int k)
{
    if (k < 0 || k >= numObjs)
        return nullptr;
    return objs[k];
}

bool cDefaultList::defaultListContains(cOwnedObject *obj) const
{
    return obj && obj->getOwner() == const_cast<cDefaultList *>(this);
}

#ifdef SIMFRONTEND_SUPPORT
bool cDefaultList::hasChangedSince(int64_t lastRefreshSerial)
{
    return lastChangeSerial > lastRefreshSerial;
}

#endif

}  // namespace omnetpp

