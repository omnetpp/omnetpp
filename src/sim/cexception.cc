//=========================================================================
//  CEXCEPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Exception classes
//
//   Author: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // for exit()
#include <stdarg.h>  // for va_list
#include <signal.h>  // raise()
#include "cexception.h"
#include "csimul.h"
#include "cmodule.h"
#include "errmsg.h"
#include "cenvir.h"
#include "cconfig.h"


#define BUFLEN 1024
static char buffer[BUFLEN];
static char buffer2[BUFLEN];


//
// Helper, called from every cException constructor.
//
// If an exception occurs in initialization code (during construction of
// global objects, before main() is called), there's nobody who could
// catch the error, so it would just cause a program abort.
// Here we handle this case manually: if cException ctor is invoked before
// main() has started, we print the error message and call exit(1).
//
static void exitIfStartupError(cException *e)
{
    if (!cStaticFlag::isSet())
    {
        ev.printfmsg("Error during startup/shutdown: %s. Exiting.", e->message());
        exit(1);
    }
}

cException::cException()
{
    errorcode = eCUSTOM;
    storeCtx();
    // 'message' remains empty
    exitIfStartupError(this);
}

cException::cException(int errc...)
{
    va_list va;
    va_start(va, errc);
    init(NULL, errc, emsg[errc], va);
    va_end(va);
    exitIfStartupError(this);
}

cException::cException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(NULL, eCUSTOM, msgformat, va);
    va_end(va);
    exitIfStartupError(this);
}

cException::cException(const cObject *where, int errc...)
{
    va_list va;
    va_start(va, errc);
    init(where, errc, emsg[errc], va);
    va_end(va);
    exitIfStartupError(this);
}

cException::cException(const cObject *where, const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(where, eCUSTOM, msgformat, va);
    va_end(va);
    exitIfStartupError(this);
}

void cException::storeCtx()
{
    if (!simulation.contextModule())
    {
        moduleid = -1;
    }
    else
    {
        moduleid = simulation.contextModule()->id();
        moduleclassname = simulation.contextModule()->className();
        modulefullpath = simulation.contextModule()->fullPath().c_str();
    }
}

void cException::init(const cObject *where, int errc, const char *fmt, va_list va)
{
    // store error code
    errorcode = errc;

    // print "(%s)%s:" conditionally:
    //  - if object is the module itself: skip
    //  - if object is local in module: use fullName()
    //  - if object is somewhere else: use fullPath()
    buffer[0]='\0';
    if (where && where!=simulation.contextModule())
    {
        // try: if module's fullpath is same as module fullpath + object fullname, no need to print path
        sprintf(buffer2,"%s.%s",(simulation.contextModule()?simulation.contextModule()->fullPath().c_str():""), where->fullName());
        bool needpath = strcmp(buffer2,where->fullPath().c_str())!=0;
        sprintf(buffer, "(%s)%s: ", where->className(), needpath ? where->fullPath().c_str() : where->fullName());
    }

    vsprintf(buffer+strlen(buffer),fmt,va);
    msg = buffer;

    // store context
    storeCtx();

    //*(int*)0 = 0; // debugging aid
}

//---

cTerminationException::cTerminationException(int errc...)
{
    va_list va;
    va_start(va, errc);
    init(NULL, errc, emsg[errc], va);
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

cRuntimeError::cRuntimeError(int errc...)
{
    va_list va;
    va_start(va, errc);
    init(NULL, errc, emsg[errc], va);
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

cRuntimeError::cRuntimeError(const cObject *where, int errc...)
{
    va_list va;
    va_start(va, errc);
    init(where, errc, emsg[errc], va);
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
    if (ev.debug_on_errors)
    {
        printf("\n"
               "RUNTIME ERROR. A cRuntimeError exception is about to be thrown,\n"
               "and you requested (by setting debug-on-errors=true in the ini file)\n"
               "that errors abort execution and break into the debugger.\n"
               " - on Linux or Unix-like systems: you should now probably be running the\n"
               "   simulation under gdb or another debugger. The simulation kernel will now\n"
               "   raise a SIGABRT signal which will get you into the debugger. If you're not\n"
               "   running under a debugger, you can still use the core dump for post-mortem\n"
               "   debugging.\n"
               " - on Windows: your should have a just-in-time debugger (such as\n"
               "   the Visual C++ IDE) enabled. The simulation kernel will now\n"
               "   cause a debugger interrupt to get you into the debugger -- press\n"
               "   the [Debug] button in the dialog that comes up.\n"
               "Once in the debugger, use its \"view stack trace\" command (in gdb: \"bt\")\n"
               "to see the context of the runtime error. See error text below.\n"
               "\n"
               );
        if (moduleID()==-1)
            printf("<!> Error: %s.\n", message());
        else
            printf("<!> Error in module (%s) %s: %s.\n", moduleClassName(), moduleFullPath(),
                   message());
        fflush(stdout);

#ifdef _MSC_VER
        __asm int 3; // debugger interrupt
#else
        raise(6); // raise SIGABRT signal
#endif
    }
}

//---

cStackCleanupException::cStackCleanupException()
{
}

