//==========================================================================
//  FILEOUTPUTSCALARMGR.CC - part of
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

#include <assert.h>
#include <string.h>
#include <fstream>
#include "opp_ctype.h"
#include "cconfigkey.h"
#include "fileutil.h"
#include "cenvirimpl.h"
#include "omnetapp.h"
#include "csimulation.h"
#include "cmodule.h"
#include "cstatistic.h"
#include "cdensityestbase.h"
#include "fileoutscalarmgr.h"
#include "ccomponenttype.h"
#include "stringutil.h"

USING_NAMESPACE

#define DEFAULT_PRECISION  "14"

Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE, "output-scalar-file", CFG_FILENAME, "${configname}-${runnumber}.sca", "Name for the output scalar file.");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_PRECISION, "output-scalar-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output scalar file. The maximum value is ~15 (IEEE double precision).");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE_APPEND, "output-scalar-file-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it (OMNeT++ 3.x behavior), or delete it and begin a new file (default).");

Register_PerObjectConfigEntry(CFGID_RECORD_SCALAR, "record-scalar", CFG_BOOL, "true", "Whether the matching output scalars should be recorded. Syntax: <module-full-path>.<scalar-name>.record-scalar=true/false. Example: **.queue.packetsDropped.record-scalar=true");

extern cConfigKey *CFGID_EXPERIMENT_LABEL;
extern cConfigKey *CFGID_MEASUREMENT_LABEL;
extern cConfigKey *CFGID_REPLICATION_LABEL;
extern cConfigKey *CFGID_SEED_SET;

Register_Class(cFileOutputScalarManager);

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output scalar file `%s'", fname.c_str())


cFileOutputScalarManager::cFileOutputScalarManager()
{
    f = NULL;
    prec = ev.config()->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
}

cFileOutputScalarManager::~cFileOutputScalarManager()
{
    closeFile();
}

void cFileOutputScalarManager::openFile()
{
    f = fopen(fname.c_str(),"a");
    if (f==NULL)
        throw cRuntimeError("Cannot open output scalar file `%s'", fname.c_str());
}

void cFileOutputScalarManager::closeFile()
{
    if (f)
    {
        fclose(f);
        f = NULL;
    }
}

void cFileOutputScalarManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    fname = ev.config()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE).c_str();
    ((cEnvirImpl&)ev).app->processFileName(fname);
    if (ev.config()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND)==false)
        removeFile(fname.c_str(), "old output scalar file");
    initialized = false;
}

void cFileOutputScalarManager::endRun()
{
    closeFile();
}

inline bool isNumeric(const char *s)
{
    char *e;
    strtod(s, &e);
    return *e=='\0';
}

void cFileOutputScalarManager::init()
{
    if (!f)
    {
        openFile();
        if (!f) return;
    }

    if (!initialized)
    {
        initialized = true;
        const char *networkname = simulation.networkType()->name();
        const char *runId = ((cEnvirImpl&)ev).app->getRunId();
        fprintf(f, "run %s\n", QUOTE(runId));
        //FIXME write out run data here as well (not only in outvectormanager)

        // save iteration variables

        std::vector<const char *> v = ev.config()->getIterationVariableNames();
        for (int i=0; i<(int)v.size(); i++)
        {
            const char *name = v[i];
            const char *value = ev.config()->getVariable(v[i]);
            //XXX write with using an "itervar" keyword not "scalar"
            if (isNumeric(value))
                CHECK(fprintf(f, "scalar . \t%s \t%s\n", name, value));
        }
    }
}

void cFileOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!initialized)
        init();
    if (!f)
        return;

    if (!name || !name[0])
        name = "(unnamed)";

    bool enabled = ev.config()->getAsBool((component->fullPath()+"."+name).c_str(), CFGID_RECORD_SCALAR);
    if (enabled)
    {
        CHECK(fprintf(f, "scalar %s \t%s \t%.*g\n", QUOTE(component->fullPath().c_str()), QUOTE(name), prec, value));
        if (attributes)
            for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
                CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
    }
}

void cFileOutputScalarManager::recordScalar(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    if (!initialized)
        init();
    if (!f)
        return;

    if (!name)
        name = statistic->fullName();
    if (!name || !name[0])
        name = "(unnamed)";

    // check that recording this statistic is not disabled as a whole
    std::string objectFullPath = component->fullPath() + "." + name;
    bool enabled = ev.config()->getAsBool(objectFullPath.c_str(), CFGID_RECORD_SCALAR);
    if (!enabled)
        return;

    // record members; note that they may get disabled individually
    std::string n = name;
    recordScalar(component, (n+":count").c_str(), statistic->count());
    if (statistic->count() != statistic->weights())  //FIXME use some isWeighted() instead?
        recordScalar(component, (n+":weights").c_str(), statistic->weights());
    recordScalar(component, (n+":mean").c_str(), statistic->mean());
    recordScalar(component, (n+":stddev").c_str(), statistic->stddev());
    recordScalar(component, (n+":sum").c_str(), statistic->sum());
    recordScalar(component, (n+":sqrsum").c_str(), statistic->sqrSum());
    recordScalar(component, (n+":min").c_str(), statistic->min());
    recordScalar(component, (n+":max").c_str(), statistic->max());

    //FIXME issue: what if all members get disabled, but there are attributes???
    // what about this file format:
    //   statistic <modulepath> <statisticname>
    //   field count 343
    //   field weights 343
    //   field mean    2.1233
    //   field stddev  1.345
    //   attr unit s
    //   bin 0  3
    //   bin 10 13
    //   bin 20 19
    //   ...
    // In Scave, it would still be possible to read fields as specially,
    // as "scalars", with their own Ids.
    //
    if (attributes)
        for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
            CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    if (dynamic_cast<cDensityEstBase *>(statistic))
    {
        // check that recording the histogram is enabled
        bool enabled = ev.config()->getAsBool((objectFullPath+":histogram").c_str(), CFGID_RECORD_SCALAR);
        if (enabled)
        {
            cDensityEstBase *hist = (cDensityEstBase *)statistic;
            CHECK(fprintf(f, "histogram %s \t%s\n", QUOTE(component->fullPath().c_str()), QUOTE(name)));

            int n = hist->cells();
            if (n>0)
            {
                CHECK(fprintf(f, "bin\t-INF\t%lu\n", hist->underflowCell()));
                for (int i=0; i<n; i++)
                    CHECK(fprintf(f, "bin\t%.*g\t%.*g\n", prec, hist->basepoint(i), prec, hist->cell(i)));
                CHECK(fprintf(f, "bin\t%.*g\t%lu\n", prec, hist->basepoint(n), hist->overflowCell()));
            }
        }
    }
}

const char *cFileOutputScalarManager::fileName() const
{
    return fname.c_str();
}

void cFileOutputScalarManager::flush()
{
    if (f)
        fflush(f);
}

