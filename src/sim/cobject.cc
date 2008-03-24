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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cobject.h"
#include "cownedobject.h"
#include "cdefaultlist.h"
#include "cexception.h"
#include "cclassdescriptor.h"

USING_NAMESPACE


cObject::~cObject()
{
    // notify environment
    ev.objectDeleted(this);
}

const char *cObject::className() const
{
    return opp_typename(typeid(*this));
}

cClassDescriptor *cObject::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor(this);
}

std::string cObject::fullPath() const
{
    if (owner()==NULL)
        return fullName();
    else
        return owner()->fullPath() + "." + fullName();
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
                        "redefined in class %s", className());
}

void cObject::ownedObjectDeleted(cOwnedObject *obj)
{
    // Note: too late to call obj->className(), at this point it'll aways return "cOwnedObject"
    throw cRuntimeError("Object %s is currently in (%s)%s, it cannot be deleted. "
                        "If this error occurs inside %s, it needs to be changed "
                        "to call drop() before it can delete that object. "
                        "If this error occurs inside %s's destructor and %s is a class member, "
                        "%s needs to call drop() in the destructor",
                        obj->fullName(), className(), fullPath().c_str(),
                        className(),
                        className(), obj->fullName(),
                        className());
}

void cObject::yieldOwnership(cOwnedObject *obj, cObject *newowner)
{
    throw cRuntimeError("(%s)%s is currently in (%s)%s, it cannot be inserted into (%s)%s",
                        obj->className(), obj->fullName(),
                        className(), fullPath().c_str(),
                        newowner->className(), newowner->fullPath().c_str());
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
                                  obj->className(), obj->fullPath().c_str());
    cOwnedObject::defaultowner->doInsert(obj);
}

void cObject::dropAndDelete(cOwnedObject *obj)
{
    if (!obj)
        return;
    if (obj->ownerp!=this)
        throw cRuntimeError(this, "dropAndDelete(): not owner of object (%s)%s",
                                  obj->className(), obj->fullPath().c_str());
    obj->ownerp = NULL;
    delete obj;
}

void cObject::netPack(cCommBuffer *buffer)
{
}

void cObject::netUnpack(cCommBuffer *buffer)
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


