//========================================================================
//  COBJECT.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cobject.h"
#include "omnetpp/cownedobject.h"
#include "omnetpp/cdefaultlist.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenvir.h"

namespace omnetpp {

#ifdef SIMFRONTEND_SUPPORT
int64_t cObject::changeCounter = 0;
#endif

cObject::~cObject()
{
    cSimulation::getActiveEnvir()->objectDeleted(this);
}

const char *cObject::getClassName() const
{
    return opp_typename(typeid(*this));
}

cClassDescriptor *cObject::getDescriptor() const
{
    return cClassDescriptor::getDescriptorFor(this);
}

#ifdef SIMFRONTEND_SUPPORT
bool cObject::hasChangedSince(int64_t lastRefreshSerial)
{
    return true;  // as we don't have more info
}

#endif

std::string cObject::getFullPath() const
{
    if (getOwner() == nullptr)
        return getFullName();
    else
        return getOwner()->getFullPath() + "." + getFullName();
}

std::string cObject::str() const
{
    return std::string();
}

cObject *cObject::dup() const
{
    throw cRuntimeError("The dup() method, declared in cObject, is not "
                        "redefined in class %s", getClassName());
}

void cObject::ownedObjectDeleted(cOwnedObject *obj)
{
    // Note: too late to call obj->getClassName(), at this point it'll aways return "cOwnedObject"
    throw cRuntimeError("Object %s is currently in (%s)%s, it cannot be deleted. "
                        "If this error occurs inside %s, it needs to be changed "
                        "to call drop() before it can delete that object. "
                        "If this error occurs inside %s's destructor and %s is a class member, "
                        "%s needs to call drop() in the destructor",
            obj->getFullName(), getClassName(), getFullPath().c_str(),
            getClassName(),
            getClassName(), obj->getFullName(),
            getClassName());
}

void cObject::yieldOwnership(cOwnedObject *obj, cObject *newowner)
{
    throw cRuntimeError("(%s)%s is currently in (%s)%s, it cannot be inserted into (%s)%s",
            obj->getClassName(), obj->getFullName(),
            getClassName(), getFullPath().c_str(),
            newowner->getClassName(), newowner->getFullPath().c_str());
}

void cObject::take(cOwnedObject *obj)
{
    // ask current owner to release it -- if it's a cDefaultList, it will.
    obj->owner->yieldOwnership(obj, this);
}

void cObject::drop(cOwnedObject *obj)
{
    if (obj->owner != this)
        throw cRuntimeError(this, "drop(): Not owner of object (%s)%s",
                obj->getClassName(), obj->getFullPath().c_str());
    cOwnedObject::defaultOwner->doInsert(obj);
}

void cObject::dropAndDelete(cOwnedObject *obj)
{
    if (!obj)
        return;
    if (obj->owner != this)
        throw cRuntimeError(this, "dropAndDelete(): Not owner of object (%s)%s, owner is (%s)%s",
                obj->getClassName(), obj->getFullPath().c_str(),
                obj->owner->getClassName(), obj->owner->getFullPath().c_str());
    obj->owner = nullptr;
    delete obj;
}

void cObject::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(E_CANTPACK);
}

void cObject::parsimUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(E_CANTPACK);
}

void cObject::copyNotSupported() const
{
    throw cRuntimeError(this, E_CANTCOPY);
}

void cObject::forEachChild(cVisitor *v)
{
}

// Internally used visitors

/**
 * Finds a child object by name.
 */
class cChildObjectFinderVisitor : public cVisitor
{
  protected:
    const char *name;
    cObject *result;

  public:
    cChildObjectFinderVisitor(const char *objname) {
        name = objname;
        result = nullptr;
    }

    virtual void visit(cObject *obj) override {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
    }

    cObject *getResult() { return result; }
};

/**
 * Recursively finds an object by name.
 */
class cRecursiveObjectFinderVisitor : public cVisitor
{
  protected:
    const char *name;
    cObject *result;

  public:
    cRecursiveObjectFinderVisitor(const char *objname) {
        name = objname;
        result = nullptr;
    }

    virtual void visit(cObject *obj) override {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
        obj->forEachChild(this);
    }

    cObject *getResult() { return result; }
};

cObject *cObject::findObject(const char *objname, bool deep)
{
    if (deep) {
        // recursively
        cRecursiveObjectFinderVisitor v(objname);
        v.processChildrenOf(this);
        return v.getResult();
    }
    else {
        // among children
        cChildObjectFinderVisitor v(objname);
        v.processChildrenOf(this);
        return v.getResult();
    }
}

}  // namespace omnetpp

