//=========================================================================
//  CENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/commonutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/csimulation.h"

using namespace omnetpp::common;

namespace omnetpp {

cEnvir::cEnvir()
{
    disableTracing = false;
    debugOnErrors = false;
    attachDebuggerOnErrors = false;
    suppressNotifications = false;  //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
}

cConfigurationEx *cEnvir::getConfigEx()
{
    cConfigurationEx *cfg = dynamic_cast<cConfigurationEx *>(getConfig());
    if (!cfg)
        throw cRuntimeError("Illegal call to cEnvir::getConfigEx(): configuration object is not subclassed from cConfigurationEx");
    return cfg;
}

//
// Temp buffer for vararg functions below.
// Note: using a static buffer reduces stack usage of activity() modules;
// it also makes the following functions non-reentrant, but we don't need
// them to be reentrant anyway.
//
#define BUFLEN    1024
static char staticbuf[BUFLEN];

void cEnvir::printfmsg(const char *fmt, ...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    putsmsg(staticbuf);
}

bool cEnvir::askYesNo(const char *fmt, ...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    return askyesno(staticbuf);
}

}  // namespace omnetpp

