//==========================================================================
//  CSTRINGPOOL.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CSTRINGPOOL_H
#define __OMNETPP_CSTRINGPOOL_H

#include <cstring>
#include <string>
#include <unordered_map>
#include "simkerneldefs.h"

namespace omnetpp {

/**
 * @brief Reference-counted storage for strings.
 *
 * The purpose of this class is to allow saving memory on the storage of
 * (largely) constant strings that occur in many instances during runtime:
 * module names, gate names, property names, keys and values, etc.
 *
 * @see cNamedObject::cNamedObject, cNamedObject::setNamePooling()
 * @ingroup internals
 */
class cStringPool
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
    std::string name;
    std::unordered_map<const char *,int,str_hash> pool; // map<string,refcount>
    bool alive = true; // useful when stringpool is a global variable

  public:
    /**
     * Constructor.
     */
    cStringPool(const char *poolName=nullptr);

    /**
     * Destructor
     */
    ~cStringPool();

    /**
     * Returns pointer to the pooled copy of the given string, and increments
     * its reference count. get() and release() must occur in pairs.
     * Passing nullptr is OK.
     */
    const char *get(const char *s);

    /**
     * Returns pointer to the pooled copy of the given string, or nullptr.
     * Reference count is not incremented. Passing nullptr is OK.
     */
    const char *peek(const char *s) const;

    /**
     * The parameter must be a pointer returned by get(). It decrements the
     * reference count and frees the pooled string if it reaches zero.
     * Passing nullptr is OK.
     */
    void release(const char *s);

    /**
     * For debug purposes.
     */
    void dump() const;
};

}  // namespace omnetpp


#endif


