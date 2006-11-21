//========================================================================
//  CPOLYMORPHIC.CC - part of
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

#include "cpolymorphic.h"
#include "cexception.h"
#include "cclassdescriptor.h"


    
cPolymorphic::~cPolymorphic()
{
    // notify environment
    ev.objectDeleted(this);
}

const char *cPolymorphic::className() const
{
    return opp_typename(typeid(*this));
}

cClassDescriptor *cPolymorphic::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor(this);
}

std::string cPolymorphic::fullPath() const
{
    return std::string(fullName());
}

std::string cPolymorphic::info() const
{
    return std::string();
}

std::string cPolymorphic::detailedInfo() const
{
    return std::string();
}

cPolymorphic *cPolymorphic::dup() const
{
    throw new cRuntimeError("The dup() method, declared in cPolymorphic, is not "
                            "redefined in class %s", className());
}

void cPolymorphic::netPack(cCommBuffer *buffer)
{
}

void cPolymorphic::netUnpack(cCommBuffer *buffer)
{
}

void cPolymorphic::copyNotSupported() const
{
    throw new cRuntimeError(this,eCANTCOPY);
}

void cPolymorphic::forEachChild(cVisitor *v)
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
    cPolymorphic *result;
  public:
    cChildObjectFinderVisitor(const char *objname) {
        name = objname; result = NULL;
    }
    virtual void visit(cPolymorphic *obj) {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
    }
    cPolymorphic *getResult() {return result;}
};

/**
 * Recursively finds an object by name.
 */
class cRecursiveObjectFinderVisitor : public cVisitor
{
  protected:
    const char *name;
    cPolymorphic *result;
  public:
    cRecursiveObjectFinderVisitor(const char *objname) {
        name = objname; result = NULL;
    }
    virtual void visit(cPolymorphic *obj) {
        if (obj->isName(name)) {
            result = obj;
            throw EndTraversalException();
        }
        obj->forEachChild(this);
    }
    cPolymorphic *getResult() {return result;}
};

cPolymorphic *cPolymorphic::findObject(const char *objname, bool deep)
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


