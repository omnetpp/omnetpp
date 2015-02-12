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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "cnullenvir.h"

USING_NAMESPACE

cNullEnvir::cNullEnvir(int ac, char **av, cConfiguration *c)
{
    argc = ac;
    argv = av;
    cfg = c;
    rng = new cMersenneTwister();
    lastnum = 0;
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

void cNullEnvir::addListener(cISimulationLifecycleListener *listener)
{
    std::vector<cISimulationLifecycleListener*>::iterator it = std::find(listeners.begin(), listeners.end(), listener);
    if (it == listeners.end()) {
        listeners.push_back(listener);
        listener->listenerAdded();
    }
}

void cNullEnvir::removeListener(cISimulationLifecycleListener *listener)
{
    std::vector<cISimulationLifecycleListener*>::iterator it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        listener->listenerRemoved();
    }
}

void cNullEnvir::notifyListeners(SimulationLifecycleEventType eventType, cObject *details)
{
    // make a copy of the listener list, to avoid problems from listeners
    // getting added/removed during notification
    std::vector<cISimulationLifecycleListener*> copy = listeners;
    for (int i=0; i<(int)copy.size(); i++)
        copy[i]->lifecycleEvent(eventType, details);
}


