//=========================================================================
//  HISTOGRAM.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#include "commondefs.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

/**
 * Class for storing histogram bins (cells).
 */
class COMMON_API Histogram {
    public:
        struct Bin {double lowerBound, count;};
    private:
        std::vector<Bin> bins;
    public:
        Histogram() {}
        Histogram(const Histogram& other) : bins(other.bins) {}
        void clear() {bins.clear();}

        void reserveBins(int expectedNumberOfBins) {bins.reserve(expectedNumberOfBins);}
        void addBin(double lowerBound, double count = 0);
        void collect(double value, double weight = 1);
        void collectIntoBin(int k, double weight = 1);

        const std::vector<Bin>& getBins() const {return bins;}
        std::vector<double> getBinLowerBounds() const;
        std::vector<double> getBinValues() const;

        int getNumBins() const {return bins.size();}
        double getBinLowerBound(int k) const;
        double getBinUpperBound(int k) const;
        double getBinValue(int k) const;
};

inline void Histogram::addBin(double lowerBound, double count)
{
    Assert(bins.empty() || lowerBound > bins.back().lowerBound); // preserve ordering
    bins.push_back(Bin { lowerBound, count} );
}

inline void Histogram::collect(double value, double weight)
{
    //TODO find corresponding cell, then add
}

inline void Histogram::collectIntoBin(int k, double weight)
{
    Assert(k >= 0 && k < (int)bins.size());
    bins[k].count += weight;
}

inline double Histogram::getBinLowerBound(int k) const
{
    Assert(k >= 0 && k < (int)bins.size());
    return bins[k].lowerBound;
}

inline double Histogram::getBinUpperBound(int k) const
{
    Assert(k >= 0 && k < (int)bins.size());
    return (k == (int)bins.size()-1) ? POSITIVE_INFINITY : bins[k+1].lowerBound;
}

inline double Histogram::getBinValue(int k) const
{
    Assert(k >= 0 && k < (int)bins.size());
    return bins[k].count;
}

inline std::vector<double> Histogram::getBinLowerBounds() const
{
    std::vector<double> result;
    result.reserve(bins.size());
    for (auto& bin : bins)
        result.push_back(bin.lowerBound);
    return result;
}

inline std::vector<double> Histogram::getBinValues() const
{
    std::vector<double> result;
    result.reserve(bins.size());
    for (auto& bin : bins)
        result.push_back(bin.count);
    return result;
}


}  // namespace common
}  // namespace omnetpp


#endif
