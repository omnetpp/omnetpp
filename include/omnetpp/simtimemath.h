//==========================================================================
//   SIMTIME.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SIMTIMEMATH_H
#define __OMNETPP_SIMTIMEMATH_H

#include "simtime.h"

namespace omnetpp {

// used locally; needed because sign of a%b is implementation dependent if a<0
inline int64_t _i64mod(const int64_t& any_t, const int64_t& positive_u)
{
    int64_t m = any_t % positive_u;
    return m>=0 ? m : m+positive_u;
}

/**
 * simtime_t version of floor(double) from math.h.
 */
inline const SimTime floor(const SimTime& x)
{
    int64_t u = SimTime::getScale();
    int64_t t = x.raw();
    return SimTime().setRaw(t - _i64mod(t,u));
}

/**
 * Generalized version of floor(), accepting a unit and an offset:
 * floor(x,u,off) = floor((x-off)/u)*u + off.
 *
 * Examples: floor(2.1234, 0.1) = 2.1; floor(2.1234, 0.1, 0.007) = 2.107;
 * floor(2.1006, 0.1, 0.007) = 2.007.
 */
inline const SimTime floor(const SimTime& x, const SimTime& unit, const SimTime& offset = SimTime())
{
    int64_t off = offset.raw();
    int64_t u = unit.raw();
    int64_t t = x.raw() - off;
    return SimTime().setRaw(t - _i64mod(t,u) + off);
}

/**
 * simtime_t version of ceil(double) from math.h.
 */
inline const SimTime ceil(const SimTime& x)
{
    int64_t u = SimTime::getScale();
    int64_t t = x.raw() + u-1;
    return SimTime().setRaw(t - _i64mod(t,u));
}

/**
 * Generalized version of ceil(), accepting a unit and an offset:
 * ceil(x,u,off) = ceil((x-off)/u)*u + off.
 */
inline const SimTime ceil(const SimTime& x, const SimTime& unit, const SimTime& offset = SimTime())
{
    int64_t off = offset.raw();
    int64_t u = unit.raw();
    int64_t t = x.raw() - off + u-1;
    return SimTime().setRaw(t - _i64mod(t,u) + off);
}

/**
 * simtime_t version of fabs(double) from math.h.
 */
inline const SimTime fabs(const SimTime& x)
{
    return x.raw()<0 ? SimTime().setRaw(-x.raw()) : x;
}

/**
 * simtime_t version of fmod(double,double) from math.h.
 */
inline const SimTime fmod(const SimTime& x, const SimTime& y)
{
    return SimTime().setRaw(x.raw() % y.raw());
}

}  // namespace omnetpp

#endif


