//==========================================================================
//  CSTRINGPOOL.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cstringpool.h"
#include "cownedobject.h"

USING_NAMESPACE

cStringPool::cStringPool()
{
}

cStringPool::~cStringPool()
{
    // nothing to do -- well-behaved clients should call releaseString()
    // for their strings, so cStringPool gets empty by itself.
}

const char *cStringPool::get(const char *s)
{
    ASSERT(cStaticFlag::isSet()); // don't use stringpool on global objects
    if (!s)
        return NULL;

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end())
    {
        char *str = new char[strlen(s)+1];
        strcpy(str, s);
        pool[str] = 1;
        //printf("      XXX stringpool: returning new '%s' %p\n", str, str);
        return str;
    }
    else
    {
        it->second++;
        //printf("      XXX stringpool returning existing '%s' %p, refcount=%d\n", it->first, it->first, it->second);
        return it->first;
    }
}

const char *cStringPool::peek(const char *s)
{
    ASSERT(cStaticFlag::isSet()); // don't use stringpool on global objects
    if (!s)
        return NULL;

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    return it==pool.end() ? NULL : it->first;
}

void cStringPool::release(const char *s)
{
    if (!s)
        return;
    if (!cStaticFlag::isSet())
    {
        printf("Warning: cStringPool::release: string '%s' released too late, after main() already exited\n", s);
        return; // it is unsafe to release it, because our dtor may have run already
    }

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end()) printf("      XXX stringPool::release: string '%s' %p not found\n", s, s);
    if (it->first!=s) printf("      XXX stringPool::release: stringpool has another copy of string '%s' %p\n", s, s);
    ASSERT(it!=pool.end() && it->first==s); // s must point to a pooled string
    //printf("      XXX stringpool releasing '%s' %p, remaining refcount=%d\n", s, s, it->second-1);
    if (--(it->second) == 0)
    {
        delete [] s; // that is, it->first
        pool.erase(it);
    }
}


