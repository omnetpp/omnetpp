//=========================================================================
//  COMMONUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_COMMONUTIL_H
#define __OMNETPP_COMMON_COMMONUTIL_H

#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <typeinfo>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

#ifdef NDEBUG
#  define Assert(x) ((void)0)
#  define DBG(x)    ((void)0)
#else
#  define Assert(expr)  ((void) ((expr) ? 0 : (throw omnetpp::common::opp_runtime_error("Assert: Condition '%s' does not hold in function '%s' at %s:%d", \
                        #expr, __FUNCTION__, __FILE__, __LINE__), 0)))
#  define DBG(x)    ((void)0)  //or if needed: printf x
#endif


COMMON_API extern const double NaN;
COMMON_API extern const double POSITIVE_INFINITY;
COMMON_API extern const double NEGATIVE_INFINITY;

inline bool isNaN(double d) { return d != d;}
inline bool isPositiveInfinity(double d) { return d==POSITIVE_INFINITY; }
inline bool isNegativeInfinity(double d) { return d==NEGATIVE_INFINITY; }


#ifdef _MSC_VER
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif


#define VSNPRINTF(buffer, buflen, formatarg) \
    VSNPRINTF2(buffer, buflen, formatarg, formatarg)

#define VSNPRINTF2(buffer, buflen, lastarg, format) \
    va_list va; \
    va_start(va, lastarg); \
    vsnprintf(buffer, buflen, format, va); \
    buffer[buflen-1] = '\0'; \
    va_end(va);

/**
 * Sets locale to Posix ("C"). Needed because we want to read/write real numbers
 * with "." as decimal separator always (and not "," used by some locales).
 * This affects sprintf(), strtod(), etc.
 */
COMMON_API void setPosixLocale();

/**
 * A more convenient gethostname(). Does its best, but when it fails the result will be nullptr.
 */
COMMON_API const char *opp_gethostname();

/**
 * Returns the name of a C++ type, correcting the quirks of various compilers.
 */
COMMON_API const char *opp_typename(const std::type_info& t);

// internal for opp_insidemain() and opp_exiting()
COMMON_API extern bool __insidemain;
COMMON_API extern bool __exiting;

/**
 * Returns true if main() is on the stack, i.e. we are not during static
 * initialization or deinitialization.
 */
inline bool opp_insidemain() {return __insidemain;}

/**
 * Returns true after getting a TERM or INT signal (Windows)
 */
inline bool opp_exiting() {return __exiting;}

/**
 * Debugging aid: prints a message on entering/leaving methods; message
 * gets indented according to call depth. See TRACE_CALL macro.
 */
template <class T> struct ToString;

class COMMON_API CallTracer
{
  private:
    static int depth;
    std::string funcname;
    std::string result;
  public:
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    CallTracer(const char *fmt,...);
    ~CallTracer();

    _OPP_GNU_ATTRIBUTE(format(printf, 1, 2))
    static void printf(const char *fmt, ...);
    static void setDepth(int d) {depth = d;}
    template <class T> void setResult(T x) { result = ToString<T>::toString(x); };
};

// helper class because template method overloading sucks
// see: http://www.gotw.ca/publications/mill17.htm

template <class T> struct ToString
{
    static std::string toString(const T x) { std::ostringstream s; s << x; return s.str();}
};

template <class T> struct ToString<T*>
{
    static std::string toString(const T* x) { std::ostringstream s; s << ((void*)x); return s.str(); }
};

#define TRACE_CALL(...)  CallTracer __x(__VA_ARGS__)

#define TPRINTF(...)  CallTracer::printf(__VA_ARGS__)

#define RETURN(x) { __x.setResult(x); return x; }

/**
 * Not all our bison/flex based parsers are reentrant. This macro is meant
 * to catch and report concurrent invocations, e.g. from background threads
 * in the GUI code.
 */
#define NONREENTRANT_PARSER() \
    static bool active = false; \
    struct Guard { \
      Guard() {if (active) throw opp_runtime_error("non-reentrant parser invoked again while parsing"); active=true;} \
      ~Guard() {active=false;} \
    } __guard;

}  // namespace common
}  // namespace omnetpp

#endif
