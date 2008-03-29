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
    alive = true;
}

cStringPool::~cStringPool()
{
    // do not release strings -- clients are supposed to call releaseString()
    // for their strings, so cStringPool gets empty by itself.
#ifndef NDEBUG
    dump();
#endif
    alive = false;
}

void cStringPool::dump() const
{
    if (!pool.empty()) {
        printf("stringpool contents%s:\n", cStaticFlag::isSet() ? "" : " (after returning from main())");
        for (StringIntMap::const_iterator it = pool.begin(); it!=pool.end(); ++it)
            printf("  \"%s\" %p (%d ref(s))\n", it->first, it->first, it->second);
    }
}

const char *cStringPool::get(const char *s)
{
    if (!cStaticFlag::isSet())
        {fprintf(stderr, "ERROR: cStringPool::get(\"%s\") invoked outside main() -- please do not use cStringPool from global objects", s); return opp_strdup(s);}
    if (!s)
        return NULL;

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end())
    {
        // allocate new string
        char *str = new char[strlen(s)+1];
        strcpy(str, s);
        pool[str] = 1;
        return str;
    }
    else
    {
        // increment refcount of existing string
        it->second++;
        return it->first;
    }
}

const char *cStringPool::peek(const char *s) const
{
    if (!cStaticFlag::isSet())
        {fprintf(stderr, "ERROR: cStringPool::peek(\"%s\") invoked outside main() -- please do not use cStringPool from global objects", s); return false;}
    if (!s)
        return NULL;

    StringIntMap::const_iterator it = pool.find(const_cast<char *>(s));
    return it==pool.end() ? NULL : it->first;
}

void cStringPool::release(const char *s)
{
    if (!s)
        return;
    if (!alive)
        return; // prevents crash when stringpool is a global object, and program exits via exit()
    if (!cStaticFlag::isSet())
        {fprintf(stderr, "Warning: cStringPool::release(): string \"%s\" released too late, after main() already exited\n", s); return;}

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));

    // sanity checks
    if (it==pool.end()) 
        {fprintf(stderr, "ERROR: cStringPool::release(): string %p \"%s\" not in stringpool\n", s, s); return;}
    if (it->first!=s) 
        {fprintf(stderr, "ERROR: cStringPool::release(): wrong string pointer %p \"%s\", stringpool has a different copy of the same string\n", s, s); return;}

    // decrement refcount or release string
    if (--(it->second) == 0)
    {
        delete [] s; // that is, it->first
        pool.erase(it);
    }
}


