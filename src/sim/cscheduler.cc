//=========================================================================
//  CSCHEDULER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cscheduler.h"
#include "cmessage.h"
#include "csimul.h"
#include "cmsgheap.h"
#include "macros.h"
#include "cenvir.h"
#include "cconfig.h"
#include "platdep/misc.h" // usleep


cScheduler::cScheduler()
{
    sim = NULL;
}

cScheduler::~cScheduler()
{
}

void cScheduler::setSimulation(cSimulation *_sim)
{
    sim = _sim;
}

//-----

Register_Class(cSequentialScheduler);

cMessage *cSequentialScheduler::getNextEvent()
{
    cMessage *msg = sim->msgQueue.peekFirst();
    if (!msg)
        throw new cTerminationException(eENDEDOK);
    return msg;
}

//-----

Register_Class(cRealTimeScheduler);

void cRealTimeScheduler::startRun()
{
    factor = ev.config()->getAsDouble("General", "realtimescheduler-scaling", 0);
    if (factor!=0)
        factor = 1/factor;
    doScaling = (factor!=0);

    gettimeofday(&baseTime, NULL);
}

void cRealTimeScheduler::endRun()
{
}

void cRealTimeScheduler::executionResumed()
{
    gettimeofday(&baseTime, NULL);
    baseTime = timeval_substract(baseTime, doScaling ? factor*sim->simTime() : sim->simTime());
}

bool cRealTimeScheduler::waitUntil(const timeval& targetTime)
{
    // if there's more than 200ms to wait, wait in 100ms chunks
    // in order to keep UI responsiveness by invoking ev.idle()
    timeval curTime;
    gettimeofday(&curTime, NULL);
    while (targetTime.tv_sec-curTime.tv_sec >=2 ||
           timeval_diff_usec(targetTime, curTime) >= 200000)
    {
        usleep(100000); // 100ms
        if (ev.idle())
            return false;
        gettimeofday(&curTime, NULL);
    }

    // difference is now at most 100ms, do it at once
    long usec = timeval_diff_usec(targetTime, curTime);
    if (usec>0)
        usleep(usec);
    return true;
}

cMessage *cRealTimeScheduler::getNextEvent()
{
    cMessage *msg = sim->msgQueue.peekFirst();
    if (!msg)
        throw new cTerminationException(eENDEDOK);

    // calculate target time
    simtime_t eventSimtime = msg->arrivalTime();
    timeval targetTime = timeval_add(baseTime, doScaling ? factor*eventSimtime : eventSimtime);

    // if needed, wait until that time arrives
    timeval curTime;
    gettimeofday(&curTime, NULL);
    if (timeval_greater(targetTime, curTime))
    {
        if (!waitUntil(targetTime))
            return NULL; // user break
    }
    else
    {
        // we're behind -- customized versions of this class may alert
        // if we're too much behind, or modify basetime to accept the skew
    }

    // ok, return the message
    return msg;
}



