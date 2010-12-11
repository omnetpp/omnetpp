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
  Copyright (C) 2005-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_TIMEUTIL_H
#define __PLATDEP_TIMEUTIL_H

#include <time.h>  // localtime()
#include <stdio.h> // sprintf()
#include <math.h>  // fmod()

//
// Platform-independent gettimeofday(), and some basic timeval operations
//

#ifndef _WIN32
# include <sys/time.h>
# include <unistd.h>
#else
// Windows:
# include <sys/types.h>
# include <sys/timeb.h>  // ftime(), timeb

// Declare struct timeval. Note: timeval is declared in <winsock.h>/<winsock2.h>
// (they're mutually exclusive), but we don't want <omnetpp.h> to pull them in
// because winsock definitions often conflict with similar definitions in
// model code; we rather define struct timeval here ourselves.
// (Note: _WINSOCKAPI_ is used in the Windows SDK headers, _TIMEVAL_DEFINED in MinGW)
#if !defined(_WINSOCKAPI_) && !defined(_WINSOCK2API_) && !defined(_TIMEVAL_DEFINED)
#define _TIMEVAL_DEFINED
// NOTE: if this timeval definition conflicts with winsock.h's definition,
// make sure that winsock.h gets #included before this header!
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif

// Windows doesn't have gettimeofday(), so emulate it with ftime()
inline int gettimeofday(struct timeval *tv, struct timezone *)
{
    struct timeb tb;
    ftime(&tb);
    tv->tv_sec = (long) tb.time;
    tv->tv_usec = tb.millitm * 1000UL;
    return 0;
}
#endif /* _WIN32 */

inline timeval timeval_add(const timeval& a, const timeval& b)
{
    timeval res;
    res.tv_sec = a.tv_sec + b.tv_sec;
    res.tv_usec = a.tv_usec + b.tv_usec;
    if (res.tv_usec>1000000) {
        res.tv_sec++;
        res.tv_usec -= 1000000;
    }
    return res;
}

inline timeval timeval_add(const timeval& a, double b)
{
    double bInt;
    double bFrac = modf(b, &bInt);
    timeval res;
    res.tv_sec = a.tv_sec + (long)bInt;
    res.tv_usec = a.tv_usec + (long)floor(1000000.0*bFrac);
    if (res.tv_usec>1000000) {
        res.tv_sec++;
        res.tv_usec -= 1000000;
    }
    return res;
}

inline timeval timeval_substract(const timeval& a, const timeval& b)
{
    timeval res;
    res.tv_sec = a.tv_sec - b.tv_sec;
    res.tv_usec = a.tv_usec - b.tv_usec;
    if (res.tv_usec<0) {
        res.tv_sec--;
        res.tv_usec += 1000000;
    }
    return res;
}

inline timeval timeval_substract(const timeval& a, double b)
{
    double bInt;
    double bFrac = modf(b, &bInt);
    timeval res;
    res.tv_sec = a.tv_sec - (long)bInt;
    res.tv_usec = a.tv_usec - (long)floor(1000000.0*bFrac);
    if (res.tv_usec<0) {
        res.tv_sec--;
        res.tv_usec += 1000000;
    }
    return res;
}

inline bool timeval_greater(const timeval& a, const timeval& b)
{
    if (a.tv_sec==b.tv_sec)
        return a.tv_usec > b.tv_usec;
    else
        return (unsigned long)a.tv_sec > (unsigned long)b.tv_sec;
}

// On 32-bit architectures, good up to ~1200 hours
inline unsigned long timeval_msec(const timeval& a)
{
    return 1000*a.tv_sec + (a.tv_usec/1000);
}

// User must take care that result doesn't overflow!
inline long timeval_diff_usec(const timeval& t2, const timeval& t1)
{
    long sec = t2.tv_sec - t1.tv_sec;
    long usec = t2.tv_usec - t1.tv_usec;
    return sec*1000000L + usec;
}

inline timeval operator+(const timeval& a, const timeval& b) {return timeval_add(a,b);}
inline timeval operator-(const timeval& a, const timeval& b) {return timeval_substract(a,b);}

// prints time in "yyyy-mm-dd hh:mm:ss" format
inline char *opp_asctime(time_t t, char *buf)
{
    struct tm now = *localtime(&t);
    sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",
                now.tm_year+1900, now.tm_mon+1, now.tm_mday,
                now.tm_hour, now.tm_min, now.tm_sec);
    return buf;
}

#endif
