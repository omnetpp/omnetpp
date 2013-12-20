//=========================================================================
//  CEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Exception classes
//
//  Author: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // for exit()
#include <stdarg.h>  // for va_list
#include <signal.h>  // raise()
#include "cexception.h"
#include "csimulation.h"
#include "ccomponent.h"
#include "cmodule.h"
#include "cenvir.h"
#include "cconfiguration.h"
#include "stringutil.h"
#include "commonutil.h"
#include "platdep/platmisc.h"  // for DEBUG_TRAP

NAMESPACE_BEGIN


#define BUFLEN 1024
static char buffer[BUFLEN];
static char buffer2[BUFLEN];

#define LL  INT64_PRINTF_FORMAT

cException::cException() : std::exception()
{
    errorcode = eCUSTOM;
    storeCtx();
    msg = "n/a";
}

cException::cException(OppErrorCode errorcode...) : std::exception()
{
    va_list va;
    va_start(va, errorcode);
    init(NULL, errorcode, cErrorMessages::get(errorcode), va);
    va_end(va);
}

cException::cException(const char *msgformat...) : std::exception()
{
    va_list va;
    va_start(va, msgformat);
    init(NULL, eCUSTOM, msgformat, va);
    va_end(va);
}

cException::cException(const cObject *where, OppErrorCode errorcode...) : std::exception()
{
    va_list va;
    va_start(va, errorcode);
    init(where, errorcode, cErrorMessages::get(errorcode), va);
    va_end(va);
}

cException::cException(const cObject *where, const char *msgformat...) : std::exception()
{
    va_list va;
    va_start(va, msgformat);
    init(where, eCUSTOM, msgformat, va);
    va_end(va);
}

cException::cException(const cException& e) : std::exception(e)
{
    errorcode = e.errorcode;
    msg = e.msg;

    simulationstage = e.simulationstage;
    eventnumber = e.eventnumber;
    simtime = e.simtime;

    hascontext = e.hascontext;
    contextclassname = e.contextclassname;
    contextfullpath = e.contextfullpath;
    moduleid = e.moduleid;
}

void cException::storeCtx()
{
    cSimulation *sim = cSimulation::getActiveSimulation();

    if (!sim)
    {
        simulationstage = CTX_NONE;
        eventnumber = 0;
        simtime = SIMTIME_ZERO;
    }
    else
    {
        simulationstage = sim->getSimulationStage();
        eventnumber = sim->getEventNumber();
        simtime = sim->getSimTime();
    }

    if (!sim || !sim->getContext())
    {
        hascontext = false;
        moduleid = -1;
    }
    else
    {
        hascontext = true;
        contextclassname = sim->getContext()->getClassName();
        contextfullpath = sim->getContext()->getFullPath().c_str();
        moduleid = sim->getContextModule() ? sim->getContextModule()->getId() : -1;
    }
}

void cException::exitIfStartupError()
{
    if (!cStaticFlag::isSet())
    {
        // note: ev may not be available at this time
        fprintf(stderr, "<!> Error during startup/shutdown: %s. Aborting.", what());
        abort();
    }
}

void cException::init(const cObject *where, OppErrorCode errorcode, const char *fmt, va_list va)
{
    // store error code
    this->errorcode = errorcode;

    // prefix message with "where" object conditionally, as "(%s)%s:"
    //  - if object is the module itself: skip
    //  - if object is local in module: use getFullName()
    //  - if object is somewhere else: use getFullPath()
    buffer[0] = '\0';
    cSimulation *sim = cSimulation::getActiveSimulation();
    cComponent *context = sim ? sim->getContext() : NULL;
    if (where && where!=context)
    {
        // try: if context's fullpath is same as module fullpath + object fullname, no need to print path
        sprintf(buffer2, "%s.%s", (context ? context->getFullPath().c_str() : ""), where->getFullName());
        bool needpath = strcmp(buffer2,where->getFullPath().c_str())!=0;
        sprintf(buffer, "(%s)%s: ", where->getClassName(), needpath ? where->getFullPath().c_str() : where->getFullName());
    }

    vsnprintf(buffer+strlen(buffer), BUFLEN-strlen(buffer), fmt, va);
    buffer[BUFLEN-1] = '\0';
    msg = buffer;

    // store context
    storeCtx();

    // if a global object's ctor/dtor throws an exception, there won't be
    // anyone around to catch it, so print it and abort here.
    exitIfStartupError();
}

