//=========================================================================
//
//  CARRAY.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cBag   : expandable vector to store small non-class items
//    cArray : vector to store cObject descendants
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
#include <string.h>          // memcmp, memcpy, memset
#include "macros.h"
#include "errmsg.h"
#include "carray.h"
#include "cexception.h"

//=== Registration
Register_Class(cBag);
Register_Class(cArray);

//==========================================================================
//=== cBag - member functions

#define BOOL     sizeof(bool)
#define PTR(m)   ((char *)vect + (m)*(BOOL+elemsize))
#define ELEM(m)  ((void *)((char *)vect + (m)*(BOOL+elemsize) + BOOL))
#define USED(m)  (*(bool *)((char *)vect + (m)*(BOOL+elemsize)))
#define BLK      (BOOL+elemsize)

cBag::cBag(const cBag& bag) : cObject()
{
   vect = NULL;
   setName( bag.name() );
   operator=(bag);
}

cBag::cBag(const char *name, int esiz, int siz, int delt) :
cObject( name ), vect(NULL)
{
    setup(esiz, siz, delt);
}

void cBag::info(char *buf)
{
    cObject::info( buf );

    if (lastused==-1)
        sprintf( buf+strlen(buf), " (empty)" );
    else
        sprintf( buf+strlen(buf), " (size=%d) ", lastused+1);
}

cBag& cBag::operator=(const cBag& bag)
{
    if (this==&bag) return *this;

    cObject::operator=( bag );
    elemsize = bag.elemsize;
    size = bag.size;
    delta = bag.delta;
    lastused = bag.lastused;
    firstfree = bag.firstfree;
    delete [] vect;
    vect = new char[ size*BLK ];
    if( vect )  memcpy( vect, bag.vect, size*BLK );
    return *this;
}

void cBag::setup(int esiz, int siz, int delt)
{
   clear();
   elemsize = Max(1,esiz);
   delta = Max(1,delt);
   size = Max(siz,0);
   vect = new char[ size*BLK ];
   for (int i=0; i<size; i++) USED(i)=false;
}

void cBag::clear()
{
   delete [] vect;
   vect = NULL;
   size = firstfree = 0;
   lastused = -1;
   for (int i=0; i<size; i++) USED(i)=false; // just to be safe
}

int cBag::add(void *obj)
{
   int retval;
   if (firstfree < size) // fits in current vector
   {
      USED(firstfree) = true;
      memcpy( ELEM(firstfree), obj, elemsize);
      retval = firstfree;
      lastused = Max(lastused,firstfree);
      do {
         firstfree++;
      } while (USED(firstfree) && firstfree<=lastused);
   }
   else // must allocate bigger vector
   {
      char *v = new char[ (size+delta)*BLK ];
      memcpy(v, vect, size*BLK );
      delete [] vect;
      vect = v;
      for (int i=size; i<size+delta; i++) USED(i)=false;
      USED(size) = true;
      memcpy( ELEM(size), obj, elemsize);
      retval = size;
      lastused = size;
      firstfree = size+1;
      size += delta;
   }
   return retval;
}

int cBag::addAt(int m, void *obj) // --LG
{
   if (m < size)  // fits in current vector
   {
      USED(m) = true;
      memcpy( ELEM(m), obj, elemsize);
      lastused = Max(lastused,m);
      if (m==firstfree)
         do {
            firstfree++;
         } while (USED(firstfree) && firstfree<=lastused);
      return m;
   }
   else // must allocate bigger vector
   {
      int newsize = Max(size+delta,m+1);
      char *v = new char[ newsize*BLK ];
      memcpy(v, vect, size*BLK );
      delete [] vect;
      vect = v;
      for (int i=size; i<newsize; i++) USED(i)=false;
      USED(m) = true;
      memcpy( ELEM(m), obj, elemsize);
      lastused = m;
      if (m==firstfree)
          firstfree++;
      size = newsize;
      return m;
   }
}

int cBag::find(void *obj) const
{
    int i;
    for (i=0; i<=lastused; i++)
        if( USED(i) && memcmp( ELEM(i), obj, elemsize)==0 ) break;
    if (i<=lastused)
        return i;
    else
        return -1;
}

void *cBag::get(int m)
{
    if( m>=0 && m<=lastused && USED(m) )
        return ELEM(m);
    else
        return NULL;
}

const void *cBag::get(int m) const
{
    if( m>=0 && m<=lastused && USED(m) )
        return ELEM(m);
    else
        return NULL;
}


bool cBag::isUsed(int m) const
{
    if (m>=0 && m<=lastused)
        return USED(m);
    else
        return false;
}

void *cBag::remove(int m)
{
    if (m<0 || m>lastused || !USED(m))
        return NULL;

    USED(m)=false;
    firstfree = Min(firstfree, m);
    if (m==lastused)
        do {
           lastused--;
        } while ( !USED(lastused) && lastused>=0);
    return ELEM(m);
}

#undef BOOL
#undef PTR
#undef ELEM
#undef USED
#undef BLK

//==========================================================================
//=== cArray - member functions

cArray::cArray(const cArray& list) : cObject()
{
    vect=NULL;
    last=-1;
    setName( list.name() );
    operator=(list);
}

