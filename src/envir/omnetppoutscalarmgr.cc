//==========================================================================
//  OMNETPPOUTSCALARMGR.CC - part of
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
#include "envirbase.h"
#include "omnetppoutscalarmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

#define SCALAR_FILE_VERSION    2

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_PRECISION;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE_APPEND;

// per-scalar options
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;
extern omnetpp::cConfigOption *CFGID_BIN_RECORDING;

Register_Class(OmnetppOutputScalarManager);


OmnetppOutputScalarManager::OmnetppOutputScalarManager()
{
    initialized = false;
}

OmnetppOutputScalarManager::~OmnetppOutputScalarManager()
{
    closeFile();
}

void OmnetppOutputScalarManager::openFile()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());
}

void OmnetppOutputScalarManager::closeFile()
{
    writer.close();
}

void OmnetppOutputScalarManager::startRun()
{
    // finish up previous run
    initialized = false;
    closeFile();

    // delete file left over from previous runs
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old output scalar file");

    int prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
    writer.setPrecision(prec);
}

void OmnetppOutputScalarManager::endRun()
{
    initialized = false;
    if (writer.isOpen()) {
        writer.endRecordingForRun();
        closeFile();
    }
}

inline StringMap convertMap(const opp_string_map *m) {
    StringMap result;
    if (m)
        for (auto pair : *m)
            result[pair.first.c_str()] = pair.second.c_str();
    return result;
}

void OmnetppOutputScalarManager::initialize()
{
    openFile();
    writeRunData();
}

void OmnetppOutputScalarManager::writeRunData()
{
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), ResultFileUtils::getRunAttributes(), ResultFileUtils::getIterationVariables(), ResultFileUtils::getParamAssignments());
}

void OmnetppOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
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

void OmnetppOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
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

const char *OmnetppOutputScalarManager::getFileName() const
{
    return fname.c_str();
}

void OmnetppOutputScalarManager::flush()
{
    if (writer.isOpen())
        writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

