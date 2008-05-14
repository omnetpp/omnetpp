//=========================================================================
//  STATISTICS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include <float.h>
#include "scavedefs.h"
#include "scaveutils.h"

NAMESPACE_BEGIN

/**
 * Class for collecting statistical data of the result
 * used in several places in Scave.
 */
class SCAVE_API Statistics
{
    private:
        long _count;
        double _min;
        double _max;
        double _sum;
        double _sumSqr;

    public:
        Statistics() : _count(0), _min(dblPositiveInfinity), _max(dblNegativeInfinity), _sum(0.0), _sumSqr(0.0) {}
        Statistics(long count, double min, double max, double sum, double sumSqr)
            :_count(count), _min(min), _max(max), _sum(sum), _sumSqr(sumSqr) {}

        long count() const { return _count; }
        double min() const { return _min; }
        double max() const { return _max; }
        double sum() const { return _sum; }
        double sumSqr() const { return _sumSqr; }
        double mean() const { return _count == 0 ? dblNaN : _sum / _count; }
        double stddev() const { return sqrt(variance()); }
        double variance() const;

        void collect(double value);
        void adjoin(const Statistics &other);
};

inline double Statistics::variance() const
{
            if (_count >= 1)
            {
                double var = (_sumSqr - _sum*_sum/_count)/(_count-1);
                return var < 0 ? 0 : var;
            }
            else
                return dblNaN;
}

inline void Statistics::collect(double value)
{
            _count++;
            _min = (_min < value ? _min : value);
            _max = (_max > value ? _max : value);
            _sum += value;
            _sumSqr += value * value;
}

inline void Statistics::adjoin(const Statistics &other)
{
            _count += other._count;
            _min = (_min < other._min ? _min : other._min);
            _max = (_max > other._max ? _max : other._max);
            _sum += other._sum;
            _sumSqr += other._sumSqr;
}

NAMESPACE_END


#endif