cArray::cArray(const char *name, int siz, int dt) :
cObject( name )
{
    delta = Max(1,dt);
    size = Max(siz,0);
    firstfree = 0;
    last = -1;
    vect = new cObject *[size];
    for (int i=0; i<size; i++) vect[i]=NULL;
}

cArray::~cArray()
{
    // no clear()!
    delete [] vect;
}

cArray& cArray::operator=(const cArray& list)
{
    if (this==&list) return *this;

    clear();

    cObject::operator=( list );

    size = list.size;
    delta = list.delta;
    firstfree = list.firstfree;
    last = list.last;
    delete [] vect;
    vect = new cObject *[size];
    if (vect) memcpy( vect, list.vect, size * sizeof(cObject *) );

    for (int i=0; i<=last; i++)
        if (vect[i] && vect[i]->owner()==const_cast<cArray*>(&list))
            take( vect[i] = vect[i]->dup() );
    return *this;
}

void cArray::info(char *buf)
{
    cObject::info( buf );

    if (last==-1)
        sprintf( buf+strlen(buf), " (empty)" );
    else
        sprintf( buf+strlen(buf), " (size=%d)", last+1);
}

void cArray::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
        for (int i=0; i<=last; i++)
        {
            cObject *p = vect[i];
            if (p) p->forEach( do_fn );
        }
    }
    do_fn(this,false);
}

void cArray::clear()
{
    for (int i=0; i<=last; i++)
    {
        if (vect[i] && vect[i]->owner()==this)
           discard( vect[i] );
        vect[i] = NULL;  // this is not strictly necessary
    }
    firstfree = 0;
    last = -1;
}

int cArray::add(cObject *obj)
{
    if (!obj)
        throw new cException(this,"cannot insert NULL pointer");

    int retval;
    if (takeOwnership()) take( obj );
    if (firstfree < size)  // fits in current vector
    {
        vect[firstfree] = obj;
        retval = firstfree;
        last = Max(last,firstfree);
        do {
            firstfree++;
        } while (firstfree<=last && vect[firstfree]!=NULL);
    }
    else // must allocate bigger vector
    {
        cObject **v = new cObject *[size+delta];
        memcpy(v, vect, sizeof(cObject*)*size );
        memset(v+size, 0, sizeof(cObject*)*delta);
        delete [] vect;
        vect = v;
        vect[size] = obj;
        retval = last = size;
        firstfree = size+1;
        size += delta;
    }
    return retval;
}

int cArray::addAt(int m, cObject *obj)
{
    if (!obj)
        throw new cException(this,"cannot insert NULL pointer");

    if (m<size)  // fits in current vector
    {
        if (m<0)
            throw new cException(this,"addAt(): negative position %d",m);
        if (vect[m]!=NULL)
            throw new cException(this,"addAt(): position %d already used",m);
        vect[m] = obj;
        if (takeOwnership()) take(obj);
        last = Max(m,last);
        if (firstfree==m)
            do {
                firstfree++;
            } while (firstfree<=last && vect[firstfree]!=NULL);
    }
    else // must allocate bigger vector
    {
        cObject **v = new cObject *[m+delta];
        memcpy(v, vect, sizeof(cObject*)*size);
        memset(v+size, 0, sizeof(cObject*)*(m+delta-size));
        delete [] vect;
        vect = v;
        vect[m] = obj;
        if (takeOwnership()) take(obj);
        size = m+delta;
        last = m;
        if (firstfree==m)
            firstfree++;
    }
    return m;
}

int cArray::set(cObject *obj)
{
    if (!obj)
        throw new cException(this,"cannot insert NULL pointer");

    int i = find(obj->name());
    if (i<0)
    {
        return add(obj);
    }
    else
    {
        if (vect[i]->owner()==this) discard(vect[i]);
        vect[i] = obj;
        if (takeOwnership()) take(obj);
        return i;
    }
}

int cArray::find(cObject *obj) const
{
    int i;
    for (i=0; i<=last; i++)
        if (vect[i]==obj)
            break;
    if (i<=last)
        return i;
    else
        return -1;
}

int cArray::find(const char *objname) const
{
    int i;
    for (i=0; i<=last; i++)
        if (vect[i] && vect[i]->isName(objname))
            break;
    if (i<=last)
        return i;
    else
        return -1;
}

cObject *cArray::get(int m)
{
    if (m>=0 && m<=last && vect[m])
        return vect[m];
    else
        return NULL;
}

const cObject *cArray::get(int m) const
{
    if (m>=0 && m<=last && vect[m])
        return vect[m];
    else
        return NULL;
}

cObject *cArray::get(const char *objname)
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return get(m);
}

const cObject *cArray::get(const char *objname) const
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return get(m);
}

cObject *cArray::remove(const char *objname)
{
    int m = find( objname );
    if (m==-1)
        return NULL;
    return remove(m);
}

cObject *cArray::remove(cObject *obj)
{
    if (!obj) return NULL;

    int m = find( obj );
    if (m==-1)
        return NULL;
    return remove(m);
}

cObject *cArray::remove(int m)
{
    if (m<0 || m>last || vect[m]==NULL)
        return NULL;

    cObject *obj = vect[m]; vect[m] = NULL;
    firstfree = Min(firstfree, m);
    if (m==last)
        do {
            last--;
        } while (last>=0 && vect[last]==NULL);
    if (obj->owner()==this)  drop( obj );
    return obj;
}


