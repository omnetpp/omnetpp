//=========================================================================
//  SCAVEEXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include "scaveexception.h"


ResultFileFormatException::ResultFileFormatException(const char *message, const char *file, int line)
    : opp_runtime_error(""), file(file), line(line)
{
    char messagebuf[1024];
    sprintf(messagebuf, "%s, file %s, line %d", message, file, line);
    errormsg = messagebuf;
}
