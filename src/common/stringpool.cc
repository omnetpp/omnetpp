//=========================================================================
//  STRINGPOOL.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stringpool.h"

namespace omnetpp {
namespace common {

StringPool::StringPool()
{
}

StringPool::~StringPool()
{
    for (const char *str : pool)
        delete[] const_cast<char *>(str);
}

void StringPool::clear()
{
    for (const char *str : pool)
        delete[] const_cast<char *>(str);
    pool.clear();
}

const char *StringPool::get(const char *s)
{
    if (s == nullptr)
        return "";  // must not be nullptr because SWIG-generated code will crash!
    StringSet::iterator it = pool.find(s);
    if (it != pool.end())
        return *it;
    char *str = new char[strlen(s)+1];
    strcpy(str, s);
    pool.insert(str);
    return str;
}

bool StringPool::contains(const char *s) const
{
    return s == nullptr ? true : pool.find(s) != pool.end();
}

}  // namespace common
}  // namespace omnetpp

