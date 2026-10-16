//==========================================================================
//  TYPENAMEREGISTRY.H - part of
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

#ifndef __OMNETPP_COMMON_TYPENAMEREGISTRY_H
#define __OMNETPP_COMMON_TYPENAMEREGISTRY_H

#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Produces the C++ type names returned by opp_typename(). The name is obtained
 * by demangling type_info::name(), then normalizing it (see normalize()).
 * Names are cached, and it is verified that different types get different
 * names; violations, as well as demangling and normalization failures, are
 * reported with an exception.
 */
class COMMON_API TypeNameRegistry
{
  private:
    std::unordered_map<std::type_index, std::string> typeNames;
    std::unordered_map<std::string, std::string> demangledNameByTypeName; // to detect name collisions

  public:
    /**
     * Returns the name of the given type. The returned pointer stays valid
     * for the lifetime of the registry.
     */
    const char *getTypeName(const std::type_info& type);

  protected:
    // Steps of computing the type name. Protected (not private) so tests can access them.
    static std::string cleanUpMsvcName(const std::string& name);
    static std::string normalize(const std::string& demangledName);
    static std::string demangle(const std::type_info& type);

    static std::string normalizeTemplateArgs(const std::string& s);
    static std::vector<std::string> splitTemplateArgs(const std::string& s);
    static std::string makeTemplateId(const std::string& name, std::vector<std::string> args);
};

}  // namespace common
}  // namespace omnetpp

#endif
