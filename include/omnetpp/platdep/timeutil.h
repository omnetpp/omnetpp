//=========================================================================
//  PLATDEP/TIME.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2005
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2017 Andras Varga
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
#include "platdefs.h"  // _OPPDEPRECATED

//
// Platform-independent gettimeofday() and timeval.
//
#ifndef _WIN32

# include <sys/time.h>
# include <unistd.h>

#else

# include <winsock2.h>  // timeval
# include <sys/types.h>
# include <sys/timeb.h>  // ftime(), timeb

// Windows doesn't have gettimeofday(), so emulate it with ftime()
// Deprecated, use opp_get_monotonic_clock_usecs or opp_get_monotonic_clock_nsecs instead.
_OPPDEPRECATED inline int gettimeofday(struct timeval *tv, struct timezone *)
{
    struct timeb tb;
    ftime(&tb);
    tv->tv_sec = (long) tb.time;
    tv->tv_usec = tb.millitm * 1000UL;
    return 0;
}

#endif // _WIN32

//
// timeval utility functions (all platforms)
//
namespace omnetpp {

_OPPDEPRECATED inline int64_t timeval_usec(const timeval& a)
{
    return (int64_t)a.tv_sec * 1000000 + a.tv_usec;
}

_OPPDEPRECATED inline timeval to_timeval(double b)
{
    double bInt;
    double bFrac = std::modf(b, &bInt);
    timeval res;
    res.tv_sec = (long)bInt;
    res.tv_usec = (long) std::floor(1000000.0*bFrac);
    if (res.tv_usec < 0) {
        res.tv_sec--;
        res.tv_usec += 1000000;
    }
    return res;
}

_OPPDEPRECATED inline timeval timeval_add(const timeval& a, const timeval& b)
{
    timeval res;
    res.tv_sec = a.tv_sec + b.tv_sec;
    res.tv_usec = a.tv_usec + b.tv_usec;
    if (res.tv_usec > 1000000) {
        res.tv_sec++;
        res.tv_usec -= 1000000;
    }
    return res;
}

_OPPDEPRECATED inline void timeval_addto(timeval& a, const timeval& b)
{
    a.tv_sec += b.tv_sec;
    a.tv_usec += b.tv_usec;
    if (a.tv_usec > 1000000) {
        a.tv_sec++;
        a.tv_usec -= 1000000;
    }
}

_OPPDEPRECATED inline timeval timeval_add(const timeval& a, double b)
{
    double bInt;
    double bFrac = modf(b, &bInt);
    timeval res;
    res.tv_sec = a.tv_sec + (long)bInt;
    res.tv_usec = a.tv_usec + (long) std::floor(1000000.0*bFrac);
    if (res.tv_usec > 1000000) {
        res.tv_sec++;
        res.tv_usec -= 1000000;
    }
    return res;
}

#define timeval_substract  timeval_subtract  /*spelling fixed*/

_OPPDEPRECATED inline timeval timeval_subtract(const timeval& a, const timeval& b)
{
    timeval res;
    res.tv_sec = a.tv_sec - b.tv_sec;
    res.tv_usec = a.tv_usec - b.tv_usec;
    if (res.tv_usec < 0) {
        res.tv_sec--;
        res.tv_usec += 1000000;
    }
    return res;
}

_OPPDEPRECATED inline timeval timeval_subtract(const timeval& a, double b)
{
    double bInt;
    double bFrac = modf(b, &bInt);
    timeval res;
    res.tv_sec = a.tv_sec - (long)bInt;
    res.tv_usec = a.tv_usec - (long) std::floor(1000000.0*bFrac);
    if (res.tv_usec < 0) {
        res.tv_sec--;
        res.tv_usec += 1000000;
    }
    return res;
}

_OPPDEPRECATED inline bool timeval_greater(const timeval& a, const timeval& b)
{
    if (a.tv_sec == b.tv_sec)
        return a.tv_usec > b.tv_usec;
    else
        return (unsigned long)a.tv_sec > (unsigned long)b.tv_sec;
}

// On 32-bit architectures, good up to ~1200 hours
_OPPDEPRECATED inline unsigned long timeval_msec(const timeval& a)
{
    return 1000*a.tv_sec + (a.tv_usec/1000);
}

// User must take care that result doesn't overflow!
_OPPDEPRECATED inline long timeval_diff_usec(const timeval& t2, const timeval& t1)
{
    long sec = t2.tv_sec - t1.tv_sec;
    long usec = t2.tv_usec - t1.tv_usec;
    return sec*1000000L + usec;
}

_OPPDEPRECATED inline timeval operator+(const timeval& a, const timeval& b) {return timeval_add(a,b);}
_OPPDEPRECATED inline timeval operator-(const timeval& a, const timeval& b) {return timeval_subtract(a,b);}
_OPPDEPRECATED inline void operator+=(timeval& a, const timeval& b) {timeval_addto(a,b);}
_OPPDEPRECATED inline bool operator>(const timeval& a, const timeval& b) {return timeval_greater(a,b);}
_OPPDEPRECATED inline bool operator<(const timeval& a, const timeval& b) {return timeval_greater(b,a);}
_OPPDEPRECATED inline bool operator>=(const timeval& a, const timeval& b) {return !timeval_greater(b,a);}
_OPPDEPRECATED inline bool operator<=(const timeval& a, const timeval& b) {return !timeval_greater(a,b);}


// prints time in "yyyy-mm-dd hh:mm:ss" format
_OPPDEPRECATED inline char *opp_asctime(time_t t, char *buf)
{
    struct tm now = *localtime(&t);
    sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",
                now.tm_year+1900, now.tm_mon+1, now.tm_mday,
                now.tm_hour, now.tm_min, now.tm_sec);
    return buf;
}

} // namespace omnetpp

#endif
