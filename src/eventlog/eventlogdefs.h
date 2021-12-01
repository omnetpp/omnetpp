//=========================================================================
//  EVENTLOGDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOGDEFS_H
#define __OMNETPP_EVENTLOG_EVENTLOGDEFS_H

#include <cstdint> // for int64_t, our equivalent of Java's "long" type
#include "common/exception.h"
#include "common/commonutil.h"
#include "common/bigdecimal.h"
#include "omnetpp/platdep/platdefs.h"

namespace omnetpp {
namespace common { class FileReader; };
}  // namespace omnetpp

namespace omnetpp {
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
// the C++11 type uintptr_t here will be 32 bits on 32 bit platforms and 64 bits on 64 bit platforms
//
// SWIG provides (by cPtr) positive Java long values even for objects hanging in the upper two gigabytes
// of memory on 32 bit platforms; so to have pointers to C++ objects == in Java we must ensure
// that the eventlog API functions return values which will be converted to positive Java long
// values by SWIG.
//

typedef uintptr_t ptr_t;

typedef int64_t eventnumber_t;
#define EVENTNUMBER_PRINTF_FORMAT PRId64

typedef int64_t msgid_t;
#define MSGID_PRINTF_FORMAT PRId64

typedef int64_t txid_t;
#define TXID_PRINTF_FORMAT PRId64

inline int64_t abs64(int64_t a)
{
   return a < 0 ? -a : a;
}

using omnetpp::common::BigDecimal;
using omnetpp::common::FileReader;
using omnetpp::common::opp_runtime_error;

typedef BigDecimal simtime_t;
#define simtime_nil BigDecimal::MinusOne

}  // namespace eventlog
}  // namespace omnetpp


#endif
