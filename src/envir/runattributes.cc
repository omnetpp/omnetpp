//==========================================================================
//  RUNATTRIBUTES.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <cstring>
#include "common/stringutil.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"
#include "runattributes.h"

using namespace OPP::common;

NAMESPACE_BEGIN
namespace envir {

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())


void RunData::initRun()
{
    if (!initialized) {
        // Collect the attributes and module parameters of the current run
        // from the configuration.
        cConfigurationEx *cfg = getEnvir()->getConfigEx();
        runId = cfg->getVariable(CFGVAR_RUNID);

        std::vector<const char *> keys1 = cfg->getPredefinedVariableNames();
        for (int i = 0; i < (int)keys1.size(); i++)
            if (OPP::opp_strcmp(keys1[i], CFGVAR_RUNID) != 0)  // skip runId
                attributes[keys1[i]] = cfg->getVariable(keys1[i]);


        std::vector<const char *> keys2 = cfg->getIterationVariableNames();
        for (int i = 0; i < (int)keys2.size(); i++)
            attributes[keys2[i]] = cfg->getVariable(keys2[i]);

/*XXX
        std::vector<const char *> keys = cfg->getMatchingConfigKeys("*");
        for (int i=0; i<(int)keys.size(); i++)
        {
            const char *key = keys[i];
            config[key] = cfg->getConfigValue(key);
        }
*/
        // fill in moduleParams[]
        std::vector<const char *> params = cfg->getParameterKeyValuePairs();
        for (int i = 0; i < (int)params.size(); i += 2)
            moduleParams[params[i]] = params[i+1];

        initialized = true;
    }
}

void RunData::reset()
{
    initialized = false;
    attributes.clear();
    moduleParams.clear();
}

void RunData::writeRunData(FILE *f, opp_string fname)
{
    CHECK(fprintf(f, "run %s\n", QUOTE(runId.c_str())));
    for (opp_string_map::const_iterator it = attributes.begin(); it != attributes.end(); ++it) {
        CHECK(fprintf(f, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    for (opp_string_map::const_iterator it = moduleParams.begin(); it != moduleParams.end(); ++it) {
        CHECK(fprintf(f, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    CHECK(fprintf(f, "\n"));
}

}  // namespace envir
NAMESPACE_END

