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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
    loggingEnabled = true;
    suppressNotifications = false;  //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
}

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

}  // namespace omnetpp

