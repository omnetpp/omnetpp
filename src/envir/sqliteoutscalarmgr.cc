//==========================================================================
//  SQLITEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Zoltan Bojthe, Andras Varga
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

Register_Class(SqliteOutputScalarManager);

SqliteOutputScalarManager::SqliteOutputScalarManager()
{
    initialized = false;
}

SqliteOutputScalarManager::~SqliteOutputScalarManager()
{
}

void SqliteOutputScalarManager::openDb()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());

    int commitFreq = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_DB_COMMIT_FREQ);
    writer.setCommitFreq(commitFreq);
}

void SqliteOutputScalarManager::closeDb()
{
    writer.close();
}

void SqliteOutputScalarManager::startRun()
{
    // clean up file from previous runs
    initialized = false;
    closeDb();
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old SQLite output scalar file");
}

void SqliteOutputScalarManager::endRun()
{
    closeDb();
    initialized = false;
}

void SqliteOutputScalarManager::initialize()
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

void SqliteOutputScalarManager::writeRunData()
{
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), SimTime::getScaleExp(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getParamAssignments());

    // save numeric iteration variables as scalars as well, after saving them as run attributes (TODO this should not be necessary)
    std::vector<const char *> names = getEnvir()->getConfigEx()->getIterationVariableNames();
    for (const char *name : names) {
        const char *value = getEnvir()->getConfigEx()->getVariable(name);
        recordNumericIterationVariableAsScalar(name, value);
    }
}

void SqliteOutputScalarManager::recordNumericIterationVariableAsScalar(const char *name, const char *valueStr)
{
    static const std::string MODULE_FOR_RUNATTR_SCALAR("_runattrs_");
    char *e;
    setlocale(LC_NUMERIC, "C");
    double value = strtod(valueStr, &e);
    if (*e == '\0') {
        // plain number - just record as it is
        writer.recordScalar(MODULE_FOR_RUNATTR_SCALAR, name, value, convertMap(nullptr));
    }
    else if (e != valueStr) {
        // starts with a number, so it might be something like "100s"; if so, record it as scalar with "unit" attribute
        std::string unit;
        bool parsedOK = false;
        try {
            value = UnitConversion::parseQuantity(valueStr, unit);
            parsedOK = true;
        }
        catch (std::exception& e) {
        }
        StringMap attrs;
        if (parsedOK && !unit.empty())
            attrs["unit"] = unit;
        writer.recordScalar(MODULE_FOR_RUNATTR_SCALAR, name, value, attrs);
    }
}

void SqliteOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (enabled)
        writer.recordScalar(componentFullPath, name, value, convertMap(attributes));
}

void SqliteOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    static const double MinusInfinity = -1.0/0.0;

    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (isBad())
        return;

    if (!name)
        name = statistic->getFullName();
    if (!name || !name[0])
        name = "(unnamed)";

    std::string componentFullPath = component->getFullPath();
    // check that recording this statistic is not disabled as a whole
    std::string objectFullPath = componentFullPath + "." + name;
    bool enabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return;

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
                bins.reserveBins(n+2);
                bins.addBin(MinusInfinity, histogram->getUnderflowSumWeights());
                for (int i = 0; i < n; i++)
                    bins.addBin(histogram->getBinEdge(i), histogram->getBinValue(i));
                bins.addBin(histogram->getBinEdge(n), histogram->getOverflowSumWeights());
                writer.recordHistogram(componentFullPath, name, stats, bins, convertMap(attributes));
                savedAsHistogram = true;
            }
        }
    }

    if (!savedAsHistogram)
        writer.recordStatistic(componentFullPath, name, stats, convertMap(attributes));
}

const char *SqliteOutputScalarManager::getFileName() const
{
    return fname.c_str();
}

void SqliteOutputScalarManager::flush()
{
    writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

