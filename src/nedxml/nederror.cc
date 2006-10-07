//==========================================================================
// nederror.cc -
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
#include "nederror.h"


void NEDErrorStore::doAdd(NEDElement *context, const char *loc, int category, const char *message)
{
    entries.push_back(Entry());
    Entry& e = entries.back();

    if (!loc && context)
        loc = context->getSourceLocation();

    e.context = context;
    e.location = loc ? loc : "";
    e.category = category;
    e.message = message;

    if (doprint)
    {
        if (loc)
            fprintf(stderr, "%s: error: %s\n", loc, message);
        else if (context)
            fprintf(stderr, "<%s>: error: %s\n", context->getTagName(), message);
        else
            fprintf(stderr, "error: %s\n", message);
   }
}

void NEDErrorStore::add(NEDElement *context, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    doAdd(context, NULL, ERRCAT_ERROR, messagebuf);
}

void NEDErrorStore::add(NEDElement *context, int category, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    doAdd(context, NULL, category, messagebuf);
}

void NEDErrorStore::add(int category, const char *location, const char *message, ...)
{
    va_list va;
    va_start(va, message);
    char messagebuf[1024];
    vsprintf(messagebuf,message,va);
    va_end(va);

    doAdd(NULL, location, category, messagebuf);
}

const char *NEDErrorStore::errorCategory(int i) const
{
    if (i<0 || i>=entries.size()) return NULL;
    return categoryName(entries[i].category);
}

int NEDErrorStore::errorCategoryCode(int i) const
{
    if (i<0 || i>=entries.size()) return -1;
    return entries[i].category;
}

const char *NEDErrorStore::errorLocation(int i) const
{
    if (i<0 || i>=entries.size()) return NULL;
    return entries[i].location.c_str();
}

NEDElement *NEDErrorStore::errorContext(int i) const
{
    if (i<0 || i>=entries.size()) return NULL;
    return entries[i].context;
}

const char *NEDErrorStore::errorText(int i) const
{
    if (i<0 || i>=entries.size()) return NULL;
    return entries[i].message.c_str();
}

const char *NEDErrorStore::categoryName(int cat)
{
    switch (cat)
    {
        case ERRCAT_INFO:    return "Info";
        case ERRCAT_WARNING: return "Warning";
        case ERRCAT_ERROR:   return "Error";
        case ERRCAT_FATAL:   return "Fatal";
        default:             return "???";
    }
}

//---

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
    // exit(-1);
    //__asm int 3; //FIXME this windows-only
}

NEDException::NEDException(const char *msgformat...)
{
    va_list va;
    va_start(va, msgformat);
    char message[1024];
    vsprintf(message,msgformat,va);
    va_end(va);

    errormsg = message;
}

