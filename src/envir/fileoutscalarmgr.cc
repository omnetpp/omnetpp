//==========================================================================
//  FILEOUTPUTSCALARMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "simkerneldefs.h"

#include <assert.h>
#include <string.h>
#include <fstream>
#include "opp_ctype.h"
#include "cconfigkey.h"
#include "fileutil.h"
#include "bootenv.h"
#include "envirbase.h"
#include "csimulation.h"
#include "cmodule.h"
#include "cstatistic.h"
#include "cdensityestbase.h"
#include "fileoutscalarmgr.h"
#include "ccomponenttype.h"
#include "stringutil.h"

USING_NAMESPACE

#define SCALAR_FILE_VERSION 2
#define DEFAULT_PRECISION  "14"

Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE, "output-scalar-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.sca", "Name for the output scalar file.");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_PRECISION, "output-scalar-precision", CFG_INT, DEFAULT_PRECISION, "The number of significant digits for recording data into the output scalar file. The maximum value is ~15 (IEEE double precision).");
Register_PerRunConfigEntry(CFGID_OUTPUT_SCALAR_FILE_APPEND, "output-scalar-file-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it (OMNeT++ 3.x behavior), or delete it and begin a new file (default).");

Register_PerObjectConfigEntry(CFGID_SCALAR_RECORDING, "scalar-recording", CFG_BOOL, "true", "Whether the matching output scalars should be recorded. Syntax: <module-full-path>.<scalar-name>.scalar-recording=true/false. Example: **.queue.packetsDropped.scalar-recording=true");

Register_Class(cFileOutputScalarManager);

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw cRuntimeError("Cannot write output scalar file `%s'", fname.c_str())


cFileOutputScalarManager::cFileOutputScalarManager()
{
    f = NULL;
    prec = ev.getConfig()->getAsInt(CFGID_OUTPUT_SCALAR_PRECISION);
}

cFileOutputScalarManager::~cFileOutputScalarManager()
{
    closeFile();
}

void cFileOutputScalarManager::openFile()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    f = fopen(fname.c_str(),"a");
    if (f==NULL)
        throw cRuntimeError("Cannot open output scalar file `%s'", fname.c_str());
}

void cFileOutputScalarManager::closeFile()
{
    if (f)
    {
        fclose(f);
        f = NULL;
    }
}

void cFileOutputScalarManager::startRun()
{
    // clean up file from previous runs
    closeFile();
    fname = ev.getConfig()->getAsFilename(CFGID_OUTPUT_SCALAR_FILE).c_str();
    dynamic_cast<EnvirBase *>(&ev)->processFileName(fname);
    if (ev.getConfig()->getAsBool(CFGID_OUTPUT_SCALAR_FILE_APPEND)==false)
        removeFile(fname.c_str(), "old output scalar file");
    run.reset();
}

void cFileOutputScalarManager::endRun()
{
    closeFile();
}

inline bool isNumeric(const char *s)
{
    char *e;
    (void) strtod(s, &e);
    return *e=='\0';
}

void cFileOutputScalarManager::init()
{
    if (!f)
    {
        openFile();
        if (!f) return;
        
        CHECK(fprintf(f, "version %d\n", SCALAR_FILE_VERSION));
    }

    if (!run.initialized)
    {
        run.initRun();

        // this is the first scalar written in this run, write out run attributes
        run.writeRunData(f, fname);

        // save iteration variables
        std::vector<const char *> v = ev.getConfig()->getIterationVariableNames();
        for (int i=0; i<(int)v.size(); i++)
        {
            const char *name = v[i];
            const char *value = ev.getConfig()->getVariable(v[i]);
            //XXX write with using an "itervar" keyword not "scalar"
            if (isNumeric(value))
                CHECK(fprintf(f, "scalar . \t%s \t%s\n", name, value));
        }
    }
}

void cFileOutputScalarManager::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    if (!run.initialized)
        init();
    if (!f)
        return;

    if (!name || !name[0])
        name = "(unnamed)";

    bool enabled = ev.getConfig()->getAsBool((component->getFullPath()+"."+name).c_str(), CFGID_SCALAR_RECORDING);
    if (enabled)
    {
        CHECK(fprintf(f, "scalar %s \t%s \t%.*g\n", QUOTE(component->getFullPath().c_str()), QUOTE(name), prec, value));
        if (attributes)
            for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
                CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
    }
}

void cFileOutputScalarManager::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    if (!run.initialized)
        init();
    if (!f)
        return;

    if (!name)
        name = statistic->getFullName();
    if (!name || !name[0])
        name = "(unnamed)";

    // check that recording this statistic is not disabled as a whole
    std::string objectFullPath = component->getFullPath() + "." + name;
    bool enabled = ev.getConfig()->getAsBool(objectFullPath.c_str(), CFGID_SCALAR_RECORDING);
    if (!enabled)
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
    // In Scave, fields are read as separate scalars.
    CHECK(fprintf(f, "statistic %s \t%s\n", QUOTE(component->getFullPath().c_str()), QUOTE(name)));
    writeStatisticField("count", statistic->getCount());
    writeStatisticField("mean", statistic->getMean());
    writeStatisticField("stddev", statistic->getStddev());
    writeStatisticField("sum", statistic->getSum());
    writeStatisticField("sqrsum", statistic->getSqrSum());
    writeStatisticField("min", statistic->getMin());
    writeStatisticField("max", statistic->getMax());
    if (statistic->isWeighted())
    {
        writeStatisticField("weights", statistic->getWeights());
        writeStatisticField("weightedSum", statistic->getWeightedSum());
        writeStatisticField("sqrSumWeights", statistic->getSqrSumWeights());
        writeStatisticField("weightedSqrSum", statistic->getWeightedSqrSum());
    }

    if (attributes)
        for (opp_string_map::iterator it=attributes->begin(); it!=attributes->end(); it++)
            CHECK(fprintf(f,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    if (dynamic_cast<cDensityEstBase *>(statistic))
    {
        // check that recording the histogram is enabled
        bool enabled = ev.getConfig()->getAsBool((objectFullPath+":histogram").c_str(), CFGID_SCALAR_RECORDING);
        if (enabled)
        {
            cDensityEstBase *hist = (cDensityEstBase *)statistic;
            int n = hist->getNumCells();
            if (n>0)
            {
                CHECK(fprintf(f, "bin\t-INF\t%lu\n", hist->getUnderflowCell()));
                for (int i=0; i<n; i++)
                    CHECK(fprintf(f, "bin\t%.*g\t%.*g\n", prec, hist->getBasepoint(i), prec, hist->getCellValue(i)));
                CHECK(fprintf(f, "bin\t%.*g\t%lu\n", prec, hist->getBasepoint(n), hist->getOverflowCell()));
            }
        }
    }
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

