//=========================================================================
//  PLATDEP/TIME.H - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2005
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PLATDEP_TIME_H
#define __PLATDEP_TIME_H

#include <math.h>  // fmod()

#ifndef _MSC_VER
# include <sys/time.h>
# include <unistd.h>
#else
# include <sys/types.h>
# include <sys/timeb.h>  // ftime(), timeb
# include <winsock2.h>  // for timeval (!?)
# undef min  // would conflict with omnetpp's util.h
# undef max
inline int gettimeofday(struct timeval *tv, struct timezone *)
{
    struct timeb tb;
    ftime(&tb);
    tv->tv_sec = tb.time;
    tv->tv_usec = tb.millitm * 1000UL;
    return 0;
}
#endif

#ifdef _MSC_VER
#define usleep(x) _sleep((x)/1000)
#endif

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
    double byInt;
    double byFrac = modf(b, &byInt);
    timeval res;
    res.tv_sec = a.tv_sec + (long)byInt;
    res.tv_usec = a.tv_usec + (long)floor(1000000.0*byFrac);
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
    if (res.tv_usec<1000000) {
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

/**
 * User must take care that result doesn't overflow
 */
inline long timeval_diff_usec(const timeval& t2, const timeval& t1)
{
    long sec = t2.tv_sec - t1.tv_sec;
    long usec = t2.tv_usec - t1.tv_usec;
    return sec*1000000L + usec;
}

#endif
