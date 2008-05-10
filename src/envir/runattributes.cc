//==========================================================================
//  RUNATTRIBUTES.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "simkerneldefs.h"

#include <string.h>
#include "cconfigkey.h"
#include "platmisc.h"
#include "envirbase.h"
#include "csimulation.h"
#include "stringutil.h"
#include "runattributes.h"

USING_NAMESPACE


#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())


void sRunData::initRun()
{
    if (!initialized)
    {
        // Collect the attributes and module parameters of the current run
        // from the configuration.
        //
        runId = ev.getRunId();

        cConfiguration *cfg = ev.config();

        std::vector<const char *> keys1 = cfg->getPredefinedVariableNames();
        for (int i=0; i<(int)keys1.size(); i++)
            if (opp_strcmp(keys1[i], CFGVAR_RUNID)!=0) // skip runId
                attributes[keys1[i]] = cfg->getVariable(keys1[i]);

        std::vector<const char *> keys2 = cfg->getIterationVariableNames();
        for (int i=0; i<(int)keys2.size(); i++)
            attributes[keys2[i]] = cfg->getVariable(keys2[i]);

        // fill in moduleParams[]
        std::vector<const char *> params = cfg->getParameterKeyValuePairs();
        for (int i=0; i<params.size(); i+=2)
            moduleParams[params[i]] = params[i+1];

        initialized = true;
    }
}

void sRunData::reset()
{
    initialized = false;
    attributes.clear();
    moduleParams.clear();
}


void sRunData::writeRunData(FILE *f, opp_string fname)
{
    CHECK(fprintf(f, "run %s\n", QUOTE(runId.c_str())));
    for (opp_string_map::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
    {
        CHECK(fprintf(f, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    for (opp_string_map::const_iterator it = moduleParams.begin(); it != moduleParams.end(); ++it)
    {
        CHECK(fprintf(f, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    CHECK(fprintf(f, "\n"));
}

