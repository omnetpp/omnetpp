//=========================================================================
//  CNULLENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "omnetpp/cnullenvir.h"

using namespace omnetpp;

cNullEnvir::cNullEnvir(int ac, char **av, cConfiguration *c)
{
    argc = ac;
    argv = av;
    cfg = c;
    rng = new cMersenneTwister();
    nextUniqueNumber = 0;
}

cNullEnvir::~cNullEnvir()
{
    while (!listeners.empty()) {
        listeners.back()->listenerRemoved();
        listeners.pop_back();
    }
    delete cfg;
    delete rng;
}

void cNullEnvir::addLifecycleListener(cISimulationLifecycleListener *listener)
{
    std::vector<cISimulationLifecycleListener *>::iterator it = std::find(listeners.begin(), listeners.end(), listener);
    if (it == listeners.end()) {
        listeners.push_back(listener);
        listener->listenerAdded();
    }
}

void cNullEnvir::removeLifecycleListener(cISimulationLifecycleListener *listener)
{
    std::vector<cISimulationLifecycleListener *>::iterator it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        listener->listenerRemoved();
    }
}

void cNullEnvir::notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details)
{
    // make a copy of the listener list, to avoid problems from listeners
    // getting added/removed during notification
    std::vector<cISimulationLifecycleListener *> copy = listeners;
    for (auto & listener : copy) {
        try {
            listener->lifecycleEvent(eventType, details);
        }
        catch (std::exception& e) {  //XXX perhaps we shouldn't hide the exception!!!! just re-throw? then all notifyLifecycleListeners() calls MUST be surrounded with try-catch!!!!
            const char *eventName = cISimulationLifecycleListener::getSimulationLifecycleEventName(eventType);
            printfmsg("Error: Exception during notifying lifecycle listeners about %s event: %s", eventName, e.what());
        }
    }
}

