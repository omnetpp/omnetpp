//=========================================================================
//
//  EXCEPTION.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Exception class
//
//
//   Author: Andras Varga
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "cexception.h"
#include "csimul.h" 
#include "cmodule.h" 
#include "errmsg.h"

cException::cException(int errc...)
{
    va_list va;
    va_start(va, errc);
    char message[256];
    vsprintf(message,emsg[errc],va);
    va_end(va);

    errorcode = errc;
    msg = message;

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

cException::cException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[256];
    vsprintf(message,msgformat,va);
    va_end(va);

    errorcode = eCUSTOM;
    msg = message;

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

bool cException::isNormalTermination() const
{
    return errorcode==eSTOPSIMRCVD // stopped by another PVM segment
        || errorcode==eENDEDOK     // no more events
        || errorcode==eFINISH      // 'finish simulation' was requested interactively
        || errorcode==eENDSIM      // endSimulation() called
        || errorcode==eREALTIME    // execution time limit
        || errorcode==eSIMTIME;    // sim. time limit
}

