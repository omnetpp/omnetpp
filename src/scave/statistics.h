//=========================================================================
//  INDEXFILE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _STATISTICS_H_
#define _STATISTICS_H_

#include "scavedefs.h"

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
        Statistics() : _count(0), _min(DBL_MAX), _max(DBL_MIN), _sum(0.0), _sumSqr(0.0) {}
        Statistics(long count, double min, double max, double sum, double sumSqr)
            :_count(count), _min(min), _max(max), _sum(sum), _sumSqr(sumSqr) {}

        Statistics &operator=(const Statistics &other)
        {
            _count = other._count;
            _min = other._min;
            _max = other._max;
            _sum = other._sum;
            _sumSqr = other._sumSqr;
            return *this;
        }

        long count() const { return _count; }
        double min() const { return _min; }
        double max() const { return _max; }
        double sum() const { return _sum; }
        double sumSqr() const { return _sumSqr; }

        void collect(double value)
        {
            _count++;
            _min = (_min < value ? _min : value);
            _max = (_max > value ? _max : value);
            _sum += value;
            _sumSqr += value * value;
        }

        void adjoin(const Statistics &other)
        {
            _count += other._count;
            _min = (_min < other._min ? _min : other._min);
            _max = (_max > other._max ? _max : other._max);
            _sum += other._sum;
            _sumSqr += other._sumSqr;
        }
};

#endif
