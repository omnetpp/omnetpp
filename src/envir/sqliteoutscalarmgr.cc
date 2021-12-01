//==========================================================================
//  SQLITEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cassert>
#include <cstring>
#include <fstream>
#include <clocale>
#include "common/opp_ctype.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/unitconversion.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/cabstracthistogram.h"
#include "omnetpp/ccomponenttype.h"
#include "envir/envirbase.h"
#include "sqliteoutscalarmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

#define DEFAULT_COMMIT_FREQ      "100000"

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE_APPEND;

Register_GlobalConfigOption(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ, "output-scalar-db-commit-freq", CFG_INT, DEFAULT_COMMIT_FREQ, "Used with SqliteOutputScalarManager: COMMIT every n INSERTs.");

// per-scalar options
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;
extern omnetpp::cConfigOption *CFGID_BIN_RECORDING;
extern omnetpp::cConfigOption *CFGID_PARAM_RECORDING;

Register_Class(SqliteOutputScalarManager);

void SqliteOutputScalarManager::startRun()
{
    // prevent reuse of object for multiple runs
    Assert(state == NEW);
    state = STARTED;

    // clean up file from previous runs
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old SQLite output scalar file");
}

void SqliteOutputScalarManager::endRun()
{
    Assert(state == NEW || state == STARTED || state == OPENED);
    state = ENDED;

    closeFile();
}

void SqliteOutputScalarManager::openFileForRun()
{
    // ensure startRun() has been invoked
    Assert(state == STARTED);
    state = OPENED;

    // open database
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());

    int commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ);
    writer.setCommitFreq(commitFreq);

    // write run data
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), SimTime::getScaleExp(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getSelectedConfigEntries());
}

void SqliteOutputScalarManager::closeFile()
{
    writer.close();
}

bool SqliteOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (state == ENDED)
        return false;    // ignore writes during network teardown

    Assert(state == STARTED || state == OPENED);

    if (state != OPENED)
        openFileForRun();

    if (isBad())
        return false;

    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return false;

    writer.recordScalar(componentFullPath, name, value, ResultFileUtils::convertMap(attributes));
    return true;
}

bool SqliteOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    if (state == ENDED)
        return false;    // ignore writes during network teardown

    Assert(state == STARTED || state == OPENED);

    if (state != OPENED)
        openFileForRun();

    if (isBad())
        return false;

    if (!name)
        name = statistic->getFullName();
    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    // check that recording this statistic is not disabled as a whole
    std::string objectFullPath = componentFullPath + "." + name;
    bool enabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return false;

    Statistics stats;
    if (!statistic->isWeighted())
        stats = Statistics::makeUnweighted(statistic->getCount(), statistic->getMin(), statistic->getMax(), statistic->getSum(), statistic->getSqrSum());
    else
        stats = Statistics::makeWeighted(statistic->getCount(), statistic->getMin(), statistic->getMax(), statistic->getSumWeights(), statistic->getWeightedSum(), statistic->getSqrSumWeights(), statistic->getWeightedSqrSum());

    bool savedAsHistogram = false;
    if (cAbstractHistogram *histogram = dynamic_cast<cAbstractHistogram *>(statistic)) {
        // check that recording the histogram is enabled
        bool binsEnabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_BIN_RECORDING);
        if (binsEnabled) {
            if (!histogram->binsAlreadySetUp())
                histogram->setUpBins();

            Histogram bins;
            int n = histogram->getNumBins();
            if (n > 0) {
                bins.setBins(histogram->getBinEdges(), histogram->getBinValues());
                bins.setUnderflows(histogram->getUnderflowSumWeights());
                bins.setOverflows(histogram->getOverflowSumWeights());
                writer.recordHistogram(componentFullPath, name, stats, bins, ResultFileUtils::convertMap(attributes));
                savedAsHistogram = true;
            }
        }
    }

    if (!savedAsHistogram)
        writer.recordStatistic(componentFullPath, name, stats, ResultFileUtils::convertMap(attributes));
    return true;
}

bool SqliteOutputScalarManager::recordParameter(cPar *par)
{
    if (state == ENDED)
        return false;    // ignore writes during network teardown

    Assert(state == STARTED || state == OPENED);

    if (state != OPENED)
        openFileForRun();

    if (isBad())
        return false;

    std::string componentFullPath = par->getOwner()->getFullPath();
    const char *name = par->getName();
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_PARAM_RECORDING);
    if (!enabled)
        return false;

    writer.recordParameter(componentFullPath, name, par->str(), ResultFileUtils::convertProperties(par->getProperties()));
    return true;
}

bool SqliteOutputScalarManager::recordComponentType(cComponent *component)
{
    if (state == ENDED)
        return false;    // ignore writes during network teardown

    Assert(state == STARTED || state == OPENED);

    if (state != OPENED)
        openFileForRun();

    if (isBad())
        return false;

    std::string componentFullPath = component->getFullPath();
    const char *name = "typename";
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_PARAM_RECORDING);
    if (!enabled)
        return false;

    const char *nedType = component->getComponentType()->getFullName();
    writer.recordParameter(componentFullPath, name, opp_quotestr(nedType), StringMap());
    return true;
}

void SqliteOutputScalarManager::flush()
{
    writer.flush();
}

}  // namespace envir
}  // namespace omnetpp
