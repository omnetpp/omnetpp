//==========================================================================
//  STOPWATCH.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stopwatch.h"
#include "omnetpp/cexception.h"

namespace omnetpp {
namespace envir {

Stopwatch::Stopwatch()
{
    realtimeLimit = to_timeval(-1);
    cpuTimeLimitClocks = -1;
    hasTimeLimit_ = false;
    resetClock();
}

void Stopwatch::setRealTimeLimit(double seconds)
{
    realtimeLimit = to_timeval(seconds < 0 ? -1 : seconds);
    hasTimeLimit_ = realtimeLimit.tv_sec >= 0 || cpuTimeLimitClocks >= 0;
}

void Stopwatch::setCPUTimeLimit(double seconds)
{
    cpuTimeLimitClocks = seconds < 0 ? -1 : CLOCKS_PER_SEC*seconds;
    hasTimeLimit_ = realtimeLimit.tv_sec >= 0 || cpuTimeLimitClocks >= 0;
}

void Stopwatch::addRealtimeDelta()
{
    timeval now;
    gettimeofday(&now, nullptr);
    timeval deltaT = now - lastTime;
    elapsedTime += deltaT;
    lastTime = now;
}

void Stopwatch::addClockDelta()
{
    clock_t now = clock();
    clock_t delta = now - lastClock; // note: do not merge into the next statement, as delta must be computed using clock_t arithmetic
    elapsedClocks += delta;
    lastClock = now;
}

void Stopwatch::resetClock()
{
    elapsedTime.tv_sec = elapsedTime.tv_usec = 0;
    elapsedClocks = 0;
    clockRunning = false;
}

void Stopwatch::startClock()
{
    gettimeofday(&lastTime, nullptr);
    lastClock = clock();
    clockRunning = true;
}

void Stopwatch::stopClock()
{
    if (clockRunning) {
        addRealtimeDelta();
        addClockDelta();
        clockRunning = false;
    }
}

void Stopwatch::checkTimeLimits()
{
    // check real time limit
    if (realtimeLimit.tv_sec >= 0) {
        addRealtimeDelta();
        if (elapsedTime >= realtimeLimit)
            throw cTerminationException(E_REALTIME);
    }

    // check CPU time limit
    if (cpuTimeLimitClocks >= 0) {
        addClockDelta();
        if (elapsedClocks >= cpuTimeLimitClocks)
            throw cTerminationException(E_CPUTIME);
    }
}

timeval Stopwatch::getElapsedTime()
{
    if (clockRunning)
        addRealtimeDelta();
    return elapsedTime;
}

double Stopwatch::getElapsedSec()
{
    timeval elapsed = getElapsedTime();
    return elapsed.tv_sec + elapsed.tv_usec / 1000000.0;
}

double Stopwatch::getCPUUsageSec()
{
    if (clockRunning)
        addClockDelta();
    return elapsedClocks / (double)CLOCKS_PER_SEC;
}

}  // namespace envir
}  // namespace omnetpp