std::string cException::getFormattedMessage() const
{
    std::string when;
    switch (getSimulationStage())
    {
        case CTX_NONE: when = ""; break;
        case CTX_BUILD: when = " during network setup"; break; // note leading spaces
        case CTX_INITIALIZE: when = " during network initialization"; break;
        case CTX_EVENT: when = opp_stringf(" at event #%" LL "d, t=%s",getEventNumber(), SIMTIME_STR(getSimtime())); break; // note we say "at" and not "in", because error may have occurred outside handleMessage()
        case CTX_FINISH: when = " during finalization"; break;
        case CTX_CLEANUP: when = " during network cleanup"; break;
    }

    std::string result;
    if (isError())
    {
        if (!hasContext())
            result = opp_stringf("Error%s: %s.", when.c_str(), what());
        else if (getModuleID()==-1)
            result = opp_stringf("Error in component (%s) %s%s: %s.", getContextClassName(), getContextFullPath(), when.c_str(), what());
        else
            result = opp_stringf("Error in module (%s) %s (id=%d)%s: %s.", getContextClassName(), getContextFullPath(), getModuleID(), when.c_str(), what());
    }
    else
    {
        if (!hasContext())
            result = opp_stringf("%s%s.", what(), when.c_str());
        else if (getModuleID()==-1)
            result = opp_stringf("Component (%s) %s%s: %s.", getContextClassName(), getContextFullPath(), when.c_str(), what());
        else
            result = opp_stringf("Module (%s) %s (id=%d)%s: %s.", getContextClassName(), getContextFullPath(), getModuleID(), when.c_str(), what());
    }

    return result;
}

//---

cTerminationException::cTerminationException(OppErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(NULL, errorcode, cErrorMessages::get(errorcode), va);
    va_end(va);
}

cTerminationException::cTerminationException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(NULL, eCUSTOM, msgformat, va);
    va_end(va);
}

//---

cRuntimeError::cRuntimeError(OppErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(NULL, errorcode, cErrorMessages::get(errorcode), va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(NULL, eCUSTOM, msgformat, va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const cObject *where, OppErrorCode errorcode...)
{
    va_list va;
    va_start(va, errorcode);
    init(where, errorcode, cErrorMessages::get(errorcode), va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

cRuntimeError::cRuntimeError(const cObject *where, const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(where, eCUSTOM, msgformat, va);
    va_end(va);
    breakIntoDebuggerIfRequested();
}

void cRuntimeError::breakIntoDebuggerIfRequested()
{
    if (ev.attach_debugger_on_errors)
    {
        ev.attachDebugger();
    }
    else if (ev.debug_on_errors)
    {
        printf("\n"
               "RUNTIME ERROR. A cRuntimeError exception is about to be thrown, and you\n"
               "requested (by setting debug-on-errors=true in the ini file) that errors\n"
               "abort execution and break into the debugger.\n\n"
#ifdef _MSC_VER
               "If you see a [Debug] button on the Windows crash dialog and you have\n"
               "just-in-time debugging enabled, select it to get into the Visual Studio\n"
               "debugger. Otherwise, you should already have attached to this process from\n"
               "Visual Studio. Once in the debugger, see you can browse to the context of\n"
               "the error in the \"Call stack\" debug view.\n\n"
#else
               "You should now probably be running the simulation under gdb or another\n"
               "debugger. The simulation kernel will now raise a SIGABRT signal which will\n"
               "get you into the debugger. If you are not running under a debugger, you can\n"
               "still use the core dump for post-mortem debugging. Once in the debugger,\n"
               "view the call stack (in gdb: \"bt\" command) to see the context of the\n"
               "runtime error.\n\n"
#endif
               );

        printf("<!> %s\n", getFormattedMessage().c_str());
        printf("\nTRAPPING on the exception above, due to a debug-on-errors=true configuration option. Is your debugger ready?\n");
        fflush(stdout);

        // cause debugger interrupt or signal
        DEBUG_TRAP;
    }
}

NAMESPACE_END

