//=========================================================================
//  CSCHEDULER.CC - part of
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

#include "omnetpp/cscheduler.h"
#include "omnetpp/cevent.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/globals.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/platdep/platmisc.h"  // usleep

namespace omnetpp {

Register_GlobalConfigOption(CFGID_REALTIMESCHEDULER_SCALING, "realtimescheduler-scaling", CFG_DOUBLE, nullptr, "When cRealTimeScheduler is selected as scheduler class: ratio of simulation time to real time. For example, `realtimescheduler-scaling=2` will cause simulation time to progress twice as fast as runtime.");

std::string cScheduler::str() const
{
    return "(no scheduler info provided)";
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

std::string cSequentialScheduler::str() const
{
    return "";  // NOTE: Do not change this to "sequential scheduler"! As this scheduler is the "nothing special" scheduler, we don't want to advertise its presence.
}

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

std::string cRealTimeScheduler::str() const
{
    if (!doScaling)
        return "real-time scheduling";
    else {
        char buf[64];
        sprintf(buf, "scaled real-time scheduling (%gx)", factor);
        return buf;
    }
}

void cRealTimeScheduler::startRun()
{
    factor = getEnvir()->getConfig()->getAsDouble(CFGID_REALTIMESCHEDULER_SCALING);
    if (factor != 0)
        factor = 1 / factor;
    doScaling = (factor != 0);

    baseTime = opp_get_monotonic_clock_usecs();
}

int64_t cRealTimeScheduler::toUsecs(simtime_t t)
{
    return (int64_t) (1000000 * (doScaling ? factor * t.dbl() : t.dbl()));
}

void cRealTimeScheduler::executionResumed()
{
    baseTime = opp_get_monotonic_clock_usecs();
    baseTime = baseTime - toUsecs(sim->getSimTime());
}

bool cRealTimeScheduler::waitUntil(int64_t targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking getEnvir()->idle()
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    while (targetTime - currentTime >= 200000) {
        usleep(100000);  // 100ms
        if (getEnvir()->idle())
            return false;
        currentTime = opp_get_monotonic_clock_usecs();
    }

    // difference is now at most 100ms, do it at once
    int64_t remaining = targetTime - currentTime;
    if (remaining > 0)
        usleep(remaining);
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
    int64_t targetTime = baseTime + toUsecs(eventSimtime);

    // if needed, wait until that time arrives
    int64_t currentTime = opp_get_monotonic_clock_usecs();
    if (targetTime > currentTime) {
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

}  // namespace omnetpp

