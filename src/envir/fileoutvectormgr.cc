//==========================================================================
//  FILEOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

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
#include "timeutil.h"
#include "platmisc.h"
#include "bootenv.h"
#include "envirbase.h"
#include "csimulation.h"
#include "cmodule.h"
#include "fileoutvectormgr.h"
#include "ccomponenttype.h"
#include "stringutil.h"

USING_NAMESPACE

Register_Class(cFileOutputVectorManager);

#define VECTOR_FILE_VERSION 2
#define DEFAULT_PRECISION  "14"

Register_PerRunConfigEntry(CFGID_OUTPUT_VECTOR_FILE, "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.vec", "Name for the output vector file.");
Register_PerRunConfigEntry(CFGID_OUTPUT_VECTOR_PRECISION, "output-vector-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output vector file. The maximum value is ~15 (IEEE double precision). This setting has no effect on the \"time\" column of output vectors, which are represented as fixed-point numbers and always get recorded precisely.");

Register_PerObjectConfigEntry(CFGID_VECTOR_RECORDING, "vector-recording", CFG_BOOL, "true", "Whether data written into an output vector should be recorded.");
Register_PerObjectConfigEntry(CFGID_VECTOR_RECORD_EVENTNUMBERS, "vector-record-eventnumbers", CFG_BOOL, "true", "Whether to record event numbers for an output vector. Simulation time and value are always recorded. Event numbers are needed by the Sequence Chart Tool, for example.");
Register_PerObjectConfigEntry(CFGID_VECTOR_RECORDING_INTERVAL, "vector-recording-interval", CFG_CUSTOM, NULL, "Recording interval(s) for an output vector. Syntax: [<from>]..[<to>],... That is, both start and end of an interval are optional, and intervals are separated by comma. Example: ..100, 200..400, 900..");

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())


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
    mkPath(directoryOf(fname.c_str()).c_str());
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

void cFileOutputVectorManager::writeRunData()
{
    run.initRun();
    run.writeRunData(f, fname);
}

void cFileOutputVectorManager::initVector(sVectorData *vp)
{
    if (!f)
    {
        openFile();
        if (!f) return;
        
        CHECK(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));
    }

    if (!run.initialized)
    {
        // this is the first vector written in this run, write out run attributes
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
    dynamic_cast<EnvirBase *>(&ev)->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    // clear run data
    run.reset();
}

void cFileOutputVectorManager::endRun()
{
    closeFile();
}

void cFileOutputVectorManager::getOutVectorConfig(const char *modname,const char *vecname,
                                                  bool& outEnabled, bool& outRecordEventNumbers,
                                                  Intervals& outIntervals)
{
    std::string vectorfullpath = std::string(modname) + "." + vecname;
    outEnabled = ev.config()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);
    outRecordEventNumbers = ev.config()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);

    // get interval string
    const char *text = ev.config()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVAL);
    if (text)
        outIntervals.parse(text);
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
    ASSERT(vectorhandle!=NULL);
    sVectorData *vp = (sVectorData *)vectorhandle;
    delete vp;
}

void cFileOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle!=NULL);
    sVectorData *vp = (sVectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    static char buff[64];

    ASSERT(vectorhandle!=NULL);
    sVectorData *vp = (sVectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t))
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

const char *cFileOutputVectorManager::fileName() const
{
    return fname.c_str();
}

void cFileOutputVectorManager::flush()
{
    if (f)
        fflush(f);
}



