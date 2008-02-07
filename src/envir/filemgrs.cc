//==========================================================================
//  FILEMGRS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     cFileOutputVectorManager
//     cFileOutputScalarManager
//     cFileSnapshotManager
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
#include <errno.h> // SGI
#include "opp_ctype.h"
#include "cconfigkey.h"
#include "timeutil.h"
#include "platmisc.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "csimulation.h"
#include "cmodule.h"
#include "cstat.h"
#include "cdensity.h"
#include "filemgrs.h"
#include "ccomponenttype.h"
#include "stringutil.h"
#include "ivfilemgr.h"
#include "stringtokenizer.h"

USING_NAMESPACE

using std::ostream;
using std::ofstream;
using std::ios;

//XXX split up this file by classes -- it's too big

Register_Class(cFileOutputVectorManager);

#define DEFAULT_PRECISION  "14"

Register_PerRunConfigEntry(CFGID_EXPERIMENT_LABEL, "experiment-label", CFG_STRING, "${configname}", "Identifies the simulation experiment (which consists of several, potentially repeated measurements). This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_MEASUREMENT_LABEL, "measurement-label", CFG_STRING, "${iterationvars}", "Identifies the measurement within the experiment. This string gets recorded into result files, and may be referred to during result analysis.");
Register_PerRunConfigEntry(CFGID_REPLICATION_LABEL, "replication-label", CFG_STRING, "#${repetition}, seedset=@", "Identifies one replication of a measurement (see repeat= and measurement-label= as well). This string gets recorded into result files, and may be referred to during result analysis.");

Register_PerRunConfigEntry(CFGID_OUTPUT_VECTOR_FILE, "output-vector-file", CFG_FILENAME, "${configname}-${runnumber}.vec", "Name for the output vector file.");
Register_PerRunConfigEntry(CFGID_OUTPUT_VECTOR_PRECISION, "output-vector-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output vector file. The maximum value is ~15 (IEEE double precision). This setting has no effect on the \"time\" column of output vectors, which are represented as fixed-point numbers and always get recorded precisely.");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE, "output-scalar-file", CFG_FILENAME, "${configname}-${runnumber}.sca", "Name for the output scalar file.");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_PRECISION, "output-scalar-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output scalar file. The maximum value is ~15 (IEEE double precision).");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE_APPEND, "output-scalar-file-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it (OMNeT++ 3.x behavior), or delete it and begin a new file (default).");
Register_PerRunConfigEntry(CFGID_SNAPSHOT_FILE, "snapshot-file", CFG_FILENAME, "${configname}-${runnumber}.sna", "Name of the snapshot file.");

Register_PerObjectConfigEntry(CFGID_OUTVECTOR_ENABLED, "enable-recording", CFG_BOOL, "true", "Whether data written into an output vector should be recorded.");
Register_PerObjectConfigEntry(CFGID_OUTVECTOR_EVENT_NUMBERS, "record-event-numbers", CFG_BOOL, "true", "Whether to record event numbers for an output vector. Simulation time and value are always recorded. Event numbers are needed by the Sequence Chart Tool, for example.");
Register_PerObjectConfigEntry(CFGID_OUTVECTOR_INTERVAL, "recording-interval", CFG_CUSTOM, NULL, "Recording interval(s) for an output vector. Syntax: [<from>]..[<to>],... That is, both start and end of an interval are optional, and intervals are separated by comma. Example: ..100, 200..400, 900..");

extern cConfigKey *CFGID_SEED_SET;


#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())


static void removeFile(const char *fname, const char *descr)
{
    if (unlink(fname)!=0 && errno!=ENOENT)
        throw cRuntimeError("Cannot remove %s `%s': %s", descr, fname, strerror(errno));
}

cFileOutputVectorManager::cFileOutputVectorManager()
{
    nextid = 0;
    f = NULL;
    prec = ev.config()->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);
}

cFileOutputVectorManager::~cFileOutputVectorManager()
{
    closeFile();
}

void cFileOutputVectorManager::openFile()
{
    f = fopen(fname.c_str(),"a");
    if (f==NULL)
        throw cRuntimeError("Cannot open output vector file `%s'",fname.c_str());
}

void cFileOutputVectorManager::closeFile()
{
    if (f)
    {
        fclose(f);
        f = NULL;
    }
}

