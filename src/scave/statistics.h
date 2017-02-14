//=========================================================================
//  STATISTICS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_STATISTICS_H
#define __OMNETPP_SCAVE_STATISTICS_H

#include <cfloat>
#include "scavedefs.h"
#include "scaveutils.h"

namespace omnetpp {
namespace scave {

/**
 * Class for collecting statistical data of the result
 * used in several places in Scave.
 */
class SCAVE_API Statistics
{
    private:
        long count;
        double min;
        double max;
        double sum;
        double sumSqr;

    public:
        Statistics() : count(0), min(POSITIVE_INFINITY), max(NEGATIVE_INFINITY), sum(0.0), sumSqr(0.0) {}
        Statistics(long count, double min, double max, double sum, double sumSqr)
            :count(count), min(min), max(max), sum(sum), sumSqr(sumSqr) {}
        Statistics(const Statistics& s) : count(s.count), min(s.min), max(s.max), sum(s.sum), sumSqr(s.sumSqr) {}

        long getCount() const { return count; }
        double getMin() const { return min; }
        double getMax() const { return max; }
        double getSum() const { return sum; }
        double getSumSqr() const { return sumSqr; }
        double getMean() const { return count == 0 ? NaN : sum / count; }
        double getStddev() const { return sqrt(getVariance()); }
        double getVariance() const;

        void collect(double value);
        void adjoin(const Statistics& other);
};

inline double Statistics::getVariance() const
{
    if (count == 0)
        return NaN;
    else {
        double var = (sumSqr - sum*sum/count) / (count-1);
        return var < 0 ? 0 : var;
    }
}

inline void Statistics::collect(double value)
{
    count++;
    min = (min < value ? min : value);
    max = (max > value ? max : value);
    sum += value;
    sumSqr += value * value;
}

inline void Statistics::adjoin(const Statistics& other)
{
    count += other.count;
    min = (min < other.min ? min : other.min);
    max = (max > other.max ? max : other.max);
    sum += other.sum;
    sumSqr += other.sumSqr;
}

} // namespace scave
}  // namespace omnetpp


#endif
