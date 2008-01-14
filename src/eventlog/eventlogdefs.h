//=========================================================================
//  EVENTLOGDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EVENTLOGDEFS_H_
#define _EVENTLOGDEFS_H_

#include "platdefs.h"
#include "exception.h"
#include "inttypes.h" // for int64, our equivalent of Java's "long" type
#include "commonutil.h"
#include "bigdecimal.h"


#if defined(EVENTLOG_EXPORT)
#  define EVENTLOG_API OPP_DLLEXPORT
#elif defined(EVENTLOG_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define EVENTLOG_API OPP_DLLIMPORT
#else
#  define EVENTLOG_API
#endif

#define PRINT_DEBUG_MESSAGES false

typedef BigDecimal simtime_t;

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

#define simtime_nil BigDecimal::MinusOne

#ifndef PI
#define PI        3.141592653589793
#endif

/**
 * Variables which store event numbers throughout the API may have these
 * additional values (positive values are real event numbers).
 */
enum EventNumberKind
{
    /**
     * The event number is not yet calculated before.
     */
    EVENT_NOT_YET_CALCULATED = -1,
    /**
     * There is no such event and will never be because it is impossible. (e.g. the previous event of the first event)
     */
    NO_SUCH_EVENT = -2,
    /**
     * The event is not and will never be in the log, although it could be. (e.g. filtered by the user from the file)
     */
    FILTERED_EVENT = -3,
    /**
     * The event is not yet in the log, but it might appear in the future (e.g. simulation not there yet).
     * The last event number is added to the constant and returned.
     * (e.g. reading the event 1000 when the file ends at 100 will give -104)
     */
    EVENT_NOT_YET_REACHED = -4
};

enum MatchKind
{
    EXACT, // returns not NULL if exactly one event was found
    FIRST_OR_PREVIOUS, // if multiple matches were found it returns the first one or the previous if there are no matches at all
    FIRST_OR_NEXT, // if multiple matches were found it returns the first one or the next if there are no matches at all
    LAST_OR_NEXT, // as per FIRST_OR_PREVIOUS but the opposite
    LAST_OR_PREVIOUS // as per FIRST_OR_NEXT but the opposite
};

enum TimelineMode {
    SIMULATION_TIME,
    EVENT_NUMBER,
    STEP,
    NONLINEAR
};

#endif
