//==========================================================================
//   FILEMGRS.CC - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     cFileOutputVectorManager
//     cFileOutputScalarManager
//     cFileSnapshotManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "defs.h"

#include <assert.h>
#include <string.h>

#ifdef USE_STD_NAMESPACE
#include <fstream>
using std::ofstream;
using std::ios;
#else
#include <fstream.h>   // ofstream
#endif

#include "cenvir.h"
#include "cinifile.h"
#include "omnetapp.h"
#include "csimul.h"
#include "cmodule.h"
#include "cstat.h"
#include "macros.h"
#include "filemgrs.h"


Register_Class(cFileOutputVectorManager);

#define CHECK(fprintf)    if (fprintf<0) opp_error(eOUTVECT)

// helper function
static void createFileName(opp_string& fname, cIniFile *inifile, int run_no, const char *configentry, const char *defaultval)
{
    // get file name from ini file
    char section[16];
    sprintf(section,"Run %d",run_no);
    const char *basefname = inifile->getAsString2(section,"General",configentry,defaultval);

    // append host name if necessary
    if (!inifile->getAsBool2(section,"General","fname-append-host",false))
    {
        fname = basefname;
    }
    else
    {
        const char *hostname=getenv("HOST");
        if (!hostname)
        {
            opp_error("Cannot append hostname to file name `%s': no HOST environment variable", basefname);
        }

        // add ".<hostname>" to fname
        fname.allocate(strlen(basefname)+1+strlen(hostname)+1);
        sprintf(fname.buffer(),"%s.%s", (const char *)fname, hostname);
    }
}



cFileOutputVectorManager::cFileOutputVectorManager()
{
    nextid = 0;
    f = NULL;
}

cFileOutputVectorManager::~cFileOutputVectorManager()
{
    closeFile();
}

void cFileOutputVectorManager::openFile()
{
    f = fopen(fname,"a");
    if (f==NULL)
        opp_error("Cannot open output file `%s'",(const char *)fname);
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
    }

    CHECK(fprintf(f,"vector %ld  \"%s\"  \"%s\"  %d\n",
                  vp->id, (const char *)vp->modulename, (const char *) vp->vectorname? (const char*) vp->vectorname : "(null)", vp->tuple));
    vp->initialised = true;
}

void cFileOutputVectorManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    createFileName(fname, ev.app->getIniFile(), simulation.runNumber(), "output-vector-file", "omnetpp.vec");
    remove(fname);
}

void cFileOutputVectorManager::endRun()
{
    closeFile();
}

void *cFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname, int tuple)
{
    sVectorData *vp = createVectorData();
    vp->tuple = tuple;
    vp->id = nextid++;
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;

    ev.app->getOutVectorConfig(simulation.runNumber(), modulename, vectorname,
                               vp->enabled, vp->starttime, vp->stoptime);
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
    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
            initVector(vp);
        assert(f!=NULL);
        CHECK(fprintf(f,"%ld\t%.9g\t%.9g\n",vp->id, t, value));
        return true;
    }
    return false;
}

bool cFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
            initVector(vp);
        assert(f!=NULL);
        CHECK(fprintf(f,"%ld\t%.9g\t%.9g\t%.9g\n",vp->id, t, value1, value2));
        return true;
    }
    return false;
}

const char *cFileOutputVectorManager::fileName() const
{
    return (const char *)fname;
}


//=================================================================

Register_Class(cFileOutputScalarManager);

#define CHECK(fprintf)    if (fprintf<0) opp_error(eOUTVECT)

cFileOutputScalarManager::cFileOutputScalarManager()
{
    f = NULL;
}

cFileOutputScalarManager::~cFileOutputScalarManager()
{
    closeFile();
}

void cFileOutputScalarManager::openFile()
{
    f = fopen(fname,"a");
    if (f==NULL)
        opp_error("Cannot open output file `%s'",(const char *)fname);
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
    createFileName(fname, ev.app->getIniFile(), simulation.runNumber(), "output-scalar-file", "omnetpp.sca");
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
        fprintf(f,"run %d \"%s\"\n", simulation.runNumber(), simulation.networkType()->name());
    }
}

void cFileOutputScalarManager::recordScalar(cModule *module, const char *name, double value)
{
    if (!initialized)
        init();

    if (!f) return;

    fprintf(f,"scalar \"%s\" \t\"%s\" \t%.9g\n", module->fullPath(), name? name : "(null)", value);
}

void cFileOutputScalarManager::recordScalar(cModule *module, const char *name, const char *text)
{
    if (!initialized)
        init();

    if (!f) return;

    fprintf(f,"scalar \"%s\" \t\"%s\" \t\"%s\"\n", module->fullPath(), name? name : "(null)", text);
}

void cFileOutputScalarManager::recordScalar(cModule *module, const char *name, cStatistic *stats)
{
    if (!initialized)
        init();

    if (!f) return;

    fprintf(f,"statistics \"%s\" \t\"%s\" \t\"%s\"\n", module->fullPath(), name? name : "(null)", stats->className());
    stats->saveToFile( f );
}

const char *cFileOutputScalarManager::fileName() const
{
    return (const char *)fname;
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
    createFileName(fname, ev.app->getIniFile(), simulation.runNumber(), "snapshot-file", "omnetpp.sna");
    remove(fname);
}

void cFileSnapshotManager::endRun()
{
}

ostream *cFileSnapshotManager::getStreamForSnapshot()
{
    ostream *os = new ofstream( fname, ios::out|ios::app);
    return os;
}

void cFileSnapshotManager::releaseStreamForSnapshot(ostream *os)
{
    delete os;
}

const char *cFileSnapshotManager::fileName() const
{
    return (const char *)fname;
}

