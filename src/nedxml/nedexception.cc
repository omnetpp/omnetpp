//==========================================================================
// nedexception.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "nedexception.h"
#include "nedelement.h"

NAMESPACE_BEGIN

#define VSPRINTF_MESSAGE(formatstring)  \
    va_list va; \
    va_start(va, formatstring); \
    char message[1024]; \
    vsprintf(message, formatstring, va); \
    va_end(va);

NEDException::NEDException(const char *messagefmt...) : std::runtime_error("")
{
    VSPRINTF_MESSAGE(messagefmt);
    errormsg = message;
}

NEDException::NEDException(NEDElement *context, const char *messagefmt...) : std::runtime_error("")
{
    VSPRINTF_MESSAGE(messagefmt);

    const char *loc = context ? context->getSourceLocation() : NULL;
    if (loc)
        errormsg = std::string(message) + ", at " + std::string(loc);
    else if (context)
        errormsg = std::string(context->getTagName()) + ": " + message;
    else
        errormsg = message;
}


NAMESPACE_END

