//=========================================================================
//  EVENTLOGDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOGDEFS_H
#define __OMNETPP_EVENTLOGDEFS_H

#include <stdint.h> // for int64_t, our equivalent of Java's "long" type (Note: <cstdint> is C++11)
#include "common/exception.h"
#include "common/commonutil.h"
#include "common/bigdecimal.h"
#include "omnetpp/platdep/platdefs.h"

NAMESPACE_BEGIN
namespace common { class FileReader; };
NAMESPACE_END

NAMESPACE_BEGIN
namespace eventlog {

#if defined(EVENTLOG_EXPORT)
#  define EVENTLOG_API OPP_DLLEXPORT
#elif defined(EVENTLOG_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define EVENTLOG_API OPP_DLLIMPORT
#else
#  define EVENTLOG_API
#endif

#define PRINT_DEBUG_MESSAGES false

// the ptr_t type is used throughout the eventlog library to return C++ object pointers
// those pointers are used by the gui to efficiently access the C++ library without
// generating lots of garbage in the Java world (where the pointer is stored as a 64 long value)
// the unsigned long here will be 32 bits on 32 bit platforms and 64 bits on 64 bit platforms
//
// SWIG provides (by cPtr) positive Java long values even for objects hanging in the upper two gigabytes
// of memory on 32 bit platforms; so to have pointers to C++ objects == in Java we must ensure
// that the eventlog API functions return values which will be converted to positive Java long
// values by SWIG.
//
// note that C99 defines intptr_t and uintptr_t and uintptr_t would be sufficient here
// but not all supported compilers are C99 compatible
typedef unsigned long ptr_t;

typedef int64_t eventnumber_t;
#define EVENTNUMBER_PRINTF_FORMAT INT64_PRINTF_FORMAT
inline int64_t abs64(int64_t a)
{
   return a < 0 ? -a : a;
}

using OPP::common::BigDecimal;
using OPP::common::FileReader;
using OPP::common::opp_runtime_error;

typedef BigDecimal simtime_t;
#define simtime_nil BigDecimal::MinusOne

#ifndef PI
#define PI        3.141592653589793
#endif

} // namespace eventlog
NAMESPACE_END


#endif
