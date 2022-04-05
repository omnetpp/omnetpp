//=========================================================================
//  CSOFTOWNER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cSoftOwner : stores a set of cOwnedObjects
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm> // copy_n()
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/csoftowner.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cwatch.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {


Register_Class(cSoftOwner);

Register_PerRunConfigOption(CFGID_ALLOW_OBJECT_STEALING_ON_DELETION, "allow-object-stealing-on-deletion", CFG_BOOL, "false", "Setting it to true disables the \"Context component is deleting an object it doesn't own\" error message. This option exists primarily for backward compatibility with pre-6.0 versions that were more permissive during object deletion.");

static bool allowObjectStealing = false;

namespace {
class LocalLifecycleListener : public cISimulationLifecycleListener {
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) {
        if (eventType == LF_PRE_NETWORK_SETUP)
            allowObjectStealing = cSimulation::getActiveEnvir()->getConfig()->getAsBool(CFGID_ALLOW_OBJECT_STEALING_ON_DELETION);
        else if (eventType == LF_ON_SHUTDOWN)
            delete this; // implies unsubscribing
    }
};
}

EXECUTE_ON_STARTUP(cSimulation::getActiveEnvir()->addLifecycleListener(new LocalLifecycleListener()));


cSoftOwner::cSoftOwner(const char *name, bool namepooling) : cNoncopyableOwnedObject(name, namepooling)
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
    reportUndisposed();
}

void cSoftOwner::reportUndisposed()
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
    objs = nullptr;
    numObjs = 0;
}

void cSoftOwner::doInsert(cOwnedObject *obj)
{
    ASSERT2(obj != this || this == &globalOwningContext, "Cannot insert object in itself");

    if (numObjs >= capacity) {
        // must allocate bigger vector (grow 25% but at least 2)
        capacity += (capacity < 8) ? 2 : (capacity >> 2);
        cOwnedObject **v = new cOwnedObject *[capacity];
        std::copy_n(objs, numObjs, v);
        delete[] objs;
        objs = v;
    }

    obj->owner = this;
    objs[obj->pos = numObjs++] = obj;
#ifdef SIMFRONTEND_SUPPORT
    lastChangeSerial = changeCounter++;
#endif
}

void cSoftOwner::objectStealingOnDeletion(cOwnedObject *obj)
{
    if (!allowObjectStealing) {
        // note 1: we cannot throw an exception, as C++ forbids throwing in a destructor, and noexcept(false) is not workable
        // note 2: cannot print obj->getClassName(), as type is already lost at this point (will always be cOwnedObject)
        cRuntimeError ex("Warning: Context component %s is deleting an object named \"%s\" it doesn't own, owner is %s; set %s=true to disable this error message",
                owningContext->getClassAndFullName().c_str(),
                obj->getFullName(),
                getClassAndFullName().c_str(),
                CFGID_ALLOW_OBJECT_STEALING_ON_DELETION->getName());
        getEnvir()->alert(ex.getFormattedMessage().c_str());
    }
}

void cSoftOwner::ownedObjectDeleted(cOwnedObject *obj)
{
    ASSERT(obj && obj->owner == this);
    if (this != owningContext)
        objectStealingOnDeletion(obj);

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
        if (!v->visit(objs[i]))
            return;
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

