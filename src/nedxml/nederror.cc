//==========================================================================
// nederror.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   NEDError, NEDInternalError functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "nederror.h"

static bool err = false;

void NEDError(NEDElement *context, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    const char *loc = context ? context->getSourceLocation() : NULL;
    if (loc)
        fprintf(stderr, "%s: error: %s\n", loc, messagebuf);
    else if (context)
        fprintf(stderr, "<%s>: error: %s\n", context->getTagName(), messagebuf);
    else
        fprintf(stderr, "error: %s\n", messagebuf);

    err = true;
}

bool errorsOccurred()
{
    return err;
}

void clearErrors()
{
    err = false;
}


void NEDInternalError(const char *file, int line, NEDElement *context, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    const char *loc = context ? context->getSourceLocation() : NULL;
    if (loc)
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s: %s\n", file, line, loc, messagebuf);
    else if (context)
        fprintf(stderr, "INTERNAL ERROR: %s:%d: <%s>: %s\n", file, line, context->getTagName(), messagebuf);
    else
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s\n", file, line, messagebuf);
    exit(-1);
}

NEDException::NEDException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[1024];
    vsprintf(message,msgformat,va);
    va_end(va);

    errormsg = message;

    //*(char *)0 = 0;
}

