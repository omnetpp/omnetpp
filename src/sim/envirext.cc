//==========================================================================
//  ENVIREXT.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/envirext.h"

using namespace omnetpp;

void cIOutputVectorManager::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        case LF_ON_SIMULATION_PAUSE: flush(); break;
        default: break;
    }
}

void cIOutputScalarManager::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        case LF_ON_SIMULATION_PAUSE: flush(); break;
        default: break;
    }
}

void cISnapshotManager::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_INITIALIZE: startRun(); break;
        case LF_ON_RUN_END: endRun(); break;
        default: break;
    }
}
