//==========================================================================
// nederror.cc -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include "common/commonutil.h"
#include "nederror.h"
#include "nedelement.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

void NEDErrorStore::doAdd(NEDElement *context, const char *loc, int severity, const char *message)
{
    entries.push_back(Entry());
    Entry& e = entries.back();

    if (!loc && context)
        loc = context->getSourceLocation();

    e.context = context;
    e.location = loc ? loc : "";
    e.severity = severity;
    e.message = message;

    if (doprint) {
        const char *severitytext = severityName(severity);
        if (loc && !loc[0])
            fprintf(stderr, "%s: %s\n", severitytext, message);
        else if (loc)
            fprintf(stderr, "%s: %s: %s\n", loc, severitytext, message);
        else if (context)
            fprintf(stderr, "<%s>: %s: %s\n", context->getTagName(), severitytext, message);
        else
            fprintf(stderr, "%s: %s\n", severitytext, message);
    }
}

#define BUFLEN    1024

void NEDErrorStore::addError(NEDElement *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, NED_SEVERITY_ERROR, message);
}

void NEDErrorStore::addError(const char *location, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, NED_SEVERITY_ERROR, message);
}

void NEDErrorStore::addWarning(NEDElement *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, NED_SEVERITY_WARNING, message);
}

void NEDErrorStore::addWarning(const char *location, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, NED_SEVERITY_WARNING, message);
}

void NEDErrorStore::add(NEDElement *context, int severity, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, severity, message);
}

void NEDErrorStore::add(const char *location, int severity, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, severity, message);
}

bool NEDErrorStore::containsError() const
{
    for (const auto & entry : entries)
        if (entry.severity == NED_SEVERITY_ERROR)
            return true;

    return false;
}

const char *NEDErrorStore::errorSeverity(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return severityName(entries[i].severity);
}

int NEDErrorStore::errorSeverityCode(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return -1;
    return entries[i].severity;
}

const char *NEDErrorStore::errorLocation(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].location.c_str();
}

NEDElement *NEDErrorStore::errorContext(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].context;
}

const char *NEDErrorStore::errorText(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].message.c_str();
}

int NEDErrorStore::findFirstErrorFor(NEDElement *node, int startIndex) const
{
    for (int i = startIndex; i < (int)entries.size(); i++)
        if (entries[i].context == node)
            return i;

    return -1;
}

const char *NEDErrorStore::severityName(int severity)
{
    switch (severity) {
        case NED_SEVERITY_INFO:    return "Info";
        case NED_SEVERITY_WARNING: return "Warning";
        case NED_SEVERITY_ERROR:   return "Error";
        default:                   return "???";
    }
}

//---

void NEDInternalError(const char *file, int line, NEDElement *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);

    const char *loc = context ? context->getSourceLocation() : nullptr;
    if (loc)
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s: %s\n", file, line, loc, message);
    else if (context)
        fprintf(stderr, "INTERNAL ERROR: %s:%d: <%s>: %s\n", file, line, context->getTagName(), message);
    else
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s\n", file, line, message);
    // exit(-1);
    // __asm int 3; //FIXME this windows-only
}

}  // namespace nedxml
}  // namespace omnetpp

