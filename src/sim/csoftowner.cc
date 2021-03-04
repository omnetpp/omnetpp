//=========================================================================
//  CSOFTOWNER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cSoftOwner : stores a set of cOwnedObjects
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
#include "omnetpp/csoftowner.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cwatch.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {


Register_Class(cSoftOwner);


cSoftOwner::cSoftOwner(const char *name) : cNoncopyableOwnedObject(name)
{
    // careful: if we are a global variable (ctor called before main()),
    // then insert() may get called before constructor and it invoked
    // construct() already.
    if (cStaticFlag::insideMain() || capacity == 0)
        construct();

    // if we're invoked before main, then we are a global variable (dynamic
    // instances of cSoftOwner are not supposed to be created
    // before main()) --> remove ourselves from ownership tree because
    // we shouldn't be destroyed via operator delete
    if (!cStaticFlag::insideMain())
        removeFromOwnershipTree();
}

void cSoftOwner::construct()
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

cSoftOwner::~cSoftOwner()
{
    // report objects as undisposed, except watches
    for (int i = 0; i < numObjs; i++) {
        if (dynamic_cast<cWatchBase *>(objs[i]))
            delete objs[i--];  // "i--" used because delete will move last item to position i
        else {
            getEnvir()->undisposedObject(objs[i]);
            objs[i]->owner = nullptr; // as its current owner (this) is being deleted
        }
    }
    delete[] objs;
}

void cSoftOwner::doInsert(cOwnedObject *obj)
{
    ASSERT2(obj != this || this == &globalOwningContext, "Cannot insert object in itself");

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

void cSoftOwner::ownedObjectDeleted(cOwnedObject *obj)
{
    ASSERT(obj && obj->owner == this);
#ifdef CHECK_OWNERSHIP_ON_DELETE
    if (this != owningContext) {
        // note: we cannot throw an exception, as C++ forbids throwing in a destructor, and noexcept(false) is not workable
        cRuntimeError e("Warning: Context component is deleting an object named \"%s\" it doesn't own; owner is (%s)%s",
                obj->getFullName(), getClassName(), getFullPath().c_str()); // note: cannot print obj->getClassName(), as type is already lost at this point (will always be cOwnedObject)
        getEnvir()->alert(e.getFormattedMessage().c_str());
        // abort()? -- perhaps that's too harsh
    }
#endif

    // move last object to obj's old position
    int pos = obj->pos;
    (objs[pos] = objs[--numObjs])->pos = pos;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = changeCounter++;
#endif
}

void cSoftOwner::yieldOwnership(cOwnedObject *obj, cObject *newowner)
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

std::string cSoftOwner::str() const
{
    std::stringstream out;
    out << "n=" << numObjs;
    return out.str();
}

void cSoftOwner::forEachChild(cVisitor *v)
{
    for (int i = 0; i < numObjs; i++)
        v->visit(objs[i]);
}

void cSoftOwner::parsimPack(cCommBuffer *buffer) const
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimPack(buffer);

    if (numObjs > 0)
        throw cRuntimeError(this, "parsimPack() not supported (makes no sense)");
#endif
}

void cSoftOwner::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, E_NOPARSIM);
#else
    cOwnedObject::parsimUnpack(buffer);
    if (numObjs > 0)
        throw cRuntimeError(this, "parsimUnpack(): Can only unpack into empty object");
#endif
}

void cSoftOwner::take(cOwnedObject *obj)
{
    // ask current owner to release it -- if it's a cSoftOwner, it will.
    obj->owner->yieldOwnership(obj, this);
    doInsert(obj);
}

void cSoftOwner::drop(cOwnedObject *obj)
{
    if (obj->owner != this)
        throw cRuntimeError(this, "drop(): Not owner of object (%s)%s",
                obj->getClassName(), obj->getFullPath().c_str());
    // the following 2 lines are actually the same as owningContext->take(obj);
    yieldOwnership(obj, owningContext);
    owningContext->doInsert(obj);
}

cOwnedObject *cSoftOwner::getOwnedObject(int k)
{
    if (k < 0 || k >= numObjs)
        return nullptr;
    return objs[k];
}

#ifdef SIMFRONTEND_SUPPORT
bool cSoftOwner::hasChangedSince(int64_t lastRefreshSerial)
{
    return lastChangeSerial > lastRefreshSerial;
}

#endif

}  // namespace omnetpp

