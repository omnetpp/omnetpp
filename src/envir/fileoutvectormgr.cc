//==========================================================================
//  FILEOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
#include "omnetpp/platdep/timeutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"
#include "fileoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(cFileOutputVectorManager);

#define VECTOR_FILE_VERSION    2
#define DEFAULT_PRECISION      "14"

#define LL  INT64_PRINTF_FORMAT

Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_FILE, "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.vec", "Name for the output vector file.");
Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_PRECISION, "output-vector-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output vector file. The maximum value is ~15 (IEEE double precision). This setting has no effect on the \"time\" column of output vectors, which are represented as fixed-point numbers and always get recorded precisely.");

Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING, "vector-recording", KIND_VECTOR, CFG_BOOL, "true", "Whether data written into an output vector should be recorded.\nUsage: `<module-full-path>.<vector-name>.vector-recording=true/false`. To control vector recording from a `@statistic`, use `<statistic-name>:vector for <vector-name>`. Example: `**.ping.roundTripTime:vector.vector-recording=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORD_EVENTNUMBERS, "vector-record-eventnumbers", KIND_VECTOR, CFG_BOOL, "true", "Whether to record event numbers for an output vector. (Values and timestamps are always recorded.) Event numbers are needed by the Sequence Chart Tool, for example.\nUsage: `<module-full-path>.<vector-name>.vector-record-eventnumbers=true/false`.\nExample: `**.ping.roundTripTime:vector.vector-record-eventnumbers=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING_INTERVALS, "vector-recording-intervals", KIND_VECTOR, CFG_CUSTOM, nullptr, "Allows one to restrict recording of an output vector to one or more simulation time intervals. Usage: `<module-full-path>.<vector-name>.vector-recording-intervals=<intervals>`. The syntax for `<intervals>` is: `[<from>]..[<to>],...` That is, both start and end of an interval are optional, and intervals are separated by comma.\nExample: `**.roundTripTime:vector.vector-recording-intervals=..100, 200..400, 900..`");

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output vector file `%s'", fname.c_str())


cFileOutputVectorManager::cFileOutputVectorManager()
{
    nextid = 0;
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
        throw cRuntimeError("Cannot open output vector file `%s'", fname.c_str());
}

void cFileOutputVectorManager::closeFile()
{
    if (f) {
        fclose(f);
        f = nullptr;
    }
}

void cFileOutputVectorManager::writeRunData()
{
    run.initRun();
    run.writeRunData(f, fname);
}

void cFileOutputVectorManager::initVector(VectorData *vp)
{
    if (!f) {
        openFile();
        if (!f)
            return;

        CHECK(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));
    }

    if (!run.initialized) {
        // this is the first vector written in this run, write out run attributes
        writeRunData();
    }

    CHECK(fprintf(f, "vector %d  %s  %s  %s\n",
                    vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()));
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it)
        CHECK(fprintf(f, "attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    vp->initialized = true;
}

void cFileOutputVectorManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    // clear run data
    run.reset();
}

void cFileOutputVectorManager::endRun()
{
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

        assert(f != nullptr);
        if (vp->recordEventNumbers) {
            CHECK(fprintf(f, "%d\t%" LL "d\t%s\t%.*g\n", vp->id, getSimulation()->getEventNumber(), t.str(buff), prec, value));
        }
        else {
            CHECK(fprintf(f, "%d\t%s\t%.*g\n", vp->id, t.str(buff), prec, value));
        }
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

