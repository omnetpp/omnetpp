//==========================================================================
// nederror.cc  - part of the OMNeT++ Discrete System Simulation System
//
// Contents:
//   nedError function
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include "nederror.h"

void NEDError(NEDElement *where, const char *format, ...)
{
    va_list va;
    va_start(va, format);
    char message[1024];
    vsprintf(message,format,va);
    va_end(va);

    const char *loc = where ? where->getSourceLocation() : NULL;
    if (loc)
        fprintf(stderr, "%s: %s\n", loc, message);
    else if (where)
        fprintf(stderr, "<%s>: %s\n", where->getTagName(), message);
    else
        fprintf(stderr, "%s\n", message);
}



