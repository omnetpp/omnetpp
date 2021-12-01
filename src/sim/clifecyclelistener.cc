//=========================================================================
//  CLIFECYCLELISTENER.CC - part of
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

#include "omnetpp/clifecyclelistener.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {

cISimulationLifecycleListener::~cISimulationLifecycleListener()
{
    getEnvir()->removeLifecycleListener(this);
}

const char *cISimulationLifecycleListener::getSimulationLifecycleEventName(SimulationLifecycleEventType eventType)
{
    switch (eventType) {
#define CASE(X) case LF_ ## X: return #X
        CASE(ON_STARTUP);
        CASE(PRE_NETWORK_SETUP);
        CASE(POST_NETWORK_SETUP);
        CASE(PRE_NETWORK_INITIALIZE);
        CASE(POST_NETWORK_INITIALIZE);
        CASE(ON_SIMULATION_START);
        CASE(ON_SIMULATION_PAUSE);
        CASE(ON_SIMULATION_RESUME);
        CASE(ON_SIMULATION_SUCCESS);
        CASE(ON_SIMULATION_ERROR);
        CASE(PRE_NETWORK_FINISH);
        CASE(POST_NETWORK_FINISH);
        CASE(ON_RUN_END);
        CASE(PRE_NETWORK_DELETE);
        CASE(POST_NETWORK_DELETE);
        CASE(ON_SHUTDOWN);
#undef CASE
        default: return "???";
    }
}

}  // namespace omnetpp
