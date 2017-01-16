//==========================================================================
//  FILEOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include "common/opp_ctype.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"
#include "fileoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(cFileOutputVectorManager);

#define VECTOR_FILE_VERSION    2

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE_APPEND;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_PRECISION;

// per-vector options
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORD_EVENTNUMBERS;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;


cFileOutputVectorManager::cFileOutputVectorManager()
{
    nextid = 0;
    initialized = false;
    f = nullptr;
    prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);
}

cFileOutputVectorManager::~cFileOutputVectorManager()
{
    closeFile();
}

void cFileOutputVectorManager::openFile()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    f = fopen(fname.c_str(), "a");
    if (f == nullptr)
        throw cRuntimeError("Cannot open output vector file '%s'", fname.c_str());
}

void cFileOutputVectorManager::closeFile()
{
    if (f) {
        fclose(f);
        f = nullptr;
    }
}

void cFileOutputVectorManager::check(int fprintfResult)
{
    if (fprintfResult < 0) {
        closeFile();
        throw cRuntimeError("Cannot write output vector file '%s'", fname.c_str());
    }
}

void cFileOutputVectorManager::writeRunData()
{
    ResultFileUtils::writeRunData(f, fname.c_str());
}

void cFileOutputVectorManager::initVector(VectorData *vp)
{
    if (!initialized) {
        initialized = true;
        openFile();
        check(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));
        writeRunData();
    }

    check(fprintf(f, "vector %d %s %s %s\n", vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()));
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it)
        check(fprintf(f, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    vp->initialized = true;
}

void cFileOutputVectorManager::startRun()
{
    // finish up previous run
    initialized = false;
    closeFile();

    // delete file left over from previous runs
    bool shouldAppend = getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND);
    if (shouldAppend)
        throw cRuntimeError("%s does not support append mode", getClassName());

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");
}

void cFileOutputVectorManager::endRun()
{
    initialized = false;
    closeFile();
}

void cFileOutputVectorManager::getOutVectorConfig(const char *modname, const char *vecname,
        bool& outEnabled, bool& outRecordEventNumbers,
        Intervals& outIntervals)
{
    std::string vectorfullpath = std::string(modname) + "." + vecname;
    outEnabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);
    outRecordEventNumbers = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        outIntervals.parse(text);
}

void *cFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    VectorData *vp = createVectorData();
    vp->id = nextid++;
    vp->initialized = false;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    getOutVectorConfig(modulename, vectorname, vp->enabled, vp->recordEventNumbers, vp->intervals);
    return vp;
}

cFileOutputVectorManager::VectorData *cFileOutputVectorManager::createVectorData()
{
    return new VectorData;
}

void cFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    delete vp;
}

void cFileOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    vp->attributes[name] = value;
}

bool cFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    static char buff[64];

    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        if (!vp->initialized)
            initVector(vp);

        ASSERT(f != nullptr);
        if (vp->recordEventNumbers)
            check(fprintf(f, "%d\t%" PRId64 "\t%s\t%.*g\n", vp->id, getSimulation()->getEventNumber(), t.str(buff), prec, value));
        else
            check(fprintf(f, "%d\t%s\t%.*g\n", vp->id, t.str(buff), prec, value));
        return true;
    }
    return false;
}

const char *cFileOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void cFileOutputVectorManager::flush()
{
    if (f)
        fflush(f);
}

}  // namespace envir
}  // namespace omnetpp

