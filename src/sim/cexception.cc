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
#include "csimul.h"  // cSimulation
#include "errmsg.h"

cException::cException(int errc...)
{
    va_list va;
    va_start(va, errc);
    char message[256];
    vsprintf(message,emsg[errc],va);
    va_end(va);

    errorcode = errc;
    errormsg = message;
    module = simulation.contextModule();
}

cException::cException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[256];
    vsprintf(message,msgformat,va);
    va_end(va);

    errorcode = eCUSTOM;
    errormsg = message;
    module = simulation.contextModule();
}
