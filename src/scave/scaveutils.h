//=========================================================================
//  SCAVEUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SCAVEUTILS_H
#define __OMNETPP_SCAVE_SCAVEUTILS_H

#include <string>
#include <set>
#include <functional>
#include <cstdint>
#include "common/commonutil.h"
#include "scavedefs.h"

namespace omnetpp {
namespace scave {

SCAVE_API bool parseInt(const char *str, int& dest);
SCAVE_API bool parseLong(const char *str, long& dest);
SCAVE_API bool parseInt64(const char *str, int64_t& dest);
SCAVE_API bool parseDouble(const char *str, double& dest);
SCAVE_API bool parseSimtime(const char *str, simultime_t& dest);
SCAVE_API std::string unquoteString(const char *str);

class ScaveStringPool
{
    private:
        std::set<std::string> pool;
        const std::string *lastInsertedPtr = nullptr;
    public:
        ScaveStringPool() {}
        const std::string *insert(const std::string& str);
        const std::string *find(const std::string& str) const;
        void clear() { lastInsertedPtr = nullptr; pool.clear(); }
};

}  // namespace scave
}  // namespace omnetpp


#endif

