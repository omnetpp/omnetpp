//==========================================================================
//  OMNETPPOUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORD_EVENTNUMBERS;


OmnetppOutputVectorManager::OmnetppOutputVectorManager()
{
    initialized = false;

    size_t memoryLimit = (size_t) getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    writer.setOverallMemoryLimit(memoryLimit);
}

OmnetppOutputVectorManager::~OmnetppOutputVectorManager()
{
}

void OmnetppOutputVectorManager::open()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());
}

void OmnetppOutputVectorManager::close()
{
    writer.close();
}

void OmnetppOutputVectorManager::initialize()
{
    open();
    writeRunData();
}

inline StringMap convertMap(const opp_string_map *m)
{
    StringMap result;
    if (m)
        for (auto pair : *m)
            result[pair.first.c_str()] = pair.second.c_str();
    return result;
}

void OmnetppOutputVectorManager::writeRunData()
{
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getParamAssignments());
}

void OmnetppOutputVectorManager::startRun()
{
    // clean up file from previous runs
    //initialized = false;
    //bufferedSamples = 0;
    //vectors.clear(); TODO clearing the remaining vector SHOULD be done after deleteNetwork()! because endRun may not be called at all
    //close();

    bool shouldAppend = getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND);
    if (shouldAppend)
        throw cRuntimeError("%s does not support append mode", getClassName());

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    int prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);
    writer.setPrecision(prec);
}

void OmnetppOutputVectorManager::endRun()
{
    if (writer.isOpen())
        writer.endRecordingForRun();

    initialized = false;
    vectors.clear();
    close();
}

void *OmnetppOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    //TODO assert: endRun() not yet called

    VectorData *vp = new VectorData();
    vp->handleInWriter = nullptr;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;

    std::string vectorfullpath = std::string(modulename) + "." + vectorname;
    vp->enabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
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
    //TODO assert: startRun() called, but endRun() not yet!

    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    if (!vp->enabled || !vp->intervals.contains(t))
        return false;

    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return false;

    if (vp->handleInWriter == nullptr) {
        std::string vectorFullPath = vp->moduleName.str() + "." + vp->vectorName.c_str();
        size_t bufferSize = (size_t) getEnvir()->getConfig()->getAsDouble(vectorFullPath.c_str(), CFGID_VECTOR_BUFFER);
        bool recordEventNumbers = getEnvir()->getConfig()->getAsBool(vectorFullPath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);
        vp->handleInWriter = writer.registerVector(vp->moduleName.c_str(), vp->vectorName.c_str(), convertMap(&vp->attributes), bufferSize, recordEventNumbers);
    }

    eventnumber_t eventNumber = getSimulation()->getEventNumber();
    writer.recordInVector(vp->handleInWriter, eventNumber, t.raw(), t.getScaleExp(), value);
    return true;
}

const char *OmnetppOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void OmnetppOutputVectorManager::flush()
{
    if (writer.isOpen())
        writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

