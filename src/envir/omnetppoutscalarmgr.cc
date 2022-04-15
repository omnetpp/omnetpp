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
#include "resultfileutils.h"
#include "omnetppoutscalarmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

typedef std::map<std::string, std::string> StringMap;

#define DEFAULT_OUTPUT_SCALAR_PRECISION    "14"

// global options
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_FILE, "output-scalar-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sca", "Name for the output scalar file.");
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_FILE_APPEND, "output-scalar-file-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it (OMNeT++ 3.x behavior), or delete it and begin a new file (default).");
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_PRECISION, "output-scalar-precision", CFG_INT, DEFAULT_OUTPUT_SCALAR_PRECISION, "The number of significant digits for recording data into the output scalar file. The maximum value is ~15 (IEEE double precision). This has no effect on SQLite recording, as it stores values as 8-byte IEEE floating point numbers.");

// per-scalar options
Register_PerObjectConfigOption(CFGID_SCALAR_RECORDING, "scalar-recording", KIND_SCALAR, CFG_BOOL, "true", "Whether the matching output scalars and statistic objects should be recorded.\nUsage: `<module-full-path>.<scalar-name>.scalar-recording=true/false`. To enable/disable individual recording modes for a @statistic (those added via the `record=...` key of `@statistic` or the `**.result-recording-modes=...` config option), use `<statistic-name>:<mode>` for `<scalar-name>`, and make sure the `@statistic` as a whole is not disabled with `**.<statistic-name>.statistic-recording=false`.\nExample: `**.ping.roundTripTime:stddev.scalar-recording=false`");
Register_PerObjectConfigOption(CFGID_BIN_RECORDING, "bin-recording", KIND_SCALAR, CFG_BOOL, "true", "Whether the bins of the matching histogram object should be recorded, provided that recording of the histogram object itself is enabled (`**.<scalar-name>.scalar-recording=true`).\nUsage: `<module-full-path>.<scalar-name>.bin-recording=true/false`. To control histogram recording from a `@statistic`, use `<statistic-name>:histogram` for `<scalar-name>`.\nExample: `**.ping.roundTripTime:histogram.bin-recording=false`");
Register_PerObjectConfigOption(CFGID_PARAM_RECORDING, "param-recording", KIND_PARAMETER, CFG_BOOL, "true", "Whether the matching module (and channel) parameters should be recorded.\nUsage: `<module-full-path>.<parameter-name>.param-recording=true/false`.\nExample: `**.app.pkLen.param-recording=true`");

Register_Class(OmnetppOutputScalarManager);

void OmnetppOutputScalarManager::configure(cSimulation *simulation, cConfiguration *cfg)
{
    this->cfg = cfg;
    ResultFileUtils::setConfiguration(cfg);
    simulation->addLifecycleListener(this);

    fname = cfg->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    fname = augmentFileName(fname);

    shouldAppend = cfg->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND);

    int prec = cfg->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
    writer.setPrecision(prec);
}

void OmnetppOutputScalarManager::startRun()
{
    // prevent reuse of object for multiple runs
    Assert(state == NEW);
    state = STARTED;

    // delete file left over from previous runs
    if (!shouldAppend)
        removeFile(fname.c_str(), "old output scalar file");
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
    writer.beginRecordingForRun(getRunId().c_str(), getRunAttributes(),
            getIterationVariables(), getSelectedConfigEntries());
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
    bool enabled = cfg->getAsBool((componentFullPath + "." + name).c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return false;

    writer.recordScalar(componentFullPath, name, value, convertMap(attributes));
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
    bool enabled = cfg->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
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
        bool binsEnabled = cfg->getAsBool(objectFullPath.c_str(), CFGID_BIN_RECORDING);
        if (binsEnabled) {
            if (!histogram->binsAlreadySetUp())
                histogram->setUpBins();

            Histogram bins;
            int n = histogram->getNumBins();
            if (n > 0) {
                bins.setBins(histogram->getBinEdges(), histogram->getBinValues());
                bins.setUnderflows(histogram->getUnderflowSumWeights());
                bins.setOverflows(histogram->getOverflowSumWeights());
                writer.recordHistogram(componentFullPath, name, stats, bins, convertMap(attributes));
                savedAsHistogram = true;
            }
        }
    }

    if (!savedAsHistogram)
        writer.recordStatistic(componentFullPath, name, stats, convertMap(attributes));

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
    bool enabled = cfg->getAsBool((componentFullPath+"."+name).c_str(), CFGID_PARAM_RECORDING);
    if (!enabled)
        return false;

    writer.recordParameter(componentFullPath, name, par->str(), convertProperties(par->getProperties()));
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
    bool enabled = cfg->getAsBool((componentFullPath+"."+name).c_str(), CFGID_PARAM_RECORDING);
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

