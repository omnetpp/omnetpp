//==========================================================================
//  OMNETPPSCALARFILEWRITER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

#define SCALAR_FILE_VERSION    3

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
        writeStatisticField("weights", statistic.getSumWeights()); //TODO sumWeights
        writeStatisticField("weightedSum", statistic.getWeightedSum());
        writeStatisticField("sqrSumWeights", statistic.getSumSquaredWeights());
        writeStatisticField("weightedSqrSum", statistic.getSumWeightedSquaredValues());
    }
}

void OmnetppScalarFileWriter::beginRecordingForRun(const std::string& runName, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& configEntries)
{
    Assert(isOpen());

    // save run
    check(fprintf(f, "run %s\n", QUOTE(runName.c_str())));

    // save run attributes
    writeAttributes(attributes);

    // save itervars
    for (auto pair : itervars)
        check(fprintf(f, "itervar %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));

    // save config entries
    for (auto& pair : configEntries)
        check(fprintf(f, "config %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));

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

bool OmnetppScalarFileWriter::isEnoughPrecision(const Histogram& bins, int prec)
{
    // check if the given precision is enough to make adjacent bin edges differ in the output
    int n = bins.getNumBins();
    char prevEdge[32];
    sprintf(prevEdge, "%.*g", prec, bins.getBinEdge(0));
    for (int i = 1; i <= n; i++) {
        char edge[32];
        sprintf(edge, "%.*g", prec, bins.getBinEdge(i));
        if (opp_streq(edge, prevEdge))
            return false;
        strcpy(prevEdge, edge);
    }
    return true;
}

void OmnetppScalarFileWriter::writeBin(double lowerEdge, double value, int prec)
{
    check(fprintf(f, "bin\t%.*g\t%.*g\n", prec, lowerEdge, prec, value));
}

void OmnetppScalarFileWriter::recordHistogram(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const Histogram& bins, const StringMap& attributes)
{
    Assert(isOpen());
    check(fprintf(f, "statistic %s %s\n", QUOTE(componentFullPath.c_str()), QUOTE(name.c_str())));
    writeStatisticFields(statistic);
    writeAttributes(attributes);

    const int MAX_MEANINGFUL_PRECISION = 16; // additional digits are usually just noise
    int localPrec = isEnoughPrecision(bins, prec) ? prec : MAX_MEANINGFUL_PRECISION;

    int n = bins.getNumBins();
    writeBin(-INFINITY, bins.getUnderflows(), localPrec);
    for (int i = 0; i < n; i++)
        writeBin(bins.getBinEdge(i), bins.getBinValue(i), localPrec);
    writeBin(bins.getBinEdge(n), bins.getOverflows(), localPrec);
}

void OmnetppScalarFileWriter::recordParameter(const std::string& componentFullPath, const std::string& name, const std::string& value, const StringMap& attributes)
{
    Assert(isOpen());
    check(fprintf(f, "par %s %s %s\n", QUOTE(componentFullPath.c_str()), QUOTE(name.c_str()), QUOTE(value.c_str())));
    writeAttributes(attributes);
}

void OmnetppScalarFileWriter::flush()
{
    Assert(isOpen());
    fflush(f);
}

}  // namespace common
}  // namespace omnetpp

