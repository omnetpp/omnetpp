//=========================================================================
//  CNAMEDOBJECT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy, strlen etc.
#include "cownedobject.h"
#include "csimulation.h"
#include "cenvir.h"
#include "globals.h"
#include "cexception.h"
#include "util.h"
#include "cdefaultlist.h"
#include "cclassdescriptor.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN

using std::ostream;

bool cStaticFlag::staticflag;

Register_Class(cOwnedObject);


#ifdef DEVELOPER_DEBUG
#include <set>

std::set<cOwnedObject*> objectlist;
void printAllObjects()
{
    for (std::set<cOwnedObject*>::iterator it = objectlist.begin(); it != objectlist.end(); ++it)
    {
        printf(" %p (%s)%s\n", (*it), (*it)->className(), (*it)->name());
    }
}
#endif


// static class members
cDefaultList *cOwnedObject::defaultowner = &defaultList;
long cOwnedObject::total_objs = 0;
long cOwnedObject::live_objs = 0;


cDefaultList defaultList;


cOwnedObject::cOwnedObject()
{
    defaultowner->doInsert(this);

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const char *name, bool namepooling) : cNamedObject(name, namepooling)
{
    defaultowner->doInsert(this);

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::cOwnedObject(const cOwnedObject& obj)
{
    setName(obj.name());
    defaultowner->doInsert(this);
    operator=(obj);

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cOwnedObject::~cOwnedObject()
{
#ifdef DEVELOPER_DEBUG
    objectlist.erase(this);
#endif

    if (ownerp)
        ownerp->ownedObjectDeleted(this);

    // statistics
    live_objs--;
}

void cOwnedObject::ownedObjectDeleted(cOwnedObject *obj)
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

void cOwnedObject::yieldOwnership(cOwnedObject *obj, cOwnedObject *newowner)
{
    throw cRuntimeError("(%s)%s is currently in (%s)%s, it cannot be inserted into (%s)%s",
                            obj->className(), obj->fullName(),
                            className(), fullPath().c_str(),
                            newowner->className(), newowner->fullPath().c_str());
}

void cOwnedObject::removeFromOwnershipTree()
{
    // set ownership of this object to null
    if (ownerp)
        ownerp->yieldOwnership(this, NULL);
}

void cOwnedObject::take(cOwnedObject *obj)
{
    // ask current owner to release it -- if it's a cDefaultList, it will.
    obj->ownerp->yieldOwnership(obj, this);
}

void cOwnedObject::drop(cOwnedObject *obj)
{
    if (obj->ownerp!=this)
        throw cRuntimeError(this,"drop(): not owner of object (%s)%s",
                                obj->className(), obj->fullPath().c_str());
    defaultowner->doInsert(obj);
}

void cOwnedObject::dropAndDelete(cOwnedObject *obj)
{
    if (!obj)
        return;
    if (obj->ownerp!=this)
        throw cRuntimeError(this,"dropAndDelete(): not owner of object (%s)%s",
                                obj->className(), obj->fullPath().c_str());
    obj->ownerp = NULL;
    delete obj;
}


void cOwnedObject::setDefaultOwner(cDefaultList *list)
{
    ASSERT(list!=NULL);
    defaultowner = list;
}

cDefaultList *cOwnedObject::defaultOwner()
{
    return defaultowner;
}

cOwnedObject& cOwnedObject::operator=(const cOwnedObject& obj)
{
    // Not too much to do:
    // - ownership not affected
    // - name string is NOT copied from other object
    // - flags are taken over, except for FL_NAMEPOOLING which is preserved
    unsigned short namePooling = flags & FL_NAMEPOOLING;
    flags = (obj.flags & ~FL_NAMEPOOLING) | namePooling;
    return *this;
}

void cOwnedObject::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cNamedObject::netPack(buffer);
#endif
}

void cOwnedObject::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cNamedObject::netUnpack(buffer);
#endif
}

//-----

cNoncopyableOwnedObject *cNoncopyableOwnedObject::dup() const
{
    throw cRuntimeError(this, "dup(): %s subclasses from cNoncopyableOwnedObject, "
                              "and does not support dup()", className());
}

void cNoncopyableOwnedObject::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "netPack(): %s subclasses from cNoncopyableOwnedObject, and "
                              "does not support pack/unpack operations", className());
}

void cNoncopyableOwnedObject::netUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, "netUnpack(): %s subclasses from cNoncopyableOwnedObject, and "
                              "does not support pack/unpack operations", className());
}

//-----

ostream& operator<< (ostream& os, const cOwnedObject *p)
{
    if (!p)
        return os << "(NULL)";
    return os << "(" << p->className() << ")" << p->fullName();
}

ostream& operator<< (ostream& os, const cOwnedObject& o)
{
    return os << "(" << o.className() << ")" << o.fullName();
}

NAMESPACE_END
