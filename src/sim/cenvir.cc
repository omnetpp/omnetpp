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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "commonutil.h"
#include "cconfiguration.h"
#include "csimulation.h"


cEnvir::cEnvir()
{
    disable_tracing = false;
    debug_on_errors = false;
    attach_debugger_on_errors = false;
    suppress_notifications = false; //FIXME set to true when not needed!
}

cEnvir::~cEnvir()
{
}

cConfigurationEx *cEnvir::getConfigEx()
{
    cConfigurationEx *cfg = dynamic_cast<cConfigurationEx *>(getConfig());
    if (!cfg)
        throw cRuntimeError("Illegal call to ev.getConfigEx(): configuration object is not subclassed from cConfigurationEx");
    return cfg;
}

// note: exploits the fact that evbuf does sync() on "\n"'s
void cEnvir::flushLastLine()
{
    evbuf *buf = (evbuf *)out.rdbuf();
    if (!buf->isempty())
        buf->sputn("\n",1);
}

//
// Temp buffer for vararg functions below.
// Note: using a static buffer reduces stack usage of activity() modules;
// it also makes the following functions non-reentrant, but we don't need
// them to be reentrant anyway.
//
#define BUFLEN 1024
static char staticbuf[BUFLEN];

void cEnvir::printfmsg(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    putsmsg(staticbuf);
}

int cEnvir::printf(const char *fmt,...)
{
    if (disable_tracing)
        return 0;

    va_list va;
    va_start(va, fmt);
    int len = vsnprintf(staticbuf, BUFLEN, fmt, va);
    staticbuf[BUFLEN-1] = '\0';
    va_end(va);

    // route it through streambuf to preserve ordering
    sputn(staticbuf, len);
    return len;
}

bool cEnvir::askYesNo(const char *fmt,...)
{
    VSNPRINTF(staticbuf, BUFLEN, fmt);
    return askyesno(staticbuf);
}


