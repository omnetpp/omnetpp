//=========================================================================
//
//  COBJECT.CC - part of
//                          OMNeT++
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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy, strlen etc.
#include "csimul.h"
#include "cenvir.h"
#include "macros.h"
#include "cobject.h"
#include "cexception.h"
#include "util.h"

//==========================================================================
//=== GLOBAL VARIABLES

int   cObject::staticflag;
int   cObject::heapflag;

//=== Registration
Register_Class(cObject);

//==========================================================================
//=== cObject - member functions

static bool _do_find(cObject *obj, bool beg, const char *objname, cObject *&p, bool deep);
static bool _do_list(cObject *obj, bool beg, ostream& s);

char cObject::fullpathbuf[FULLPATHBUF_SIZE];

#define DETERMINE_STORAGE()  stor = heapflag   ? (heapflag=0, 'D') : \
                                    staticflag ? 'A' : 'S' ;

cObject::cObject(const cObject& obj)
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = opp_strdup( obj.namestr );
    firstchildp = NULL;

    ownerp = NULL;
    setOwner(obj.owner());

    operator=( obj );
}

cObject::cObject()
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = NULL;
    firstchildp = NULL;

    ownerp = NULL;
    setOwner( defaultOwner() );
}

cObject::cObject(const char *name)
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = opp_strdup( name );
    firstchildp = NULL;

    ownerp = NULL;
    setOwner(defaultOwner());
}

cObject::~cObject()
{
    // notify environment
    ev.objectDeleted( this );

    // remove ourselves from owner's list, and delete owned objects
    setOwner(NULL);
    while (firstchildp!=NULL)
        discard( firstchildp );

    // finally, deallocate object name
    delete [] namestr;
}

void *cObject::operator new(size_t m)
{
    void *p = ::new char[m];
    if (p) heapflag = 1;
    return p;
}

cObject& cObject::operator=(const cObject&)
{
    // ownership not affected
    // name string is NOT copied from other object! (24.02.97 --VA)
    return *this;
}

void cObject::copyNotSupported() const
{
    throw new cException(this,eCANTCOPY);
}

const char *cObject::className() const 
{
    return opp_typename(typeid(*this));
}

void cObject::info(char *buf)
{
    sprintf( buf, "%-12s (%s)", fullName()?fullName():"<noname>", className());
}

void cObject::setOwner(cObject *newowner)
{
    // remove from owner's child list
    if (ownerp!=NULL)   
    {
         if (nextp!=NULL)
              nextp->prevp = prevp;
         if (prevp!=NULL)
              prevp->nextp = nextp;
         if (ownerp->firstchildp==this)
              ownerp->firstchildp = nextp;
         ownerp = NULL;
    }

    // insert into owner's child list as first elem
    if (newowner!=NULL) 
    {
         ownerp = newowner;
         prevp = NULL;
         nextp = ownerp->firstchildp;
         if (nextp!=NULL)
              nextp->prevp = this;
         ownerp->firstchildp = this;
    }
}

cObject *cObject::defaultOwner() const
{
    return simulation.localList();
}

const char *cObject::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cObject::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
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


/*------------------------------------------------------------------------*

 The forEach() mechanism
 ~~~~~~~~~~~~~~~~~~~~~~~
  o  The forEach() mechanism implemented in OMNeT++ is very special and
     slightly odd. The passed function is called for each object twice:
     once on entering and once on leaving the object. In addition, after
     the first ('entering') call to the function, it signals with its return
     value whether it wants to go deeper in the contained objects or not.
     Functions passed to forEach() will use static variables to store other
     necessary information. (Yes, this limits their recursive use :-( ).
  o  forEach() takes a function do_fn (of DoItFunc type) with 2 arguments:
     a (cObject *) and a (bool). First, forEach() should call do_fn with
     (this,true) to inform it about entering the object. Then, if this call
     returned true, it must call forEach(do_fn) for every contained object.
     Finally, it must call do_fn with (this,false) to let do_fn know that
     there's no more contained object.
  o  Functions using forEach() work in the following way: they call do_fn
     with (NULL,false,<additional args>) to initialize the static variables
     inside the function. Then they call forEach( (DoItFunc)do_fn ) for the
     given object. Finally, read the results by calling do_fn(NULL, false,
     <additional args>). DoItFuncs mustn't call themselves recursively!
  --VA
     ( yeah, I know this all is kind of weird, but changing it would take
       quite some work --VA )
 *------------------------------------------------------------------------*/

void cObject::forEach( ForeachFunc do_fn )
{
    do_fn(this,true);
    do_fn(this,false);
}

void cObject::writeTo(ostream& os)
{
    os << "(" << className() << ") `" << fullPath() << "' begin\n";
    writeContents( os );
    os << "end\n\n";
}

void cObject::writeContents(ostream& os)
{
    //os << "  objects:\n";
    _do_list( NULL, false, os );                    // prepare do_list
    forEach( (ForeachFunc)_do_list );
}

cObject *cObject::findObject(const char *objname, bool deep)
{
    cObject *p;
    _do_find( NULL, false, objname, p, deep ); // give 'objname' and 'deep' to do_find
    forEach( (ForeachFunc)_do_find );          // perform search
    _do_find( NULL, false, objname, p, deep ); // get result into p
    return p;
}

int cObject::cmpbyname(cObject *one, cObject *other)
{
    return opp_strcmp(one->namestr, other->namestr);
}

static bool _do_find(cObject *obj, bool beg, const char *objname, cObject *&p, bool deep)
{
    static const char *name_str;
    static cObject *r;
    static int ctr;
    static bool deepf;
    if (!obj)
    {
        name_str = objname;
        p = r;
        r = NULL;
        deepf = deep;
        ctr = 0;
        return true;
    }
    if (beg && obj->isName(name_str)) r=obj;
    return deepf || ctr==0;
}

static bool _do_list(cObject *obj, bool beg, ostream& s)
{
    static char buf[256];
    static int ctr;       // static is very important here!!!
    static ostream *os;
    if (!obj)
    {        // setup call
        ctr = 0;
        os = &s;
        return true;
    }

    if (beg)
    {
        if (ctr)
        {
            //*os << "  (" << obj->className() << ") `" << obj->name() << "'\n";
            obj->info(buf);
            *os << "   " << buf << "\n";
        }
        return ctr++ == 0;       // only one level!
    }
    else
        return true;
}

