//==========================================================================
//  OMNETPPOUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_PRECISION;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE_APPEND;

// per-scalar options
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;
extern omnetpp::cConfigOption *CFGID_BIN_RECORDING;
extern omnetpp::cConfigOption *CFGID_PARAM_RECORDING;

Register_Class(OmnetppOutputScalarManager);

void OmnetppOutputScalarManager::startRun()
{
    // prevent reuse of object for multiple runs
    Assert(state == NEW);
    state = STARTED;

    // delete file left over from previous runs
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);

    bool shouldAppend = getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND);
    if (!shouldAppend)
        removeFile(fname.c_str(), "old output scalar file");

    // read configuration
    int prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
    writer.setPrecision(prec);
}

void OmnetppOutputScalarManager::endRun()
{
    Assert(state == NEW || state == STARTED || state == OPENED);
    state = ENDED;

    if (writer.isOpen()) {
        writer.endRecordingForRun();
        closeFile();
    }
}

void OmnetppOutputScalarManager::openFileForRun()
{
    // ensure startRun() has been invoked
    Assert(state == STARTED);
    state = OPENED;

    // open file
    mkPath(directoryOf(fname.c_str()).c_str());
    writer.open(fname.c_str());

    // write run data
    writer.beginRecordingForRun(ResultFileUtils::getRunId().c_str(), ResultFileUtils::getRunAttributes(),
            ResultFileUtils::getIterationVariables(), ResultFileUtils::getSelectedConfigEntries());
}

void OmnetppOutputScalarManager::closeFile()
{
    writer.close();
}

bool OmnetppOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
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
    bool enabled = getEnvir()->getConfig()->getAsBool((componentFullPath + "." + name).c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return false;

    writer.recordScalar(componentFullPath, name, value, ResultFileUtils::convertMap(attributes));
    return true;
}

bool OmnetppOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
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

bool OmnetppOutputScalarManager::recordParameter(cPar *par)
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

bool OmnetppOutputScalarManager::recordComponentType(cComponent *component)
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

void OmnetppOutputScalarManager::flush()
{
    if (writer.isOpen())
        writer.flush();
}

}  // namespace envir
}  // namespace omnetpp