void cFileOutputVectorManager::initRun()
{
    if (!run.initialized)
    {
        // Collect the attributes and module parameters of the current run
        // from the configuration.
        //
        run.runId = ev.app->getRunId();
        cConfiguration *cfg = ev.config();
        run.attributes["config"] = cfg->getActiveConfigName();
        run.attributes["run-number"] = opp_stringf("%d", cfg->getActiveRunNumber());
        const char *inifile = cfg->getFileName();
        if (inifile)
            run.attributes["inifile"] = inifile;

        // fill in run.attributes[]
        std::vector<const char *> keys = cfg->getMatchingConfigKeys("*");
        for (int i=0; i<(int)keys.size(); i++)
        {
            const char *key = keys[i];
            run.attributes[key] = cfg->getConfigValue(key);
        }

        std::string seedset = opp_stringf("%ld", cfg->getAsInt(CFGID_SEED_SET));
        run.attributes["experiment"] = cfg->getAsString(CFGID_EXPERIMENT_LABEL); //TODO if not already in there
        run.attributes["measurement"] = cfg->getAsString(CFGID_MEASUREMENT_LABEL);
        run.attributes["replication"] = opp_replacesubstring(cfg->getAsString(CFGID_REPLICATION_LABEL).c_str(), "@", seedset.c_str(), true);
        run.attributes["seed-set"] = seedset;

        //FIXME todo: fill in run.moduleParams[]
        run.initialized = true;
    }
}

