//=========================================================================
//
//  CEXCEPTION.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Exception classes
//
//   Author: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // for exit()
#include <stdarg.h>  // for va_list
#include "cexception.h"
#include "csimul.h"
#include "cmodule.h"
#include "errmsg.h"
#include "cenvir.h"


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
        modulefullpath = simulation.contextModule()->fullPath();
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
        sprintf(buffer2,"%s.%s",(simulation.contextModule()?simulation.contextModule()->fullPath():""), where->fullName());
        bool needpath = strcmp(buffer2,where->fullPath())!=0;
        sprintf(buffer, "(%s)%s: ", where->className(), needpath ? where->fullPath() : where->fullName());
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

cStackCleanupException::cStackCleanupException()
{
}

