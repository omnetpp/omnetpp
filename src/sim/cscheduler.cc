//=========================================================================
//  CSCHEDULER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cscheduler.h"
#include "omnetpp/cevent.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/platdep/platmisc.h"  // usleep

NAMESPACE_BEGIN

Register_GlobalConfigOption(CFGID_REALTIMESCHEDULER_SCALING, "realtimescheduler-scaling", CFG_DOUBLE, nullptr, "When cRealTimeScheduler is selected as scheduler class: ratio of simulation time to real time. For example, scaling=2 will cause simulation time to progress twice as fast as runtime.");

cScheduler::cScheduler()
{
    sim = nullptr;
}

cScheduler::~cScheduler()
{
}

void cScheduler::setSimulation(cSimulation *_sim)
{
    sim = _sim;
}

void cScheduler::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        case LF_ON_SIMULATION_RESUME: executionResumed(); break;
        default: break;
    }
}

//-----

Register_Class(cSequentialScheduler);

cEvent *cSequentialScheduler::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}

cEvent *cSequentialScheduler::takeNextEvent()
{
    for (;;)
    {
        cEvent *event = sim->getFES()->removeFirst();
        if (!event)
            throw cTerminationException(E_ENDEDOK);
        if (event->isStale())
            delete event;
        else
            return event;
    }
}

void cSequentialScheduler::putBackEvent(cEvent *event)
{
    sim->getFES()->putBackFirst(event);
}

//-----

Register_Class(cRealTimeScheduler);

cRealTimeScheduler::cRealTimeScheduler() : cScheduler()
{
}

cRealTimeScheduler::~cRealTimeScheduler()
{
}

void cRealTimeScheduler::startRun()
{
    factor = getEnvir()->getConfig()->getAsDouble(CFGID_REALTIMESCHEDULER_SCALING);
    if (factor != 0)
        factor = 1 / factor;
    doScaling = (factor != 0);

    gettimeofday(&baseTime, nullptr);
}

void cRealTimeScheduler::executionResumed()
{
    gettimeofday(&baseTime, nullptr);
    baseTime = timeval_subtract(baseTime, SIMTIME_DBL(doScaling ? factor * sim->getSimTime() : sim->getSimTime()));
}

bool cRealTimeScheduler::waitUntil(const timeval& targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    timeval curTime;
    gettimeofday(&curTime, nullptr);
    while (targetTime.tv_sec - curTime.tv_sec >= 2 ||
           timeval_diff_usec(targetTime, curTime) >= 200000)
    {
        usleep(100000);  // 100ms
        if (getEnvir()->idle())
            return false;
        gettimeofday(&curTime, nullptr);
    }

    // difference is now at most 100ms, do it at once
    long usec = timeval_diff_usec(targetTime, curTime);
    if (usec > 0)
        usleep(usec);
    return true;
}

cEvent *cRealTimeScheduler::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}

cEvent *cRealTimeScheduler::takeNextEvent()
{
    cEvent *event = sim->getFES()->peekFirst();
    if (!event)
        throw cTerminationException(E_ENDEDOK);

    // calculate target time
    simtime_t eventSimtime = event->getArrivalTime();
    timeval targetTime = timeval_add(baseTime, SIMTIME_DBL(doScaling ? factor * eventSimtime : eventSimtime));

    // if needed, wait until that time arrives
    timeval curTime;
    gettimeofday(&curTime, nullptr);
    if (timeval_greater(targetTime, curTime)) {
        if (!waitUntil(targetTime))
            return nullptr;  // user break
    }
    else {
        // we're behind -- customized versions of this class may alert
        // if we're too much behind, or modify basetime to accept the skew
    }

    // remove event from FES and return it
    cEvent *tmp = sim->getFES()->removeFirst();
    ASSERT(tmp == event);
    return event;
}

void cRealTimeScheduler::putBackEvent(cEvent *event)
{
    sim->getFES()->putBackFirst(event);
}

NAMESPACE_END

