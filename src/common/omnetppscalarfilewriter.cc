//==========================================================================
//  OMNETPPSCALARFILEWRITER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonutil.h"
#include "stringutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetppscalarfilewriter.h"

namespace omnetpp {
namespace common {

#define SCALAR_FILE_VERSION    2

OmnetppScalarFileWriter::OmnetppScalarFileWriter()
{
}

OmnetppScalarFileWriter::~OmnetppScalarFileWriter()
{
    close();
}

void OmnetppScalarFileWriter::open(const char *filename)
{
    fname = filename;
    f = fopen(fname.c_str(), "a");
    if (f == nullptr)
        throw opp_runtime_error("Cannot open output scalar file '%s'", fname.c_str());

    // Seek to the end of the file. This is needed because on Windows ftell() returns 0 even after
    // opening the file in append mode. On other systems ftell() correctly points to the end of the file.
    opp_fseek(f, 0, SEEK_END);  

    if (opp_ftell(f) == 0)
        check(fprintf(f, "version %d\n", SCALAR_FILE_VERSION));
}

void OmnetppScalarFileWriter::close()
{
    if (f) {
        fclose(f);
        f = nullptr;
        fname = "";
    }
}

void OmnetppScalarFileWriter::check(int fprintfResult)
{
    if (fprintfResult < 0) {
        close();
        throw opp_runtime_error("Cannot write output scalar file '%s'", fname.c_str());
    }
}

void OmnetppScalarFileWriter::writeAttributes(const StringMap& attributes)
{
    for (auto pair : attributes)
        check(fprintf(f, "attr %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
}

void OmnetppScalarFileWriter::writeStatisticField(const char *name, int64_t value)
{
    check(fprintf(f, "field %s %" PRId64 "\n", QUOTE(name), value));
}

void OmnetppScalarFileWriter::writeStatisticField(const char *name, double value)
{
    check(fprintf(f, "field %s %.*g\n", QUOTE(name), prec, value));
}

void OmnetppScalarFileWriter::writeStatisticFields(const Statistics& statistic)
{
    // NOTE: mean and stddev may be computed from the others (and the IDE and
    // scavetool do, too), but we store them for convenience of 3rd party tools
    writeStatisticField("count", statistic.getCount());
    writeStatisticField("mean", statistic.getMean()); // computed; see note above
    writeStatisticField("stddev", statistic.getStddev()); // computed; see note above
    writeStatisticField("min", statistic.getMin());
    writeStatisticField("max", statistic.getMax());
    if (!statistic.isWeighted()) {
        writeStatisticField("sum", statistic.getSum());
        writeStatisticField("sqrsum", statistic.getSumSqr());
    }
    else {
        writeStatisticField("weights", statistic.getSumWeights());
        writeStatisticField("weightedSum", statistic.getWeightedSum());
        writeStatisticField("sqrSumWeights", statistic.getSumSquaredWeights());
        writeStatisticField("weightedSqrSum", statistic.getSumWeightedSquaredValues());
    }
}

void OmnetppScalarFileWriter::beginRecordingForRun(const std::string& runName, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& paramAssignments)
{
    Assert(isOpen());

    // save run
    check(fprintf(f, "run %s\n", QUOTE(runName.c_str())));

    // save run attributes
    writeAttributes(attributes);

    // save itervars
    for (auto pair : itervars)
        check(fprintf(f, "itervar %s %s\n", pair.first.c_str(), QUOTE(pair.second.c_str())));

    // save run params
    for (auto& pair : paramAssignments)
        check(fprintf(f, "param %s %s\n", pair.first.c_str(), QUOTE(pair.second.c_str())));

    check(fprintf(f, "\n"));
}

void OmnetppScalarFileWriter::endRecordingForRun()
{
    Assert(isOpen());
    check(fprintf(f, "\n"));
}

void OmnetppScalarFileWriter::recordScalar(const std::string& componentFullPath, const std::string& name, double value, const StringMap& attributes)
{
    Assert(isOpen());
    check(fprintf(f, "scalar %s %s %.*g\n", QUOTE(componentFullPath.c_str()), QUOTE(name.c_str()), prec, value));
    writeAttributes(attributes);
}

void OmnetppScalarFileWriter::recordStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const StringMap& attributes)
{
    Assert(isOpen());
    check(fprintf(f, "statistic %s %s\n", QUOTE(componentFullPath.c_str()), QUOTE(name.c_str())));
    writeStatisticFields(statistic);
    writeAttributes(attributes);
}

void OmnetppScalarFileWriter::recordHistogram(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const Histogram& bins, const StringMap& attributes)
{
    Assert(isOpen());
    check(fprintf(f, "statistic %s %s\n", QUOTE(componentFullPath.c_str()), QUOTE(name.c_str())));
    writeStatisticFields(statistic);
    writeAttributes(attributes);
    for (auto bin : bins.getBins())
        check(fprintf(f, "bin\t%.*g\t%.*g\n", prec, bin.lowerBound, prec, bin.count));
}

void OmnetppScalarFileWriter::flush()
{
    Assert(isOpen());
    fflush(f);
}

}  // namespace common
}  // namespace omnetpp

