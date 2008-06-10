//=========================================================================
//  STRINGPOOL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __STRINGPOOL_H_
#define __STRINGPOOL_H_

#include <set>
#include <string.h>
#include "commondefs.h"

NAMESPACE_BEGIN

/**
 * For saving memory on the storage of (largely) constant strings that occur in
 * many instances. (See Flyweight GoF pattern.)
 *
 * Note: this variant doesn't do reference counting, so strings don't need
 * to be released. The downside is that they will only be deallocated in the
 * stringpool object's destructor.
 */
class COMMON_API CommonStringPool
{
  protected:
    struct strless {
        bool operator()(const char *s1, const char *s2) const {
            int d0 = *s1 - *s2;
            if (d0<0) return true; else if (d0>0) return false;
            return strcmp(s1, s2)<0;
        }
    };
    typedef std::set<char *,strless> StringSet;
    StringSet pool;

  public:
    CommonStringPool();
    ~CommonStringPool();
    const char *get(const char *s);
    void clear() {pool.clear();}
};

NAMESPACE_END


#endif


