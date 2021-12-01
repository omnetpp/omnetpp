//=========================================================================
//  PLATDEP/TIME.H - part of
//
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
#ifndef __OMNETPP_PLATDEP_TIMEUTIL_H
#define __OMNETPP_PLATDEP_TIMEUTIL_H

#include <cstdint> // int64_t
#include <ctime>  // localtime()
#include <cstdio> // sprintf()
#include <cmath>  // modf()
#include <cassert>
#include "platdefs.h"  // _WIN32

//
// Platform-independent gettimeofday() and timeval.
// Note: do no use these functions for timing real-time simulations, use
// opp_get_monotonic_clock_usecs() or opp_get_monotonic_clock_nsecs() instead.
//
#ifndef _WIN32

# include <sys/time.h>
# include <unistd.h>

#else

# include <winsock2.h>  // timeval
# include <sys/types.h>
# include <sys/timeb.h>  // ftime(), timeb

// Windows doesn't have gettimeofday(), so emulate it with ftime()
[[deprecated("use opp_get_monotonic_clock_usecs() or opp_get_monotonic_clock_nsecs() instead")]]
inline int gettimeofday(struct timeval *tv, struct timezone *)
{
    struct timeb tb;
    ftime(&tb);
    tv->tv_sec = (long) tb.time;
    tv->tv_usec = tb.millitm * 1000UL;
    return 0;
}

#endif // _WIN32

#endif
