//==========================================================================
//  OMNETPPSCALARFILEWRITER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_OMNETPPSCALARFILEWRITER_H
#define __OMNETPP_COMMON_OMNETPPSCALARFILEWRITER_H

#include <string>
#include <map>
#include <vector>
#include "statistics.h"
#include "histogram.h"

namespace omnetpp {
namespace common {

/**
 * Class for writing text-based output scalar files.
 */
class COMMON_API OmnetppScalarFileWriter
{
  public:
    typedef std::map<std::string, std::string> StringMap;
    typedef std::vector<std::pair<std::string, std::string>> OrderedKeyValueList;

  protected:
    std::string fname;  // output file name
    FILE *f = nullptr;  // file ptr of output file; nullptr if closed (not yet opened, or after error)
    int prec = 14;      // number of significant digits when writing doubles

  protected:
    void check(int fprintfResult);
    void writeAttributes(const StringMap& attributes);
    void writeStatisticFields(const Statistics& statistic);
    void writeStatisticField(const char *name, int64_t value);
    void writeStatisticField(const char *name, double value);
    bool isEnoughPrecision(const Histogram& bins, int prec);
    void writeBin(double lowerEdge, double value, int prec);

  public:
    OmnetppScalarFileWriter() {}
    virtual ~OmnetppScalarFileWriter();

    void open(const char *filename); // append if file exists
    void close();
    bool isOpen() const {return f != nullptr;} // IMPORTANT: file will be closed when an error occurs

    void setPrecision(int p) {prec = p;}
    int getPrecision() const {return prec;}

    void beginRecordingForRun(const std::string& runName, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& configEntries);
    void endRecordingForRun();
    void recordScalar(const std::string& componentFullPath, const std::string& name, double value, const StringMap& attributes);
    void recordStatistic(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const StringMap& attributes);
    void recordHistogram(const std::string& componentFullPath, const std::string& name, const Statistics& statistic, const Histogram& bins, const StringMap& attributes); //TODO should be done by recordStatistic; do recordWeightedStatistic instead!
    void recordParameter(const std::string& componentFullPath, const std::string& name, const std::string& value, const StringMap& attributes);
    void flush();
};

}  // namespace common
}  // namespace omnetpp

#endif