void cFileOutputVectorManager::writeRunData()
{
    CHECK(fprintf(f, "run %s\n", QUOTE(run.runId.c_str())));
    for (opp_string_map::const_iterator it = run.attributes.begin(); it != run.attributes.end(); ++it)
    {
        CHECK(fprintf(f, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    for (opp_string_map::const_iterator it = run.moduleParams.begin(); it != run.moduleParams.end(); ++it)
    {
        CHECK(fprintf(f, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    }
    CHECK(fprintf(f, "\n"));
}

void cFileOutputVectorManager::initVector(sVectorData *vp)
{
    if (!f)
    {
        openFile();
        if (!f) return;
    }

    if (!run.initialized)
    {
        // this is the first vector written in this run, write out run attributes
        initRun();
        writeRunData();
    }

    CHECK(fprintf(f,"vector %d  %s  %s  %s\n",
                  vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()), vp->getColumns()));
    for (opp_string_map::iterator it=vp->attributes.begin(); it!=vp->attributes.end(); it++)
        CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    vp->initialized = true;
}

void cFileOutputVectorManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    fname = ev.config()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    ev.app->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    // clear run data
    run.initialized = false;
    run.attributes.clear();
    run.moduleParams.clear();
}

void cFileOutputVectorManager::endRun()
{
    closeFile();
}

void cFileOutputVectorManager::getOutVectorConfig(const char *modname,const char *vecname,
                                                  bool& outEnabled, bool& outRecordEventNumbers,
                                                  Interval *&outIntervals)
{
    std::string vectorfullpath = std::string(modname) + "." + vecname;
    outEnabled = ev.config()->getAsBool(vectorfullpath.c_str(), CFGID_OUTVECTOR_ENABLED);
    outRecordEventNumbers = ev.config()->getAsBool(vectorfullpath.c_str(), CFGID_OUTVECTOR_EVENT_NUMBERS);

    // get interval string
    outIntervals = NULL;
    const char *text = ev.config()->getAsCustom(vectorfullpath.c_str(), CFGID_OUTVECTOR_INTERVAL);
    if (text)
    {
        // parse the string, syntax is "start..end, start..end, start.."
        std::vector<Interval> intervals;
        StringTokenizer tokenizer(text, ",");
        while (tokenizer.hasMoreTokens())
        {
            // parse interval string
            const char *s = tokenizer.nextToken();
            const char *ellipsis = strstr(s, "..");
            if (!ellipsis)
                throw cRuntimeError("Wrong syntax in output vector interval %s=%s", text, s);

            const char *startstr = s;
            const char *stopstr = ellipsis+2;
            while (opp_isspace(*startstr)) startstr++;
            while (opp_isspace(*stopstr)) stopstr++;

            // add to vector
            Interval interval;
            if (startstr!=ellipsis)
                interval.startTime = STR_SIMTIME(std::string(startstr, ellipsis-startstr).c_str());
            if (*stopstr)
                interval.stopTime = STR_SIMTIME(stopstr);
            intervals.push_back(interval);
        }

        // return as plain C++ array
        outIntervals = new Interval[intervals.size()+1]; // +1: terminating (0,0)
        for (int i=0; i<(int)intervals.size(); i++)
            outIntervals[i] = intervals[i];
    }
}

void *cFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    sVectorData *vp = createVectorData();
    vp->id = nextid++;
    vp->initialized = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;
    getOutVectorConfig(modulename, vectorname, vp->enabled, vp->recordEventNumbers, vp->intervals);
    return vp;
}

cFileOutputVectorManager::sVectorData *cFileOutputVectorManager::createVectorData()
{
    return new sVectorData;
}

void cFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    sVectorData *vp = (sVectorData *)vectorhandle;
    delete [] vp->intervals;
    delete vp;
}

void cFileOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    sVectorData *vp = (sVectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    static char buff[64];

    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (!vp->intervals || containsTime(t, vp->intervals))
    {
        if (!vp->initialized)
            initVector(vp);

        assert(f!=NULL);
        if (vp->recordEventNumbers)
        {
            CHECK(fprintf(f,"%d\t%ld\t%s\t%.*g\n", vp->id, simulation.eventNumber(), SIMTIME_TTOA(buff, t), prec, value));
        }
        else
        {
            CHECK(fprintf(f,"%d\t%s\t%.*g\n", vp->id, SIMTIME_TTOA(buff, t), prec, value));
        }
        return true;
    }
    return false;
}

bool cFileOutputVectorManager::containsTime(simtime_t t, Interval *intervals)
{
    for (Interval *i = intervals; i->startTime!=0 || i->stopTime!=0; i++)
        if (i->startTime <= t && (i->stopTime == 0 || t <= i->stopTime))
            return true;
    return false;
}

const char *cFileOutputVectorManager::fileName() const
{
    return fname.c_str();
}

void cFileOutputVectorManager::flush()
{
    if (f)
        fflush(f);
}


//=================================================================

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
    ev.app->processFileName(fname);
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
        const char *runId = ev.app->getRunId();
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
    CHECK(fprintf(f, "scalar %s \t%s \t%.*g\n", QUOTE(component->fullPath().c_str()), QUOTE(name), prec, value));
    if (attributes)
        for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
            CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
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
    std::string n = name;
    recordScalar(component, (n+".samples").c_str(), statistic->samples());
    recordScalar(component, (n+".mean").c_str(), statistic->mean());
    recordScalar(component, (n+".stddev").c_str(), statistic->stddev());
    recordScalar(component, (n+".min").c_str(), statistic->min());
    recordScalar(component, (n+".max").c_str(), statistic->max());

    if (attributes)
        for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
            CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    if (dynamic_cast<cDensityEstBase *>(statistic))
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

const char *cFileOutputScalarManager::fileName() const
{
    return fname.c_str();
}

void cFileOutputScalarManager::flush()
{
    if (f)
        fflush(f);
}

//=================================================================

Register_Class(cFileSnapshotManager);

cFileSnapshotManager::cFileSnapshotManager()
{
}

cFileSnapshotManager::~cFileSnapshotManager()
{
}

void cFileSnapshotManager::startRun()
{
    // clean up file from previous runs
    fname = ev.config()->getAsFilename(CFGID_SNAPSHOT_FILE).c_str();
    ev.app->processFileName(fname);
    removeFile(fname.c_str(), "old snapshot file");
}

void cFileSnapshotManager::endRun()
{
}

ostream *cFileSnapshotManager::getStreamForSnapshot()
{
    ostream *os = new ofstream(fname.c_str(), ios::out|ios::app);
    return os;
}

void cFileSnapshotManager::releaseStreamForSnapshot(std::ostream *os)
{
    delete os;
}

const char *cFileSnapshotManager::fileName() const
{
    return fname.c_str();
}

// create some reference to cIndexedFileOutputVectorManager
// otherwise the MS linker omits ivfilemgr.obj
void dummyMethodReferencingCIndexedFileOutputVectorManager()
{
    cIndexedFileOutputVectorManager m;
}



