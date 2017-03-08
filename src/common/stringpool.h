//=========================================================================
//  STRINGPOOL.H - part of
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

#ifndef __OMNETPP_COMMON_STRINGPOOL_H
#define __OMNETPP_COMMON_STRINGPOOL_H

#include <set>
#include <cstring>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * For saving memory on the storage of (largely) constant strings that occur in
 * many instances. (See Flyweight GoF pattern.)
 *
 * Note: this variant does not do reference counting, so strings do not need
 * to be released. The downside is that they will only be deallocated in the
 * stringpool object's destructor.
 */
class COMMON_API StringPool
{
  protected:
    struct strless {
        bool operator()(const char *s1, const char *s2) const {
            int diff = *s1 - *s2;
            if (diff < 0)
                return true;
            else if (diff > 0)
                return false;
            else
                return strcmp(s1, s2)<0;
        }
    };
    typedef std::set<char *,strless> StringSet;
    StringSet pool;

  public:
    StringPool();
    ~StringPool();
    const char *get(const char *s);
    void clear();
};

} // namespace common
}  // namespace omnetpp


#endif


