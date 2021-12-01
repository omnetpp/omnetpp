//==========================================================================
//  RESULTFILEUTILS.CC - part of
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

#include "resultfileutils.h"

#include "omnetpp/simkerneldefs.h"

#include <cstring>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

Register_PerRunConfigOption(CFGID_CONFIG_RECORDING, "config-recording", CFG_CUSTOM, "all", "Selects the set of config options to save into result files. This option can help reduce the size of result files, which is especially useful in the case of large simulation campaigns. Possible values: all, none, config, params, essentials, globalconfig");

std::string ResultFileUtils::getRunId()
{
    cConfigurationEx *cfg = getEnvir()->getConfigEx();
    return cfg->getVariable(CFGVAR_RUNID);
}

StringMap ResultFileUtils::getRunAttributes()
{
    cConfigurationEx *cfg = getEnvir()->getConfigEx();
    std::vector<const char *> keys = cfg->getPredefinedVariableNames();
    StringMap attrs;
    for (const char *key : keys)
        if (strcmp(key, CFGVAR_RUNID) != 0)  // skip runId
            attrs[key] = cfg->getVariable(key);
    return attrs;
}

StringMap ResultFileUtils::getIterationVariables()
{
    cConfigurationEx *cfg = getEnvir()->getConfigEx();
    StringMap itervars;
    std::vector<const char *> keys2 = cfg->getIterationVariableNames();
    for (const char *key : keys2)
        itervars[key] = cfg->getVariable(key);
    return itervars;
}

static std::string removeOptionalQuotes(const char *key, const char *value)
{
    if (value[0] != '"')
        return value;
    const char *lastDot = strrchr(key, '.');
    const char *suffix = lastDot == nullptr ? key : lastDot+1;
    // Note: suffix may be a parameter name (so, not a config option), may contain wildcard, etc.
    // In all such cases, we just won't find it in configOptions and value unquoting will be skipped.
    cConfigOption *option = dynamic_cast<cConfigOption*>(configOptions.getInstance()->lookup(suffix));
    if (option == nullptr)
        return value;
    bool optionallyQuoted = option->getType() == cConfigOption::CFG_STRING || option->getType() == cConfigOption::CFG_FILENAME || option->getType() == cConfigOption::CFG_FILENAMES;
    if (!optionallyQuoted)
        return value;
    const char *endp;
    std::string unquotedValue = opp_parsequotedstr(value, endp);
    if (*endp)
        return value; // cannot be parsed as a single quoted string
    return unquotedValue;
}

OrderedKeyValueList ResultFileUtils::getSelectedConfigEntries()
{
    cConfigurationEx *cfg = getEnvir()->getConfigEx();
    const char *option = cfg->getAsCustom(CFGID_CONFIG_RECORDING);
    int flags = 0;
    for (std::string e : opp_splitandtrim(option)) {
        if (e == "none")
            flags |= 0;
        else if (e == "all")
            flags |= cConfigurationEx::FILT_ALL;
        else if (e == "config")
            flags |= cConfigurationEx::FILT_CONFIG;
        else if (e == "params")
            flags |= cConfigurationEx::FILT_PARAM;
        else if (e == "essentials")
            flags |= cConfigurationEx::FILT_ESSENTIAL_CONFIG;
        else if (e == "globalconfig")
            flags |= cConfigurationEx::FILT_GLOBAL_CONFIG;
        else
            throw cRuntimeError("Unrecognized value '%s' for option '%s'", e.c_str(), CFGID_CONFIG_RECORDING->getName());
    }

    OrderedKeyValueList result;
    std::vector<const char *> keysValues = cfg->getKeyValuePairs(flags);
    for (int i = 0; i < (int)keysValues.size(); i += 2) {
        const char *key = keysValues[i];
        const char *value = keysValues[i+1];
        result.push_back(std::make_pair(key, removeOptionalQuotes(key, value)));
    }
    return result;
}

StringMap ResultFileUtils::convertProperties(const cProperties *properties)
{
    StringMap result;
    if (properties) {
        for (int i = 0; i < properties->getNumProperties(); i++) {
            cProperty *property = properties->get(i);
            const char *name = property->getFullName();
            std::stringstream os;
            property->printValues(os);
            result[name] = os.str();
        }
    }
    return result;
}

StringMap ResultFileUtils::convertMap(const opp_string_map *m)
{
    StringMap result;
    if (m)
        for (auto pair : *m)
            result[pair.first.c_str()] = pair.second.c_str();
    return result;
}


}  // namespace envir
}  // namespace omnetpp

