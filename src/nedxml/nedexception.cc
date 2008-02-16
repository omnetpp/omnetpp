//==========================================================================
// nedexception.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "nedexception.h"
#include "nedelement.h"

NAMESPACE_BEGIN

#define DO_VSPRINTF()  \
    va_list va; \
    va_start(va, message); \
    char messagebuf[1024]; \
    vsprintf(messagebuf,message,va); \
    va_end(va);

NEDException::NEDException(const char *message...) : std::runtime_error("")
{
    DO_VSPRINTF();
    errormsg = message;
}

NEDException::NEDException(NEDElement *context, const char *message...) : std::runtime_error("")
{
    DO_VSPRINTF();

    const char *loc = context ? context->getSourceLocation() : NULL;
    if (loc)
        errormsg = std::string(loc) + ": " + message;
    else if (context)
        errormsg = std::string(context->getTagName()) + ": " + message;
    else
        errormsg = message;
}


NAMESPACE_END

