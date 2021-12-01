//=========================================================================
//  STATISTICS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_STATISTICS_H
#define __OMNETPP_COMMON_STATISTICS_H

#include <cmath>
#include <cstdint>
#include "commondefs.h"

namespace omnetpp {
namespace common {

/**
 * Descriptive statistics with weights.
 */
class COMMON_API Statistics
{
    private:
        bool weighted;
        double minValue;
        double maxValue;
        int64_t count; // the actual count of observations, independent of their weights
        double sumWeights;  // equals count in the unweighted case
        double sumWeightedValues; // sum in the unweighted case
        double sumSquaredWeights; // equals count in the unweighted case
        double sumWeightedSquaredValues; // sum of squared values in the unweighted case

    private:
        void assertUnweighted() const;

    public:
        Statistics(bool weighted=false) : weighted(weighted) {clear();}
        Statistics(const Statistics& s) = default;
        static Statistics makeUnweighted(int64_t count, double minValue, double maxValue, double sumValues, double sumSquaredValues);
        static Statistics makeWeighted(int64_t count, double minValue, double maxValue, double sumWeights, double sumWeightedValues, double sumSquaredWeights, double sumWeightedSquaredValues);
        static Statistics makeInvalid(bool weighted=false);
        void clear();
        void collect(double value);
        void collect(double value, double weight);
        void adjoin(const Statistics& other);

        bool isWeighted() const {return weighted;}
        double getMin() const {return minValue;}
        double getMax() const {return maxValue;}
        int64_t getCount() const {return count;} // actual count of values, regardless of their weights; see also getSumWeights()
        double getSumWeights() const {return sumWeights;}
        double getMean() const {return sumWeightedValues / sumWeights;}
        double getStddev() const;
        double getVariance() const;

        double getSum() const {assertUnweighted(); return sumWeightedValues;}
        double getSumSqr() const {assertUnweighted(); return sumWeightedSquaredValues;}
        double getWeightedSum() const {return sumWeightedValues;}
        double getSumSquaredWeights() const {return sumSquaredWeights;}
        double getSumWeightedSquaredValues() const {return sumWeightedSquaredValues;}
};

}  // namespace common
}  // namespace omnetpp


#endif
