//=========================================================================
//  CENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "common/commonutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/csimulation.h"

using namespace omnetpp::common;

namespace omnetpp {

cConfigurationEx *cEnvir::getConfigEx()
{
    cConfigurationEx *cfg = dynamic_cast<cConfigurationEx *>(getConfig());
    if (!cfg)
        throw cRuntimeError("Illegal call to cEnvir::getConfigEx(): Configuration object is not subclassed from cConfigurationEx");
    return cfg;
}

#define BUFLEN    1024

void cEnvir::printfmsg(const char *fmt, ...)
{
    static char staticbuf[BUFLEN];
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    alert(staticbuf);
}

void cEnvir::addLifecycleListener(cISimulationLifecycleListener *listener)
{
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it == listeners.end()) {
        listeners.push_back(listener);
        listener->listenerAdded();
    }
}

void cEnvir::removeLifecycleListener(cISimulationLifecycleListener *listener)
{
    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
        listener->listenerRemoved();
    }
}

std::vector<cISimulationLifecycleListener*> cEnvir::getLifecycleListeners() const
{
    return listeners;
}

void cEnvir::notifyLifecycleListeners(SimulationLifecycleEventType eventType, cObject *details)
{
    // make a copy of the listener list, to avoid problems from listeners getting added/removed during notification
    auto copy = listeners;
    for (auto& listener : copy)
        listener->lifecycleEvent(eventType, details);  // let exceptions through, because errors must cause exitCode!=0
}

}  // namespace omnetpp

