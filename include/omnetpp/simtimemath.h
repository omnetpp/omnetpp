//==========================================================================
//   SIMTIME.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SIMTIMEMATH_H
#define __OMNETPP_SIMTIMEMATH_H

#include "simtime.h"

namespace omnetpp {

/**
 * @addtogroup SimTime
 * @{
 */

// used locally; needed because sign of a%b is implementation dependent if a<0
inline int64_t _i64mod(const int64_t& any_t, const int64_t& positive_u)
{
    int64_t m = any_t % positive_u;
    return m>=0 ? m : m+positive_u;
}

/**
 * @brief simtime_t version of floor(double) from math.h.
 */
inline const SimTime floor(const SimTime& x)
{
    int64_t u = SimTime::getScale();
    int64_t t = x.raw();
    return SimTime().setRaw(t - _i64mod(t,u));
}

/**
 * @brief Generalized version of floor(), accepting a unit and an offset:
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
 * @brief simtime_t version of ceil(double) from math.h.
 */
inline const SimTime ceil(const SimTime& x)
{
    int64_t u = SimTime::getScale();
    int64_t t = x.raw() + u-1;
    return SimTime().setRaw(t - _i64mod(t,u));
}

/**
 * @brief Generalized version of ceil(), accepting a unit and an offset:
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
 * @brief Returns the absolute value of the simulation time x.
 */
inline const SimTime fabs(const SimTime& x)
{
    return x.raw()<0 ? -x : x;
}

/**
 * @brief Computes the quotient of the simulation times x and y. The quotient is the
 * algebraic quotient with any fractional part discarded (truncated towards zero).
 * The function internally relies on the standard C/C++ integer division (/) operation.
 *
 * The handling of division by zero (i.e. when y==0) is platform dependent;
 * on most platforms it causes a runtime error of some sort.
 *
 * @see fmod()
 */
inline int64_t div(const SimTime& x, const SimTime& y)
{
    return x.raw() / y.raw();
}

/**
 * @brief Computes the remainder of the division of two simulation times.
 *
 * The function internally relies on the standard C/C++ integer modulo (%)
 * operation. This has implications regarding the sign of the result:
 * If both operands are nonnegative then the remainder is nonnegative;
 * if not, the sign of the remainder is implementation-defined. However,
 * the following is always true (substitute div() for / and fmod() for %):
 * <tt>(x/y) * y + x%y == x</tt>
 *
 * The handling of division by zero (i.e. when y==0) is platform dependent;
 * on most platforms it causes a runtime error of some sort.
 *
 * @see div()
 */
inline const SimTime fmod(const SimTime& x, const SimTime& y)
{
    return SimTime().setRaw(x.raw() % y.raw());
}

/**
 * @brief Precise division of an integer and a simulation time.
 *
 * The result is returned as an integer part (in the return value) and
 * a fraction (in the fractionNumerator, fractionDenominator output
 * parameters).
 *
 * @see div()
 */
SIM_API int64_t preciseDiv(int64_t x, const SimTime& y, int64_t& fractionNumerator, int64_t& fractionDenominator);

/** @} */

}  // namespace omnetpp

#endif


