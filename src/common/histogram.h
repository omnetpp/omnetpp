//=========================================================================
//  HISTOGRAM.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_HISTOGRAM_H
#define __OMNETPP_COMMON_HISTOGRAM_H

#include <vector>
#include <algorithm>
#include "commondefs.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

/**
 * Class for storing histogram bins.
 */
class COMMON_API Histogram {
    private:
        std::vector<double> binEdges;
        std::vector<double> binValues; // one less than bin edges
        double underflows = 0, overflows = 0;
    public:
        Histogram() {}
        Histogram(const Histogram& other) = default;
        inline void clear();
        inline void setBins(const std::vector<double>& edges, const std::vector<double>& values);
        inline void setBinEdges(const std::vector<double>& edges); // clears the histogram
        inline void setBinValues(const std::vector<double>& values);
        void setUnderflows(double d) {underflows = d;}
        void setOverflows(double d) {overflows = d;}

        inline void collect(double value, double weight=1);
        void collectIntoBin(int k, double weight=1) {binValues.at(k) += weight;}

        const std::vector<double>& getBinEdges() const {return binEdges;} // one more than values
        const std::vector<double>& getBinValues() const {return binValues;}
        int getNumBins() const {return binValues.size();}
        double getBinEdge(int k) const {return binEdges.at(k);} // bin[k] has edges [k] and [k+1]
        double getBinValue(int k) const {return binValues.at(k);}
        double getUnderflows() const {return underflows;}
        double getOverflows() const {return overflows;}
};

inline void Histogram::clear()
{
    binEdges.clear();
    binValues.clear();
    underflows = overflows = 0;
}

inline void Histogram::setBins(const std::vector<double>& edges, const std::vector<double>& values)
{
    Assert(edges.size() == values.size() + 1);
    binEdges = edges;
    binValues = values;
}

inline void Histogram::setBinEdges(const std::vector<double>& edges)
{
    binEdges = edges;
    binValues.resize(edges.size()-1);
    std::fill(binValues.begin(), binValues.end(), 0.0);
}

inline void Histogram::setBinValues(const std::vector<double>& values)
{
    Assert(binEdges.size() == binValues.size() + 1);
    binValues = values;
}

inline void Histogram::collect(double value, double weight)
{
    Assert(binEdges.size() == binValues.size() + 1);
    auto it = std::upper_bound(binEdges.begin(), binEdges.end(), value);
    int index = it - binEdges.begin() - 1;
    if (index == -1)
        underflows += weight;
    else if (index == (int)binValues.size())
        overflows += weight;
    else
        binValues[index] += weight;
}


}  // namespace common
}  // namespace omnetpp


#endif
