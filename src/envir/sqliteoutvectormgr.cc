//==========================================================================
//  SQLITEOUTPUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Authors: Andras Varga, Zoltan Bojthe
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
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
#include "envir/envirbase.h"

#include "sqliteoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

Register_Class(SqliteOutputVectorManager);

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE_APPEND;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUTVECTOR_MEMORY_LIMIT;

// per-vector options
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;

Register_GlobalConfigOption(CFGID_OUTPUT_VECTOR_DB_INDEXING, "output-vector-db-indexing", CFG_CUSTOM, "skip", "Whether and when to add an index to the 'vectordata' table in SQLite output vector files. Possible values: skip, ahead, after");

//TODO move out table creation code into a common file!
/*
 * Performance notes:
 *  - file size: without index, about the same as with the traditional text-based format
 *  - index adds about 30-70% to the file size
 *  - raw recording performance: about half of text based recorder
 *  - with adding the index up front, total time is worse than with adding index after
 */

SqliteOutputVectorManager::SqliteOutputVectorManager()
{
    initialized = false;

    //TODO why not read per-run?

    size_t memoryLimit = (size_t) getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    writer.setOverallMemoryLimit(memoryLimit);

    std::string indexModeStr = getEnvir()->getConfig()->getAsCustom(CFGID_OUTPUT_VECTOR_DB_INDEXING);
    if (indexModeStr == "skip")
        indexingMode = INDEX_NONE;
    else if (indexModeStr == "ahead")
        indexingMode = INDEX_AHEAD;
    else if (indexModeStr == "after")
        indexingMode = INDEX_AFTER;
    else
        throw cRuntimeError("Invalid value '%s' for '%s', expecting 'skip', 'ahead' or 'after'",
                indexModeStr.c_str(), CFGID_OUTPUT_VECTOR_DB_INDEXING->getName());
}

SqliteOutputVectorManager::~SqliteOutputVectorManager()
{
}

void SqliteOutputVectorManager::openDb()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());

    if (indexingMode == INDEX_AHEAD)
        writer.createVectorIndex();
}

void SqliteOutputVectorManager::closeDb()
{
    writer.close();
}

void SqliteOutputVectorManager::initialize()
{
    openDb();
    writeRunData();
}

inline StringMap convertMap(const opp_string_map *m) {
    StringMap result;
    if (m)
        for (auto pair : *m)
            result[pair.first.c_str()] = pair.second.c_str();
    return result;
}

void SqliteOutputVectorManager::writeRunData()
{
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), SimTime::getScaleExp(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getParamAssignments());
}

void SqliteOutputVectorManager::startRun()
{
    // clean up file from previous runs
    //initialized = false;
    //bufferedSamples = 0;
    //vectors.clear(); TODO clearing the remaining vector SHOULD be done after deleteNetwork()! because endRun may not be called at all
    //closeDb();

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old SQLite output vector file");
}

void SqliteOutputVectorManager::endRun()
{
    if (writer.isOpen()) {
        writer.endRecordingForRun();
        if (indexingMode == INDEX_AFTER) {
            time_t startTime = time(nullptr);
            writer.createVectorIndex();
            double elapsedSecs = time(nullptr) - startTime + 1; // +1 is for rounding up
            std::cout << "Indexing SQLite output vector file took about " << elapsedSecs << "s" << endl;
        }
    }

    initialized = false;
    vectors.clear();
    closeDb();
}

void *SqliteOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
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

void SqliteOutputVectorManager::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    if (writer.isOpen() && vp->handleInWriter != nullptr)
        writer.deregisterVector(vp->handleInWriter);

    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    delete vp;
}

void SqliteOutputVectorManager::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    ASSERT(vp->handleInWriter == nullptr); // otherwise it's too late
    vp->attributes[name] = value;
}

bool SqliteOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
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

    if (vp->handleInWriter == nullptr) {
        std::string vectorFullPath = vp->moduleName.str() + "." + vp->vectorName.c_str();
        size_t bufferSize = (size_t) getEnvir()->getConfig()->getAsDouble(vectorFullPath.c_str(), CFGID_VECTOR_BUFFER);
        vp->handleInWriter = writer.registerVector(vp->moduleName.c_str(), vp->vectorName.c_str(), convertMap(&vp->attributes), bufferSize);
    }

    eventnumber_t eventNumber = getSimulation()->getEventNumber();
    writer.recordInVector(vp->handleInWriter, eventNumber, t.raw(), value);
    return true;
}

const char *SqliteOutputVectorManager::getFileName() const
{
    return fname.c_str();
}

void SqliteOutputVectorManager::flush()
{
    writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

