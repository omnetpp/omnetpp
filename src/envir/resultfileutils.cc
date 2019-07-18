//==========================================================================
//  RESULTFILEUTILS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Andras Varga
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
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

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

OrderedKeyValueList ResultFileUtils::getConfigEntries()
{
    cConfigurationEx *cfg = getEnvir()->getConfigEx();
    OrderedKeyValueList result;
    std::vector<const char *> keysValues = cfg->getKeyValuePairs();
    for (int i = 0; i < (int)keysValues.size(); i += 2)
        result.push_back(std::make_pair(keysValues[i], keysValues[i+1]));
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


}  // namespace envir
}  // namespace omnetpp

