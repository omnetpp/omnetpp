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

}  // namespace omnetpp

