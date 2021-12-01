//=========================================================================
//  STRINGPOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "pooledstring.h"
#include "stringutil.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

const char * const StaticStringPool::EMPTY_STRING = "";

StaticStringPool::~StaticStringPool()
{
    for (const char *str : pool)
        delete[] const_cast<char *>(str);
}

void StaticStringPool::clear()
{
    for (const char *str : pool)
        delete[] const_cast<char *>(str);
    pool.clear();
}

const char *StaticStringPool::get(const char *s)
{
    if (!s)
        return nullptr;
    if (!*s)
        return EMPTY_STRING;
    auto it = pool.find(s);
    if (it != pool.end())
        return *it;
    char *str = new char[strlen(s)+1];
    strcpy(str, s);
    pool.insert(str);
    return str;
}

bool StaticStringPool::contains(const char *s) const
{
    return (!s || !*s) ? true : pool.find(s) != pool.end();
}

//---

const char * const StringPool::EMPTY_STRING = "";

StringPool::~StringPool()
{
    // do not release strings -- clients are supposed to call release()
    // for their strings, so StringPool gets empty by itself.

    // we may want to dump unreleased strings (except after Ctrl+C on Windows)
    //if (!cStaticFlag::isExiting())
    //    dump();

    alive = false;
}

const char *StringPool::obtain(const char *s)
{
#ifndef NDEBUG
    if (!opp_insidemain()) {
        fprintf(stderr, "ERROR: StringPool::get(\"%s\") invoked outside main() -- please do not use StringPool from global objects", s);
        return opp_strdup(s);
    }
#endif

    if (!s)
        return nullptr;
    if (!*s)
        return EMPTY_STRING;

    auto it = pool.find(s);
    if (it == pool.end()) {
        // allocate new string
        char *str = new char[strlen(s)+1];
        strcpy(str, s);
        pool[str] = 1;
        return str;
    }
    else {
        // increment refcount of existing string
        it->second++;
        return it->first;
    }
}

const char *StringPool::peek(const char *s) const
{
#ifndef NDEBUG
    if (!opp_insidemain()) {
        fprintf(stderr, "ERROR: StringPool::peek(\"%s\") invoked outside main() -- please do not use StringPool from global objects", s);
        return nullptr;
    }
#endif

    if (!s)
        return nullptr;
    if (!*s)
        return EMPTY_STRING;

    auto it = pool.find(s);
    return it == pool.end() ? nullptr : it->first;
}

void StringPool::release(const char *s)
{
#ifndef NDEBUG
    if (!opp_insidemain()) {
        fprintf(stderr, "Warning: StringPool::release(): string \"%s\" released too late, after main() already exited\n", s);
        return;
    }
#endif

    if (!s || !*s)
        return;
    if (!alive)
        return;  // prevents crash when stringpool is a global object, and program exits via exit()

    auto it = pool.find(s);

    // sanity checks
    if (it == pool.end()) {
        fprintf(stderr, "ERROR: StringPool::release(): string %p \"%s\" not in stringpool\n", s, s);
        return;
    }
    if (it->first != s) {
        fprintf(stderr, "ERROR: StringPool::release(): wrong string pointer %p \"%s\", stringpool has a different copy of the same string\n", s, s);
        return;
    }

    // decrement refcount or release string
    if (--(it->second) == 0) {
        delete[] s;  // that is, it->first
        pool.erase(it);
    }
}

void StringPool::dump() const
{
    for (const auto & it : pool)
        printf("  \"%s\" %p, %d ref(s)\n", it.first, it.first, it.second);
}


}  // namespace common
}  // namespace omnetpp

