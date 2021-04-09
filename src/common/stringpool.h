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

#include <unordered_set>
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
    struct str_hash {
        std::size_t operator()(const char *s) const {
            size_t result = 0;
            const size_t prime = 31;
            for (; *s; s++)
                result = result*prime + *s;
            return result;
        }
    };
    std::unordered_set<const char *,str_hash> pool;

  public:
    StringPool() {}
    ~StringPool();
    const char *get(const char *s);
    bool contains(const char *s) const;
    void clear();
};

} // namespace common
}  // namespace omnetpp


#endif


