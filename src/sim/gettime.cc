//=========================================================================
//  GETTIME.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdint>

#ifdef _WIN32
# define WIN32_LEAN_AND_MEAN
# define NOGDI
# include <windows.h>
#elif defined __MACH__
# include <mach/clock.h>
# include <mach/mach.h>
#else
# include <ctime>
#endif

#include "omnetpp/simkerneldefs.h"

namespace omnetpp {

SIM_API int64_t opp_get_monotonic_clock_nsecs()
{
#ifdef _WIN32
    // use QPC API, see "Acquiring high-resolution time stamps"
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
    static int64_t ticksPerSec;
    if (ticksPerSec == 0) {
        LARGE_INTEGER tmp;
        QueryPerformanceFrequency(&tmp);
        ticksPerSec = tmp.QuadPart;
    }

    LARGE_INTEGER tmp;
    QueryPerformanceCounter(&tmp);
    int64_t time = tmp.QuadPart;
    time *= 1000000000;
    time /= ticksPerSec;
    return time;
#elif defined __MACH__
    // clock_gettime() is only supported from macOS 10.12, use MACH API until it becomes prevalent
    clock_serv_t cclock;
    mach_timespec_t mts;

    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    return (int64_t)mts.tv_sec * 1000000000 + mts.tv_nsec;
#else
    // assume POSIX
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
#endif
}

SIM_API int64_t opp_get_monotonic_clock_usecs()
{
    return opp_get_monotonic_clock_nsecs() / 1000;
}

} //namespace
