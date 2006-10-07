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
#include "cobject.h"

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

    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    if (it==pool.end())
    {
        char *str = opp_strdup(s);
        pool[str] = 1;
        return str;
    }
    else
    {
        it->second++;
        return it->first;
    }
}

void cStringPool::release(const char *s)
{
    StringIntMap::iterator it = pool.find(const_cast<char *>(s));
    ASSERT(it!=pool.end()); // assure correct usage by clients
    if (--(it->second) == 0)
    {
        delete [] s; // that is, it->first
        pool.erase(it);
    }
}


