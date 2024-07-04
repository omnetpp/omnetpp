//==========================================================================
//   INTUTIL.H  - part of
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

#ifndef __OMNETPP_COMMON_INTUTIL_H
#define __OMNETPP_COMMON_INTUTIL_H

#include <cinttypes>
#include <cmath>
#include <string>
#include <type_traits>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

typedef int64_t intval_t;

// helper for checked_int_cast
COMMON_API void intCastError(const std::string& num, const char *errmsg=nullptr);

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The errmsg argument will be used for the error message.
 */
template<typename ToInt, typename FromInt>
ToInt checked_int_cast(FromInt x, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value && std::is_integral<FromInt>::value, "checked_int_cast expects integers");
    ToInt res = x;
    if ((x<0) != (res<0) || x-res != 0)  // note: x!=res would result in warning: signed-unsigned comparison
        omnetpp::common::intCastError(std::to_string(x), errmsg);
    return res;
}

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The errmsg argument will be used for the error message.
 */
template<typename ToInt>
ToInt checked_int_cast(double d, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value, "checked_int_cast expects integer template argument");
    ToInt res = d;
    // NOTE: To detect an overflow, we test whether the resulting integer is the same as
    // the truncated value of the double. On overflow, we always get different values
    // except on ARM processors where a double -> integer conversion is always returning 'maxInt'
    // for any double value >= maxInt. Unfortunatley, Intel processors return INDEFINITE_INTEGER in these cases
    // (all bits set to zero except MSB, which is set to 1) which is essentially 'minInt', resulting in
    // different behavior on the two platforms. We explicitly have to treat the maxInt case as an overflow
    // so the overflow error will be triggered also on ARM platforms in these case.
    // Converting 'minInt' is not considered as an over/under flow error as both platforms return 'minInt'
    //
    // See more here: https://stackoverflow.com/questions/70258776/different-behavior-of-double-and-int64-t-conversion-on-new-apple-silicon-arm64
    //
    if ((double)res != std::trunc(d) || res == std::numeric_limits<ToInt>::max())
        omnetpp::common::intCastError(std::to_string(d), errmsg);
    return res;
}

inline double safeCastToDouble(intval_t x)
{
    double d = (double)x;
    intval_t x2 = (intval_t)d;
    if (x != x2)
        throw opp_runtime_error("Integer %" PRId64 " too large, conversion to double would incur precision loss (use explicit cast to double to suppress this error)", (int64_t)x);
    return d;
}

// safe integer operations (throw exception on overflow):

COMMON_API intval_t safeAdd(intval_t a, intval_t b);
COMMON_API intval_t safeSub(intval_t a, intval_t b);
COMMON_API intval_t safeMul(intval_t a, intval_t b);
COMMON_API intval_t intPow(intval_t base, intval_t exp);
COMMON_API intval_t shift(intval_t a, intval_t b);

}  // namespace common
}  // namespace omnetpp

#endif


