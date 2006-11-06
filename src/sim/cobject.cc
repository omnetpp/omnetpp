//=========================================================================
//  COBJECT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Static members of
//    cObject
//
//   Member functions of
//    cObject   : ultimate base class for most objects
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
#include <assert.h>
#include "csimul.h"
#include "cenvir.h"
#include "macros.h"
#include "cobject.h"
#include "cexception.h"
#include "util.h"
#include "cdefaultlist.h"
#include "cclassdescriptor.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;


bool cStaticFlag::staticflag;

//=== Registration
Register_Class(cObject);


#ifdef DEVELOPER_DEBUG
#include <set>
std::set<cObject*> objectlist;
void printAllObjects()
{
    for (std::set<cObject*>::iterator it = objectlist.begin(); it != objectlist.end(); ++it)
    {
        printf(" %p (%s)%s\n", (*it), (*it)->className(), (*it)->name());
    }
}
#endif


//==========================================================================
//=== Internally used visitors

/**
 * Calls os << obj->info() for every object.
 */
class cPrintInfoVisitor : public cVisitor
{
  protected:
    ostream& os;
  public:
    cPrintInfoVisitor(ostream& ostr) : os(ostr) {}
    virtual void visit(cObject *obj) {os << obj->info() << "\n";}
};

/**
 * Finds a child object by name.
 */
