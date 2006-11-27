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

cStringPool::cStringPool()
{
}

cStringPool::~cStringPool()
{
    // nothing to do -- well-behaved clients should call releaseString()
    // for their strings, so cStringPool would get empty by itself.
}

const char *cStringPool::get(const char *s)
{
    ASSERT(cStaticFlag::isSet()); // don't use stringpool on global objects
    ASSERT(s!=NULL);
    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end())
    {
        char *str = new char[strlen(s)+1];
        strcpy(str, s);
        pool[str] = 1;
        printf("      XXX stringpool: returning new '%s' %p\n", str, str);
        return str;
    }
    else
    {
        it->second++;
        printf("      XXX stringpool returning existing '%s' %p, refcount=%d\n", it->first, it->first, it->second);
        return it->first;
    }
}

void cStringPool::release(const char *s)
{
    if (!cStaticFlag::isSet())
        throw new cRuntimeError("cStringPool: release(\"%s\"): strings must be released before shutdown! "
                                "(Hint: some object with that name being deleted too late?)", s);

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end()) printf("      XXX stringPool::release: string '%s' %p not found\n", s, s);
    if (it->first!=s) printf("      XXX stringPool::release: stringpool has another copy of string '%s' %p\n", s, s);
    ASSERT(it!=pool.end() && it->first==s); // assure correct usage by clients
    printf("      XXX stringpool releasing '%s' %p, remaining refcount=%d\n", s, s, it->second-1);
    if (--(it->second) == 0)
    {
        delete [] s; // that is, it->first
        pool.erase(it);
    }
}


