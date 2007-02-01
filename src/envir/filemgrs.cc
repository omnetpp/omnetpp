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
#include "timeutil.h"
#include "platmisc.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "csimulation.h"
#include "cmodule.h"
#include "cstat.h"
#include "filemgrs.h"
#include "ccomponenttype.h"
#include "stringutil.h"
#include "ivfilemgr.h"

using std::ostream;
using std::ofstream;
using std::ios;


Register_Class(cFileOutputVectorManager);

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())

// helper functions
const char *getRunSectionName(int runNumber)
{
    static int lastRunNumber = -1;
    static char section[16];

    if (runNumber != lastRunNumber)
    {
        sprintf(section, "Run %d", runNumber);
        lastRunNumber = runNumber;
    }

    return section;
}

static void createFileName(opp_string& fname, int run_no, const char *configentry, const char *defaultval)
{
    // get file name from ini file
    fname = ev.config()->getAsFilename2(getRunSectionName(run_no),"General",configentry,defaultval).c_str();
    ev.app->processFileName(fname);
}

static void removeFile(const char *fname, const char *descr)
{
    if (unlink(fname)!=0 && errno!=ENOENT)
        throw cRuntimeError("Cannot remove %s `%s': %s", descr, fname, strerror(errno));
}

cFileOutputVectorManager::cFileOutputVectorManager()
{
    nextid = 0;
    f = NULL;
    prec = ev.config()->getAsInt("General", "output-vector-precision", DEFAULT_PRECISION);
}

cFileOutputVectorManager::~cFileOutputVectorManager()
{
    closeFile();
}

void cFileOutputVectorManager::openFile()
{
    f = fopen(fname.c_str(),"a");
    if (f==NULL)
        throw cRuntimeError("Cannot open output file `%s'",fname.c_str());
}

/**
 * Writes the header of the vector file.
 * The header contains:
 * - the run id
 * - run attributes
 * - module parameters specified in the ini file
 *
 * Keys for module parameters should contain an '.' and must not contain '-',
 * otherwise they are handled as run attributes.
 */
void cFileOutputVectorManager::writeHeader()
{
    cConfiguration *config = ev.config();
    const char *section = getRunSectionName(simulation.runNumber());
    const char *runId = "run_id"; // TODO: generate id in TOmnetApp.startRun() and make it available
    fprintf(f, "run %s\n", QUOTE(runId));
    const char *inifile = config->fileName();
    if (inifile) fprintf(f, "attr inifile %s\n", QUOTE(inifile));

    int numSections = config->getNumSections();
    for (int i=0; i<numSections; ++i)
    {
        const char *sectionName = config->getSectionName(i);
        bool isRunSection = strncmp(sectionName, "Run ", 4) == 0;
        bool isCurrentRunSection = strcmp(sectionName, section) == 0;
        if (isCurrentRunSection || !isRunSection) // skip other run sections
        {
            std::vector<opp_string> entries = config->getEntriesWithPrefix(sectionName, "", "");
            std::vector<opp_string>::size_type size = entries.size();

            if (size % 2 != 0)
                fprintf(stderr, "WARNING: getEntriesWithPrefix(\"%s\", \"\", \"\") returned odd number of strings. Section will be skipped.", sectionName);

            if (size == 0 || size % 2 != 0)
                continue;

            for (std::vector<opp_string>::size_type j=0; j<size-1; j+=2)
            {
                const char *name = entries[j].c_str();
                const char *value = entries[j+1].c_str();

                if (isCurrentRunSection || !config->exists(section, name))
                {
                    bool isModuleParam = strchr(name, '.') && !strchr(name, '-');
                    if (isModuleParam)
                        fprintf(f, "param %s %s\n", name, QUOTE(value));
                    else
                        fprintf(f, "attr %s %s\n", name, QUOTE(value));
                }
            }
        }
    }
}

void cFileOutputVectorManager::closeFile()
{
    if (f)
    {
        fclose(f);
        f = NULL;
    }
}

void cFileOutputVectorManager::initVector(sVectorData *vp)
{
    if (!f)
    {
        openFile();
        if (!f) return;
        writeHeader();
    }

    CHECK(fprintf(f,"vector %d  %s  %s  %s\n",
                  vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()), vp->getColumns()));
    vp->initialised = true;
}

void cFileOutputVectorManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    createFileName(fname, simulation.runNumber(), "output-vector-file", "omnetpp.vec");
    removeFile(fname.c_str(), "old output vector file");
}

void cFileOutputVectorManager::endRun()
{
    closeFile();
}

void *cFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    sVectorData *vp = createVectorData();
    vp->id = nextid++;
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;
    ev.app->getOutVectorConfig(simulation.runNumber(), modulename, vectorname,
                               vp->enabled, vp->starttime, vp->stoptime);

    const char *runSection = getRunSectionName(simulation.runNumber());
    static char param[1024];
    sprintf(param, "%s.%s.record-event-numbers", modulename, vectorname);
    vp->recordEventNumbers = ev.config()->getAsBool2(runSection, "OutVectors", param, true);

    return vp;
}

cFileOutputVectorManager::sVectorData *cFileOutputVectorManager::createVectorData()
{
    return new sVectorData;
}

void cFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    sVectorData *vp = (sVectorData *)vectorhandle;
    delete vp;
}

bool cFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    static char buff[64];

    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
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
    prec = ev.config()->getAsInt("General", "output-scalar-precision", DEFAULT_PRECISION);
}

cFileOutputScalarManager::~cFileOutputScalarManager()
{
    closeFile();
}

void cFileOutputScalarManager::openFile()
{
    f = fopen(fname.c_str(),"a");
    if (f==NULL)
        throw cRuntimeError("Cannot open output file `%s'",fname.c_str());
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
    createFileName(fname, simulation.runNumber(), "output-scalar-file", "omnetpp.sca");
    initialized = false;
}

void cFileOutputScalarManager::endRun()
{
    closeFile();
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
        fprintf(f,"run %d  %s\n", simulation.runNumber(), QUOTE(networkname));
    }
}

void cFileOutputScalarManager::recordScalar(cModule *module, const char *name, double value)
{
    if (!initialized)
        init();

    if (!f) return;

    CHECK(fprintf(f, "scalar %s \t%s \t%.*g\n",
                     QUOTE(module->fullPath().c_str()), name ? QUOTE(name) : "(null)",
                     prec, value));
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
    createFileName(fname, simulation.runNumber(), "snapshot-file", "omnetpp.sna");
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
void dummyMethodReferencingCIndexedFileOutputVectorManager() {
  cIndexedFileOutputVectorManager m;
}



