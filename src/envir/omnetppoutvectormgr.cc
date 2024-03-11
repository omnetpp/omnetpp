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
#include "envirbase.h"
#include "omnetppoutvectormgr.h"
#include "resultfileutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

Register_Class(OmnetppOutputVectorManager);

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE_APPEND;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUTVECTOR_MEMORY_LIMIT;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_PRECISION;

// per-vector options
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORD_EMPTY;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORD_EVENTNUMBERS;

void OmnetppOutputVectorManager::startRun()
{
    // prevent reuse of object for multiple runs
    Assert(state == NEW);
    state = STARTED;

    // read configuration
    bool shouldAppend = getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND);
    if (shouldAppend)
        throw cRuntimeError("%s does not support append mode", getClassName());

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    int prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);
    writer.setPrecision(prec);

    size_t memoryLimit = (size_t) getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    writer.setOverallMemoryLimit(memoryLimit);

    if (!vectors.empty())
        openFileForRun();
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
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getSelectedConfigEntries());

    for (VectorData *vp : vectors) {
        std::string vectorFullPath = vp->moduleName.str() + "." + vp->vectorName.c_str();
        bool recordEmpty = getEnvir()->getConfig()->getAsBool(vectorFullPath.c_str(), CFGID_VECTOR_RECORD_EMPTY);
        if (recordEmpty)
            doRegisterVector(vp);
    }
}

void OmnetppOutputVectorManager::closeFile()
{
    writer.close();
}

void *OmnetppOutputVectorManager::registerVector(const char *modulename, const char *vectorname, opp_string_map *attributes)
{
    Assert(state == NEW || state == STARTED || state == OPENED); // note: NEW needs to be allowed for now

    VectorData *vp = new VectorData();
    vp->handleInWriter = nullptr;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    if (attributes)
        vp->attributes = *attributes;

    std::string vectorfullpath = std::string(modulename) + "." + vectorname;
    vp->enabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        vp->intervals.parse(text);

    if (state != NEW) {
        bool recordEmpty = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORD_EMPTY);
        if (recordEmpty) {
            if (state == STARTED)
                openFileForRun();
            doRegisterVector(vp);
        }
    }

    vectors.push_back(vp);
    return vp;
}

void OmnetppOutputVectorManager::doRegisterVector(VectorData *vp)
{
    ASSERT(state == OPENED && vp->handleInWriter == nullptr);
    std::string vectorFullPath = vp->moduleName.str() + "." + vp->vectorName.c_str();
    size_t bufferSize = (size_t) getEnvir()->getConfig()->getAsDouble(vectorFullPath.c_str(), CFGID_VECTOR_BUFFER);
    bool recordEventNumbers = getEnvir()->getConfig()->getAsBool(vectorFullPath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);
    vp->handleInWriter = writer.registerVector(vp->moduleName.c_str(), vp->vectorName.c_str(), ResultFileUtils::convertMap(&vp->attributes), bufferSize, recordEventNumbers);
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

    if (vp->handleInWriter == nullptr)
        doRegisterVector(vp);

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

