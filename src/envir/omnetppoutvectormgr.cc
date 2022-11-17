//==========================================================================
//  OMNETPPOUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2016 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/platdep/platmisc.h"
#include "resultfileutils.h"
#include "omnetppoutvectormgr.h"

#include "genericenvir.h"
#include "resultfileutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

Register_Class(OmnetppOutputVectorManager);

#define DEFAULT_OUTPUT_VECTOR_PRECISION    "14"
#define DEFAULT_OUTPUT_VECTOR_MEMORY_LIMIT "16MiB"
#define DEFAULT_VECTOR_BUFFER              "1MiB"

// global options
Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_FILE, "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.vec", "Name for the output vector file.");
Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_FILE_APPEND, "output-vector-file-append", CFG_BOOL, "false", "What to do when the output vector file already exists: append to it, or delete it and begin a new file (default). Note: `cIndexedFileOutputVectorManager` currently does not support appending.");
Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_PRECISION, "output-vector-precision", CFG_INT, DEFAULT_OUTPUT_VECTOR_PRECISION, "The number of significant digits for recording data into the output vector file. The maximum value is ~15 (IEEE double precision). This setting has no effect on SQLite recording (it stores values as 8-byte IEEE floating point numbers), and for the \"time\" column which is represented as fixed-point numbers and always get recorded precisely.");
Register_GlobalConfigOptionU(CFGID_OUTPUTVECTOR_MEMORY_LIMIT, "output-vectors-memory-limit", "B", DEFAULT_OUTPUT_VECTOR_MEMORY_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later. There is also a per-vector limit, see `**.vector-buffer`.");

// per-vector options
Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING, "vector-recording", KIND_VECTOR, CFG_BOOL, "true", "Whether data written into an output vector should be recorded.\nUsage: `<module-full-path>.<vector-name>.vector-recording=true/false`. To control vector recording from a `@statistic`, use `<statistic-name>:vector for <vector-name>`. Example: `**.ping.roundTripTime:vector.vector-recording=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORD_EVENTNUMBERS, "vector-record-eventnumbers", KIND_VECTOR, CFG_BOOL, "true", "Whether to record event numbers for an output vector. (Values and timestamps are always recorded.) Event numbers are needed by the Sequence Chart Tool, for example.\nUsage: `<module-full-path>.<vector-name>.vector-record-eventnumbers=true/false`.\nExample: `**.ping.roundTripTime:vector.vector-record-eventnumbers=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING_INTERVALS, "vector-recording-intervals", KIND_VECTOR, CFG_CUSTOM, nullptr, "Allows one to restrict recording of an output vector to one or more simulation time intervals. Usage: `<module-full-path>.<vector-name>.vector-recording-intervals=<intervals>`. The syntax for `<intervals>` is: `[<from>]..[<to>],...` That is, both start and end of an interval are optional, and intervals are separated by comma.\nExample: `**.roundTripTime:vector.vector-recording-intervals=..100, 200..400, 900..`");
Register_PerObjectConfigOptionU(CFGID_VECTOR_BUFFER, "vector-buffer", KIND_VECTOR, "B", DEFAULT_VECTOR_BUFFER, "For output vectors: the maximum per-vector buffer space used for storing values before writing them out as a block into the output vector file. There is also a total limit, see `output-vectors-memory-limit`.\nUsage: `<module-full-path>.<vector-name>.vector-buffer=<amount>`.");

void OmnetppOutputVectorManager::configure(cSimulation *simulation, cConfiguration *cfg)
{
    this->cfg = cfg;
    ResultFileUtils::setConfiguration(cfg);
    simulation->addLifecycleListener(this);

    fname = cfg->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    fname = augmentFileName(fname);

    shouldAppend = cfg->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND);

    int prec = cfg->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);
    writer.setPrecision(prec);

    size_t memoryLimit = (size_t) cfg->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    writer.setOverallMemoryLimit(memoryLimit);
}

void OmnetppOutputVectorManager::startRun()
{
    // prevent reuse of object for multiple runs
    Assert(state == NEW);
    state = STARTED;

    // read configuration
    if (shouldAppend)
        throw cRuntimeError("%s does not support append mode", getClassName());

    removeFile(fname.c_str(), "old output vector file");

}

void OmnetppOutputVectorManager::endRun()
{
    Assert(state == NEW || state == STARTED || state == OPENED);
    state = ENDED;
    if (writer.isOpen()) {
        writer.endRecordingForRun();
        closeFile();
        vectors.clear();
    }
}

void OmnetppOutputVectorManager::openFileForRun()
{
    // ensure startRun() has been invoked
    Assert(state == STARTED);
    state = OPENED;

    // open file
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());

    // write run data
    writer.beginRecordingForRun(getRunId().c_str(), getRunAttributes(), getIterationVariables(), getSelectedConfigEntries());
}

void OmnetppOutputVectorManager::closeFile()
{
    writer.close();
}

void *OmnetppOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    Assert(state == NEW || state == STARTED || state == OPENED); // note: NEW needs to be allowed for now

    VectorData *vp = new VectorData();
    vp->handleInWriter = nullptr;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;

    std::string vectorfullpath = std::string(modulename) + "." + vectorname;
    vp->enabled = cfg->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = cfg->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        vp->intervals.parse(text);

    vectors.push_back(vp);
    return vp;
}

void OmnetppOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    if (writer.isOpen() && vp->handleInWriter != nullptr)
        writer.deregisterVector(vp->handleInWriter);

    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    delete vp;
}

void OmnetppOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    ASSERT(vp->handleInWriter == nullptr); // otherwise it's too late
    vp->attributes[name] = value;
}

bool OmnetppOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    if (state == ENDED)
        return false;    // ignore writes during network teardown

    Assert(state == STARTED || state == OPENED);

    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    if (!vp->enabled || !vp->intervals.contains(t))
        return false;

    if (state != OPENED)
        openFileForRun();

    if (isBad())
        return false;

    if (vp->handleInWriter == nullptr) {
        std::string vectorFullPath = vp->moduleName.str() + "." + vp->vectorName.c_str();
        size_t bufferSize = (size_t) cfg->getAsDouble(vectorFullPath.c_str(), CFGID_VECTOR_BUFFER);
        bool recordEventNumbers = cfg->getAsBool(vectorFullPath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);
        vp->handleInWriter = writer.registerVector(vp->moduleName.c_str(), vp->vectorName.c_str(), convertMap(&vp->attributes), bufferSize, recordEventNumbers);
    }

    eventnumber_t eventNumber = getSimulation()->getEventNumber();
    writer.recordInVector(vp->handleInWriter, eventNumber, t.raw(), t.getScaleExp(), value);
    return true;
}

void OmnetppOutputVectorManager::flush()
{
    if (writer.isOpen())
        writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

