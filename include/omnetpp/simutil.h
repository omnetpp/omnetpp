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

#include <type_traits> // is_integer
#include <limits>   // numeric_limits
#include <cstring>  // for strlen, etc.
#include <cstdarg>  // for va_list
#include <cstdio>   // for sprintf
#include <cstdlib>  // for gcvt
#include <typeinfo>  // for type_info
#include <string>    // for std::string
#include "platdep/platmisc.h" // for gcvt, etc
#include "simkerneldefs.h"

namespace omnetpp {

// forward declarations
class cObject;
class cComponent;

/** @addtogroup Utilities */
//@{

// helpers for checked_int_cast
void intCastError(const std::string& num, const char *errmsg=nullptr);
void intCastError(const std::string& num, const cObject *context, const char *errmsg=nullptr);

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context argument will be used for the error message.
 */
template<typename ToInt, typename FromInt>
ToInt checked_int_cast(FromInt x, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value && std::is_integral<FromInt>::value, "checked_int_cast expects integers");
    ToInt res = x;
    if ((x<0) != (res<0) || x-res != 0)  // note: x!=res would result in warning: signed-unsigned comparison
        intCastError(std::to_string(x), errmsg);
    return res;
}

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context argument will be used for the error message.
 */
template<typename ToInt, typename FromInt>
ToInt checked_int_cast(FromInt x, const cObject *context, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value && std::is_integral<FromInt>::value, "checked_int_cast expects integers");
    ToInt res = x;
    if ((x<0) != (res<0) || x-res != 0)  // note: x!=res would result in warning: signed-unsigned comparison
        intCastError(std::to_string(x), context, errmsg);
    return res;
}

/**
 * @brief Safe integer cast: it throws an exception if in case of an overflow,
 * i.e. when if the target type cannot represent the value in the source type.
 * The context argument will be used for the error message.
 */
template<typename ToInt>
ToInt checked_int_cast(double d, const char *errmsg=nullptr)
{
    static_assert(std::is_integral<ToInt>::value, "checked_int_cast expects integer template argument");
    if (d < std::numeric_limits<ToInt>::min() || d > std::numeric_limits<ToInt>::max())
        intCastError(std::to_string(d), errmsg);
    return (ToInt)d;
}

/**
 * @brief Same as the standard strlen() function, except that it also accepts
 * nullptr and returns 0 for it.
 */
inline int opp_strlen(const char *);

/**
 * @brief Duplicates the string, using <tt>new char[]</tt>. For nullptr and empty
 * strings it returns nullptr.
 */
inline char *opp_strdup(const char *);

/**
 * @brief Same as the standard strcpy() function, except that nullptr as the
 * second argument is treated as an empty string ("").
 */
inline char *opp_strcpy(char *,const char *);

/**
 * @brief Same as the standard strcmp() function, except that nullptr is treated
 * exactly as an empty string ("").
 */
inline int opp_strcmp(const char *, const char *);

/**
 * @brief Copies src string into dest, and if its length would exceed maxlen,
 * it is truncated with an ellipsis. For example, <tt>opp_strprettytrunc(buf,
 * "long-long",6)</tt> yields <tt>"lon..."</tt>.
 */
SIM_API char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen);

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

//@}

// INTERNAL: returns the name of a C++ type, correcting the quirks of various compilers.
SIM_API const char *opp_demangle_typename(const char *mangledName);


// implementations:

inline char *opp_strcpy(char *s1,const char *s2)
{
    return strcpy(s1, s2 ? s2 : "");
}

inline int opp_strlen(const char *s)
{
    return s ? strlen(s) : 0;
}

inline char *opp_strdup(const char *s)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[strlen(s)+1];
    strcpy(p,s);
    return p;
}

inline char *opp_strdup(const char *s, int len)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[len+1];
    strncpy(p,s,len);
    p[len] = 0;
    return p;
}

inline int opp_strcmp(const char *s1, const char *s2)
{
    if (s1)
        return s2 ? strcmp(s1,s2) : (*s1 ? 1 : 0);
    else
        return (s2 && *s2) ? -1 : 0;
}

// internally used: appends [num] to the given string
inline void opp_appendindex(char *s, unsigned int i)
{
   while (*s) s++;
   *s++ = '[';
   if (i<10)
       {*s++ = '0'+i; *s++=']'; *s=0; return;}
   if (i<100)
       {*s++ = '0'+i/10; *s++='0'+i%10; *s++=']'; *s=0; return;}
   sprintf(s,"%d]",i);
}

// internal
inline std::string double_to_str(double t)
{
#if __cplusplus >= 201103L
   return std::to_string(t);
#else
   char buf[32];
   return gcvt(t,16,buf);
#endif
}

}  // namespace omnetpp

#endif


