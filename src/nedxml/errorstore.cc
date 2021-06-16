//==========================================================================
// errorstore.cc -
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
#include "errorstore.h"
#include "astnode.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

void ErrorStore::doAdd(ASTNode *context, const char *loc, int severity, const char *message)
{
    entries.push_back(Entry());
    Entry& e = entries.back();

    e.context = context;
    e.location = loc ? loc : context ? context->getSourceLocation().str() : "";
    e.severity = severity;
    e.message = message;

    if (doprint) {
        const char *severitytext = severityName(severity);
        if (!e.location.empty())
            fprintf(stderr, "%s: %s: %s\n", e.location.c_str(), severitytext, message);
        else if (context)
            fprintf(stderr, "<%s>: %s: %s\n", context->getTagName(), severitytext, message);
        else
            fprintf(stderr, "%s: %s\n", severitytext, message);
    }
}

#define BUFLEN    1024

void ErrorStore::addError(ASTNode *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, SEVERITY_ERROR, message);
}

void ErrorStore::addError(const char *location, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, SEVERITY_ERROR, message);
}

void ErrorStore::addWarning(ASTNode *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, SEVERITY_WARNING, message);
}

void ErrorStore::addWarning(const char *location, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, SEVERITY_WARNING, message);
}

void ErrorStore::add(ASTNode *context, int severity, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(context, nullptr, severity, message);
}

void ErrorStore::add(const char *location, int severity, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);
    doAdd(nullptr, location, severity, message);
}

bool ErrorStore::containsError() const
{
    for (const auto & entry : entries)
        if (entry.severity == SEVERITY_ERROR)
            return true;

    return false;
}

const char *ErrorStore::errorSeverity(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return severityName(entries[i].severity);
}

int ErrorStore::errorSeverityCode(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return -1;
    return entries[i].severity;
}

const char *ErrorStore::errorLocation(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].location.c_str();
}

ASTNode *ErrorStore::errorContext(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].context;
}

const char *ErrorStore::errorText(int i) const
{
    if (i < 0 || i >= (int)entries.size())
        return nullptr;
    return entries[i].message.c_str();
}

int ErrorStore::findFirstErrorFor(ASTNode *node, int startIndex) const
{
    for (int i = startIndex; i < (int)entries.size(); i++)
        if (entries[i].context == node)
            return i;

    return -1;
}

const char *ErrorStore::severityName(int severity)
{
    switch (severity) {
        case SEVERITY_INFO:    return "Info";
        case SEVERITY_WARNING: return "Warning";
        case SEVERITY_ERROR:   return "Error";
        default:               return "???";
    }
}

//---

void NedInternalError(const char *file, int line, ASTNode *context, const char *messagefmt, ...)
{
    char message[BUFLEN];
    VSNPRINTF(message, BUFLEN, messagefmt);

    std::string loc = context ? context->getSourceLocation().str() : "";
    if (!loc.empty())
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s: %s\n", file, line, loc.c_str(), message);
    else if (context)
        fprintf(stderr, "INTERNAL ERROR: %s:%d: <%s>: %s\n", file, line, context->getTagName(), message);
    else
        fprintf(stderr, "INTERNAL ERROR: %s:%d: %s\n", file, line, message);
}

}  // namespace nedxml
}  // namespace omnetpp

