//=========================================================================
//  TEXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include "texception.h"

TException::TException(const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    errormsg = messagebuf;
}


