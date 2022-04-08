//==========================================================================
//  SIMUTIL.H - part of
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

#ifndef __OMNETPP_SIMUTIL_H
#define __OMNETPP_SIMUTIL_H

#include <cmath>
#include <string>    // for std::string
#include <typeinfo>  // for type_info
#include <type_traits> // is_integer
#include "simkerneldefs.h"

namespace omnetpp {

// forward declarations
class cObject;
class cComponent;

/**
 * @addtogroup UtilityFunctions
 * @{
 */

// helpers for checked_int_cast
SIM_API void intCastError(const std::string& num, const char *errmsg=nullptr);
SIM_API void intCastError(const std::string& num, const cObject *context, const char *errmsg=nullptr);

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
        omnetpp::intCastError(std::to_string(x), errmsg);
    return res;
}

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context and errmsg arguments will be used for the error message.
 */
template<typename ToInt, typename FromInt>
ToInt checked_int_cast(FromInt x, const cObject *context, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value && std::is_integral<FromInt>::value, "checked_int_cast expects integers");
    ToInt res = x;
    if ((x<0) != (res<0) || x-res != 0)  // note: x!=res would result in warning: signed-unsigned comparison
        omnetpp::intCastError(std::to_string(x), context, errmsg);
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
    if ((double)res != std::trunc(d))
        omnetpp::intCastError(std::to_string(d), errmsg);
    return res;
}

/**
 * @brief Returns the name of a C++ type, correcting the quirks of various compilers.
 */
SIM_API const char *opp_typename(const std::type_info& t);

/**
 * @brief Returns a monotonic time in nanoseconds since some unspecified
 * starting point. This clock is not affected by discontinuous jumps in the
 * system time (e.g. if the system administrator manually changes the clock).
 * Note that the actual resolution (precision) of the clock may be less than
 * nanoseconds.
 */
SIM_API int64_t opp_get_monotonic_clock_nsecs();  // in gettime.cc

/**
 * @brief Returns a monotonic time in microseconds since some unspecified
 * starting point. This clock is not affected by discontinuous jumps in the
 * system time (e.g. if the system administrator manually changes the clock).
 * Note that the actual resolution (precision) of the clock may be less than
 * microseconds.
 */
SIM_API int64_t opp_get_monotonic_clock_usecs();  // in gettime.cc

/** @} */

}  // namespace omnetpp

#endif


