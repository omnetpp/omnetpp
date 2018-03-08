//==========================================================================
//  FILEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
#include "fileoutscalarmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#define SCALAR_FILE_VERSION    2

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_PRECISION;
extern omnetpp::cConfigOption *CFGID_OUTPUT_SCALAR_FILE_APPEND;

// per-scalar options
extern omnetpp::cConfigOption *CFGID_SCALAR_RECORDING;
extern omnetpp::cConfigOption *CFGID_BIN_RECORDING;

Register_Class(cFileOutputScalarManager);


cFileOutputScalarManager::cFileOutputScalarManager()
{
    initialized = false;
    f = nullptr;
    prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
}

cFileOutputScalarManager::~cFileOutputScalarManager()
{
    closeFile();
}

void cFileOutputScalarManager::openFile()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    f = fopen(fname.c_str(), "a");
    if (f == nullptr)
        throw cRuntimeError("Cannot open output scalar file '%s'", fname.c_str());

    // Seek to the end of the file. This is needed because on Windows ftell() returns 0 even after
    // opening the file in append mode. On other systems ftell() correctly points to the end of the file.
    opp_fseek(f, 0, SEEK_END);  
}

void cFileOutputScalarManager::closeFile()
{
    if (f) {
        fclose(f);
        f = nullptr;
    }
}

void cFileOutputScalarManager::check(int fprintfResult)
{
    if (fprintfResult < 0) {
        closeFile();
        throw cRuntimeError("Cannot write output scalar file '%s'", fname.c_str());
    }
}

void cFileOutputScalarManager::startRun()
{
    // finish up previous run
    initialized = false;
    closeFile();

    // delete file left over from previous runs
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    if (getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND) == false)
        removeFile(fname.c_str(), "old output scalar file");
}

void cFileOutputScalarManager::endRun()
{
    initialized = false;
    closeFile();
}

void cFileOutputScalarManager::initialize()
{
    openFile();
    if (opp_ftell(f) == 0)
        check(fprintf(f, "version %d\n", SCALAR_FILE_VERSION));

    ResultFileUtils::writeRunData(f, fname.c_str());

    // save numeric iteration variables as scalars as well, after saving them as run attributes (TODO this should not be necessary)
    std::vector<const char *> v = getEnvir()->getConfigEx()->getIterationVariableNames();
    for (int i = 0; i < (int)v.size(); i++) {
        const char *name = v[i];
        const char *value = getEnvir()->getConfigEx()->getVariable(v[i]);
        recordNumericIterationVariableAsScalar(name, value);
    }
}

void cFileOutputScalarManager::recordNumericIterationVariableAsScalar(const char *name, const char *value)
{
    char *e;
    setlocale(LC_NUMERIC, "C");
    (void)strtod(value, &e);
    if (*e == '\0') {
        // plain number - just record as it is
        check(fprintf(f, "scalar _runattrs_ %s %s\n", name, value));
    }
    else if (e != value) {
        // starts with a number, so it might be something like "100s"; if so, record it as scalar with "unit" attribute
        double d;
        std::string unit;
        bool parsedOK = false;
        try {
            d = UnitConversion::parseQuantity(value, unit);
            parsedOK = true;
        }
        catch (std::exception& e) {
        }
        if (parsedOK) {
            check(fprintf(f, "scalar _runattrs_ %s %.*g\n", name, prec, d));
            if (!unit.empty())
                check(fprintf(f, "attr unit %s\n", QUOTE(unit.c_str())));
        }
    }
}

void cFileOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!name || !name[0])
        name = "(unnamed)";

    bool enabled = getEnvir()->getConfig()->getAsBool((component->getFullPath()+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return;

    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (!f)
        return;

    check(fprintf(f, "scalar %s %s %.*g\n", QUOTE(component->getFullPath().c_str()), QUOTE(name), prec, value));
    if (attributes)
        for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it)
            check(fprintf(f, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
}

void cFileOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    if (!name)
        name = statistic->getFullName();
    if (!name || !name[0])
        name = "(unnamed)";

    // check that recording this statistic object is not disabled
    std::string objectFullPath = component->getFullPath() + "." + name;
    bool enabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
        return;

    if (!initialized) {
        initialized = true;
        initialize();
    }

    if (!f)
        return;

    // file format:
    //   statistic <modulepath> <statisticname>
    //   field count 343
    //   field weights 343
    //   field mean    2.1233
    //   field stddev  1.345
    //   attr unit s
    //   bin 0  3
    //   bin 10 13
    //   bin 20 19
    //   ...

    // NOTE: mean and stddev may be computed from the others (and the IDE and
    // scavetool do, too), but we store them for convenience of 3rd party tools
    check(fprintf(f, "statistic %s %s\n", QUOTE(component->getFullPath().c_str()), QUOTE(name)));
    writeStatisticField("count", statistic->getCount());
    writeStatisticField("mean", statistic->getMean()); // computed; see note above
    writeStatisticField("stddev", statistic->getStddev()); // computed; see note above
    writeStatisticField("min", statistic->getMin());
    writeStatisticField("max", statistic->getMax());
    if (!statistic->isWeighted()) {
        writeStatisticField("sum", statistic->getSum());
        writeStatisticField("sqrsum", statistic->getSqrSum());
    }
    else {
        writeStatisticField("weights", statistic->getSumWeights());
        writeStatisticField("weightedSum", statistic->getWeightedSum());
        writeStatisticField("sqrSumWeights", statistic->getSqrSumWeights());
        writeStatisticField("weightedSqrSum", statistic->getWeightedSqrSum());
    }

    if (attributes)
        for (opp_string_map::iterator it = attributes->begin(); it != attributes->end(); ++it)
            check(fprintf(f, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));


    if (cAbstractHistogram *histogram = dynamic_cast<cAbstractHistogram *>(statistic)) {
        // check that recording histogram bins is enabled
        bool binsEnabled = getEnvir()->getConfig()->getAsBool(objectFullPath.c_str(), CFGID_BIN_RECORDING);
        if (binsEnabled) {
            if (!histogram->binsAlreadySetUp())
                histogram->setUpBins();

            int n = histogram->getNumBins();
            if (n > 0) {
                check(fprintf(f, "bin\t-inf\t%.*g\n", prec, histogram->getUnderflowSumWeights()));
                for (int i = 0; i < n; i++)
                    check(fprintf(f, "bin\t%.*g\t%.*g\n", prec, histogram->getBinEdge(i), prec, histogram->getBinValue(i)));
                check(fprintf(f, "bin\t%.*g\t%.*g\n", prec, histogram->getBinEdge(n), prec, histogram->getOverflowSumWeights()));
            }
        }
    }
}

void cFileOutputScalarManager::writeStatisticField(const char *name, int64_t value)
{
    check(fprintf(f, "field %s %" PRId64 "\n", QUOTE(name), value));
}

void cFileOutputScalarManager::writeStatisticField(const char *name, double value)
{
    check(fprintf(f, "field %s %.*g\n", QUOTE(name), prec, value));
}

const char *cFileOutputScalarManager::getFileName() const
{
    return fname.c_str();
}

void cFileOutputScalarManager::flush()
{
    if (f)
        fflush(f);
}

}  // namespace envir
}  // namespace omnetpp

