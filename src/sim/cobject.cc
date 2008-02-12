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


