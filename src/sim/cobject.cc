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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cobject.h"
#include "cownedobject.h"
#include "cdefaultlist.h"
#include "cexception.h"
#include "cclassdescriptor.h"
#include "cenvir.h"

NAMESPACE_BEGIN


cObject::~cObject()
{
    // notify environment
    ev.objectDeleted(this);
}

const char *cObject::getClassName() const
{
    return opp_typename(typeid(*this));
}

cClassDescriptor *cObject::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor(this);
}

std::string cObject::getFullPath() const
{
    if (getOwner()==NULL)
        return getFullName();
    else
        return getOwner()->getFullPath() + "." + getFullName();
}

std::string cObject::info() const
{
    return std::string();
}

std::string cObject::detailedInfo() const
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
    obj->ownerp->yieldOwnership(obj, this);
}

void cObject::drop(cOwnedObject *obj)
{
    if (obj->ownerp!=this)
        throw cRuntimeError(this, "drop(): not owner of object (%s)%s",
                                  obj->getClassName(), obj->getFullPath().c_str());
    cOwnedObject::defaultowner->doInsert(obj);
}

void cObject::dropAndDelete(cOwnedObject *obj)
{
    if (!obj)
        return;
    if (obj->ownerp!=this)
        throw cRuntimeError(this, "dropAndDelete(): not owner of object (%s)%s",
                                  obj->getClassName(), obj->getFullPath().c_str());
    obj->ownerp = NULL;
    delete obj;
}

void cObject::parsimPack(cCommBuffer *buffer)
{
}

void cObject::parsimUnpack(cCommBuffer *buffer)
{
}

void cObject::copyNotSupported() const
{
    throw cRuntimeError(this,eCANTCOPY);
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
        name = objname; result = NULL;
    }
    virtual void visit(cObject *obj) {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
    }
    cObject *getResult() {return result;}
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
        name = objname; result = NULL;
    }
    virtual void visit(cObject *obj) {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
        obj->forEachChild(this);
    }
    cObject *getResult() {return result;}
};

cObject *cObject::findObject(const char *objname, bool deep)
{
    if (deep)
    {
        // recursively
        cRecursiveObjectFinderVisitor v(objname);
        v.processChildrenOf(this);
        return v.getResult();
    }
    else
    {
        // among children
        cChildObjectFinderVisitor v(objname);
        v.processChildrenOf(this);
        return v.getResult();
    }
}

NAMESPACE_END

