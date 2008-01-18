//=========================================================================
//  COMMONUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _COMMONUTIL_H_
#define _COMMONUTIL_H_

#include <assert.h>
#include "commondefs.h"
#include "exception.h"
#include "inttypes.h"


#ifdef NDEBUG
#  define Assert(x)
#  define DBG(x)
#else
#  define Assert(expr)  ((void) ((expr) ? 0 : (throw opp_runtime_error("ASSERT: condition %s false, %s line %d", \
                        #expr, __FILE__, __LINE__), 0)))
//#  define DBG(x)  printf x
#  define DBG(x)
#endif

#ifdef _WIN32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

/**
 * Sets locale to Posix ("C"). Needed because we want to read/write real numbers
 * with "." as decimal separator always (and not "," used by some locales).
 * This affects sprintf(), strtod(), etc.
 */
COMMON_API void setPosixLocale();

//XXX docu
class COMMON_API DebugCall
{
  private:
    static int depth;
    std::string funcname;
  public:
    DebugCall(const char *fmt,...);
    ~DebugCall();
};

#define TRACE  DebugCall __x

/**
 * Performs the RDTSC (read time stamp counter) x86 instruction, and returns
 * the resulting high-resolution 64-bit timer value. This can be used for
 * ad-hoc performance measurements on Windows (this function returns 0 on
 * other platforms).
 *
 * See http://en.wikipedia.org/wiki/RDTSC
 */
uint64 readCPUTimeStampCounter();



#endif


