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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>  // for va_list
#include "cexception.h"
#include "csimul.h"
#include "cmodule.h"
#include "errmsg.h"


cException::cException()
{
    errorcode = eCUSTOM;
    storeCtx();
    // 'message' remains empty
}

cException::cException(int errc...)
{
    va_list va;
    va_start(va, errc);
    init(NULL, errc, emsg[errc], va);
    va_end(va);
}

cException::cException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(NULL, eCUSTOM, msgformat, va);
    va_end(va);
}

cException::cException(const cObject *where, int errc...)
{
    va_list va;
    va_start(va, errc);
    init(where, errc, emsg[errc], va);
    va_end(va);
}

cException::cException(const cObject *where, const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    init(where, eCUSTOM, msgformat, va);
    va_end(va);
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

    // assemble message text
    char message[256] = "\0";

    // print "(%s)%s:" conditionally:
    //  - if object is the module itself: skip
    //  - if object is local in module: use fullName()
    //  - if object is somewhere else: use fullPath()
    if (where && where!=simulation.contextModule())
    {
        // try: if module's fullpath is same as module fullpath + object fullname, no need to print path
        char tmp[256];
        sprintf(tmp,"%s.%s",(simulation.contextModule()?simulation.contextModule()->fullPath():""), where->fullName());
        bool needpath = strcmp(tmp,where->fullPath())!=0;
        sprintf(message, "(%s)%s: ", where->className(), needpath ? where->fullPath() : where->fullName());
    }

    vsprintf(message+strlen(message),fmt,va);
    msg = message;

    // store context
    storeCtx();
}

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


