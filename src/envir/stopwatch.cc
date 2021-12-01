//==========================================================================
//  STOPWATCH.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "stopwatch.h"
#include "common/commonutil.h"
#include "omnetpp/cexception.h"
#include "omnetpp/simutil.h"

namespace omnetpp {
namespace envir {

Stopwatch::Stopwatch()
{
    resetClock();
}

void Stopwatch::setRealTimeLimit(double seconds)
{
    realtimeLimitUsecs = seconds < 0 ? -1 : (int64_t)(seconds * 1000000);
    hasTimeLimit_ = realtimeLimitUsecs >= 0 || cpuTimeLimitClocks >= 0;
}

void Stopwatch::setCPUTimeLimit(double seconds)
{
    cpuTimeLimitClocks = seconds < 0 ? -1 : CLOCKS_PER_SEC*seconds;
    hasTimeLimit_ = realtimeLimitUsecs >= 0 || cpuTimeLimitClocks >= 0;
}

void Stopwatch::addRealtimeDelta()
{
    int64_t now = opp_get_monotonic_clock_usecs();
    int64_t delta = now - lastTimeUsecs;
    elapsedTimeUsecs += delta;
    lastTimeUsecs = now;
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
    elapsedTimeUsecs = 0;
    elapsedClocks = 0;
    clockRunning = false;
}

void Stopwatch::startClock()
{
    lastTimeUsecs = opp_get_monotonic_clock_usecs();
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
    if (realtimeLimitUsecs >= 0) {
        addRealtimeDelta();
        if (elapsedTimeUsecs >= realtimeLimitUsecs)
            throw cTerminationException(E_REALTIME);
    }

    // check CPU time limit
    if (cpuTimeLimitClocks >= 0) {
        addClockDelta();
        if (elapsedClocks >= cpuTimeLimitClocks)
            throw cTerminationException(E_CPUTIME);
    }
}

double Stopwatch::getElapsedSecs()
{
    if (clockRunning)
        addRealtimeDelta();
    return elapsedTimeUsecs / 1000000.0;
}

double Stopwatch::getCPUUsageSecs()
{
    if (clockRunning)
        addClockDelta();
    return elapsedClocks / (double)CLOCKS_PER_SEC;
}

}  // namespace envir
}  // namespace omnetpp

