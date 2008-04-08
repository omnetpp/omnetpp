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

Register_PerRunConfigEntry(CFGID_EXPERIMENT_LABEL, "experiment-label", CFG_STRING, "${configname}", "Identifies the simulation experiment (which consists of several, potentially repeated measurements). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_MEASUREMENT_LABEL, "measurement-label", CFG_STRING, "${iterationvars}", "Identifies the measurement within the experiment. This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_REPLICATION_LABEL, "replication-label", CFG_STRING, "#${repetition}, seedset=@", "Identifies one replication of a measurement (see repeat= and measurement-label= as well). This string gets recorded into result files, and may be referred to during result analysis.");

extern cConfigKey *CFGID_SEED_SET;


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
        attributes["config"] = cfg->getActiveConfigName();
        attributes["run-number"] = opp_stringf("%d", cfg->getActiveRunNumber());
        const char *inifile = cfg->getFileName();
        if (inifile)
            attributes["inifile"] = inifile;

        // fill in attributes[]
        std::vector<const char *> keys = cfg->getMatchingConfigKeys("*");
        for (int i=0; i<(int)keys.size(); i++)
        {
            const char *key = keys[i];
            attributes[key] = cfg->getConfigValue(key);
        }

        std::string seedset = opp_stringf("%ld", cfg->getAsInt(CFGID_SEED_SET));
        attributes["experiment"] = cfg->getAsString(CFGID_EXPERIMENT_LABEL); //TODO if not already in there
        attributes["measurement"] = cfg->getAsString(CFGID_MEASUREMENT_LABEL);
        attributes["replication"] = opp_replacesubstring(cfg->getAsString(CFGID_REPLICATION_LABEL).c_str(), "@", seedset.c_str(), true);
        attributes["seed-set"] = seedset;

        //FIXME todo: fill in moduleParams[]
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

