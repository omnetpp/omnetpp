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
#include "omnetpp/clog.h"

using namespace omnetpp::common;

namespace omnetpp {

#define BUFLEN    1024

bool cEnvir::isLoggingEnabled() const
{
    return cLog::isLoggingEnabled();
}

void cEnvir::printfmsg(const char *fmt, ...)
{
    static OPP_THREAD_LOCAL char staticbuf[BUFLEN];
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    alert(staticbuf);
}

void cEnvir::addLifecycleListener(cISimulationLifecycleListener *listener)
{
    getSimulation()->addLifecycleListener(listener);
}

void cEnvir::removeLifecycleListener(cISimulationLifecycleListener *listener)
{
    getSimulation()->removeLifecycleListener(listener);
}

uint64_t cEnvir::getUniqueNumber()
{
    return getSimulation()->getUniqueNumber();
}

int cEnvir::getParsimProcId() const
{
    return getSimulation()->getParsimProcId();
}

int cEnvir::getParsimNumPartitions() const
{
    return getSimulation()->getParsimNumPartitions();
}

}  // namespace omnetpp