class cChildObjectFinderVisitor : public cVisitor
{
  protected:
    const char *name;
    cObject *result;
  public:
    cChildObjectFinderVisitor(const char *objname) {name=objname; result=NULL;}
    virtual void visit(cObject *obj) {
        if (obj->isName(name)) {
            result=obj;
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
    cRecursiveObjectFinderVisitor(const char *objname) {name=objname; result=NULL;}
    virtual void visit(cObject *obj) {
        if (obj->isName(name)) {
            result=obj;
            throw EndTraversalException();
        }
        obj->forEachChild(this);
    }
    cObject *getResult() {return result;}
};

//==========================================================================
//=== cObject - member functions

// static class members
char cObject::fullpathbuf[MAX_OBJECTFULLPATH];
cDefaultList *cObject::defaultowner = &defaultList;
long cObject::total_objs = 0;
long cObject::live_objs = 0;


cDefaultList defaultList;


cObject::cObject(const cObject& obj)
{
    nameunion.chars[MAX_INTERNAL_NAME] = nameunion.chars[0] = '\0';
    setName(obj.name());
    defaultowner->doInsert(this);
    operator=( obj );

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cObject::cObject()
{
    nameunion.chars[MAX_INTERNAL_NAME] = nameunion.chars[0] = '\0';
    defaultowner->doInsert(this);

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cObject::cObject(const char *name)
{
    nameunion.chars[MAX_INTERNAL_NAME] = nameunion.chars[0] = '\0';
    setName(name);
    defaultowner->doInsert(this);

    // statistics
    total_objs++;
    live_objs++;
#ifdef DEVELOPER_DEBUG
    objectlist.insert(this);
#endif
}

cObject::~cObject()
{
#ifdef DEVELOPER_DEBUG
    objectlist.erase(this);
#endif

    if (ownerp)
        ownerp->ownedObjectDeleted(this);

    // notify environment
    ev.objectDeleted(this);

    if (nameunion.chars[MAX_INTERNAL_NAME])
        delete [] nameunion.p;

    // statistics
    live_objs--;
}

void cObject::ownedObjectDeleted(cObject *obj)
{
    // Note: too late to call obj->className(), at this point it'll aways return "cObject"
    throw new cRuntimeError("Object %s is currently in (%s)%s, it cannot be deleted. "
                            "If this error occurs inside %s, it needs to be changed "
                            "to call drop() before it can delete that object. "
                            "If this error occurs inside %s's destructor and %s is a class member, "
                            "%s needs to call drop() in the destructor",
                            obj->fullName(), className(), fullPath().c_str(),
                            className(),
                            className(), obj->fullName(),
                            className());
}

void cObject::yieldOwnership(cObject *obj, cObject *newowner)
{
    throw new cRuntimeError("(%s)%s is currently in (%s)%s, it cannot be inserted into (%s)%s",
                            obj->className(), obj->fullName(),
                            className(), fullPath().c_str(),
                            newowner->className(), newowner->fullPath().c_str());
}

void cObject::removeFromOwnershipTree()
{
    // set ownership of this object to null
    if (ownerp)
        ownerp->yieldOwnership(this, NULL);
}

void cObject::take(cObject *obj)
{
    // ask current owner to release it -- if it's a cDefaultList, it will.
    obj->ownerp->yieldOwnership(obj, this);
}

void cObject::drop(cObject *obj)
{
    if (obj->ownerp!=this)
        throw new cRuntimeError(this,"drop(): not owner of object (%s)%s",
                                obj->className(), obj->fullPath().c_str());
    defaultowner->doInsert(obj);
}

void cObject::dropAndDelete(cObject *obj)
{
    if (!obj)
        return;
    if (obj->ownerp!=this)
        throw new cRuntimeError(this,"dropAndDelete(): not owner of object (%s)%s",
                                obj->className(), obj->fullPath().c_str());
    obj->ownerp = NULL;
    delete obj;
}


void cObject::setDefaultOwner(cDefaultList *list)
{
    assert(list!=NULL);
    defaultowner = list;
}

cDefaultList *cObject::defaultOwner()
{
    return defaultowner;
}

cObject& cObject::operator=(const cObject&)
{
    // ownership not affected
    // name string is NOT copied from other object
    return *this;
}

void cObject::setName(const char *s)
{
    // if name is "external" (p used), then deallocate it
    if (nameunion.chars[MAX_INTERNAL_NAME])
        delete [] nameunion.p;

    if (!s)
    {
        // set beginning of name and discriminator to 0
        nameunion.chars[MAX_INTERNAL_NAME] = nameunion.chars[0] = '\0';
    }
    else if (strlen(s)<=MAX_INTERNAL_NAME)
    {
        // copy string to internal buffer, and set discriminator to 0
        // (discriminator also serves as terminating zero for long strings)
        strcpy(nameunion.chars, s);
        nameunion.chars[MAX_INTERNAL_NAME] = '\0';
    }
    else
    {
        // create a copy, then set discriminator to 1 to signal p is used
        nameunion.p  = opp_strdup(s);
        nameunion.chars[MAX_INTERNAL_NAME] = '\1';
    }
}

void cObject::copyNotSupported() const
{
    throw new cRuntimeError(this,eCANTCOPY);
}

void cObject::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    buffer->pack(name());
#endif
}

void cObject::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    opp_string tmp;
    buffer->unpack(tmp);
    setName(tmp.buffer());
#endif
}

std::string cObject::fullPath() const
{
    return std::string(fullPath(fullpathbuf,MAX_OBJECTFULLPATH));
}

const char *cObject::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no buffer or buffer too small)";
    }

    // append parent path + "."
    char *buf = buffer;
    if (owner()!=NULL)
    {
       owner()->fullPath(buf,bufsize);
       int len = strlen(buf);
       buf+=len;
       bufsize-=len;
       *buf++ = '.';
       bufsize--;
    }

    // append our own name
    opp_strprettytrunc(buf, fullName(), bufsize-1);
    return buffer;
}

void cObject::forEachChild(cVisitor *v)
{
}

void cObject::writeTo(ostream& os)
{
    os << "(" << className() << ") `" << fullPath() << "' begin\n";
    writeContents( os );
    os << "end\n\n";
}

void cObject::writeContents(ostream& os)
{
    os << detailedInfo() << std::endl;
    cPrintInfoVisitor v(os);
    v.processChildrenOf(this);
}

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

int cObject::cmpbyname(cObject *one, cObject *other)
{
    return opp_strcmp(one->name(), other->name());
}

ostream& operator<< (ostream& os, const cObject *p)
{
    if (!p)
        return os << "(NULL)";
    return os << "(" << p->className() << ")" << p->fullName();
}

ostream& operator<< (ostream& os, const cObject& o)
{
    return os << "(" << o.className() << ")" << o.fullName();
}


