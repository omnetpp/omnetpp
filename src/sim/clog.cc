//==========================================================================
//  CLOG.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/clog.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cmodule.h"

namespace omnetpp {

cLogProxy::LogBuffer cLogProxy::buffer;
std::ostream cLogProxy::stream(&cLogProxy::buffer);
LogLevel cLog::loglevel = LOGLEVEL_DEBUG;
cLogEntry cLogProxy::currentEntry;
LogLevel cLogProxy::previousLoglevel = (LogLevel)-1;
const char *cLogProxy::previousCategory = nullptr;
cLogProxy::nullstream cLogProxy::dummyStream;

//----

const char *cLog::getName(LogLevel loglevel)
{
    switch (loglevel) {
        case LOGLEVEL_OFF: return "OFF";
        case LOGLEVEL_FATAL: return "FATAL";
        case LOGLEVEL_ERROR: return "ERROR";
        case LOGLEVEL_WARN:  return "WARN";
        case LOGLEVEL_INFO:  return "INFO";
        case LOGLEVEL_DETAIL:return "DETAIL";
        case LOGLEVEL_DEBUG: return "DEBUG";
        case LOGLEVEL_TRACE: return "TRACE";
        default: throw cRuntimeError("Unknown log level '%d'", loglevel);
    }
}

LogLevel cLog::getLevel(const char *name)
{
    if (!strcasecmp(name, "OFF"))
        return LOGLEVEL_OFF;
    else if (!strcasecmp(name, "FATAL"))
        return LOGLEVEL_FATAL;
    else if (!strcasecmp(name, "ERROR"))
        return LOGLEVEL_ERROR;
    else if (!strcasecmp(name, "WARN"))
        return LOGLEVEL_WARN;
    else if (!strcasecmp(name, "INFO"))
        return LOGLEVEL_INFO;
    else if (!strcasecmp(name, "DETAIL"))
        return LOGLEVEL_DETAIL;
    else if (!strcasecmp(name, "DEBUG"))
        return LOGLEVEL_DEBUG;
    else if (!strcasecmp(name, "TRACE"))
        return LOGLEVEL_TRACE;
    else
        throw cRuntimeError("Unknown log level name '%s'", name);
}

//----

int cLogProxy::LogBuffer::sync()
{
    char *text = pbase();
    int size = pptr() - pbase();
    if (size != 0) {
        char *end = text + size;
        for (char *s = text; s != end; s++) {
            if (*s == '\n') {
                cLogProxy::currentEntry.text = text;
                cLogProxy::currentEntry.textLength = s - text + 1;
                getEnvir()->log(&cLogProxy::currentEntry);
                text = s + 1;
            }
        }
        setp(pbase(), epptr());
        if (end != text) {
            memmove(pbase(), text, end - text);
            pbump(end - text);
        }
    }
    return 0;
}

void cLogProxy::flushLastLine()
{
    if (!buffer.isEmpty()) {
        stream.put('\n');
        stream.flush();
    }
}

//----

cLogProxy::cLogProxy(const void *sourcePointer, LogLevel loglevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(loglevel, category, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = sourcePointer;
    currentEntry.sourceObject = currentEntry.sourceComponent = nullptr;
}

cLogProxy::cLogProxy(const cObject *sourceObject, LogLevel loglevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(loglevel, category, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = const_cast<cObject *>(sourceObject);
    currentEntry.sourceComponent = nullptr;
}

cLogProxy::cLogProxy(const cComponent *sourceComponent, LogLevel loglevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(loglevel, category, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = currentEntry.sourceComponent = const_cast<cComponent *>(sourceComponent);
}

cLogProxy::~cLogProxy()
{
    stream.flush();
    previousLoglevel = currentEntry.loglevel;
    previousCategory = currentEntry.category;
}

void cLogProxy::fillEntry(LogLevel loglevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    if (previousLoglevel != loglevel || (previousCategory != category && strcmp(previousCategory ? previousCategory : "", category ? category : "")))
        flushLastLine();
    currentEntry.category = category;
    currentEntry.loglevel = loglevel;
    currentEntry.sourceFile = sourceFile;
    currentEntry.sourceLine = sourceLine;
    currentEntry.sourceClass = sourceClass;
    currentEntry.sourceFunction = sourceFunction;
    currentEntry.userTime = clock();
}

cLog::NoncomponentLogPredicate cLog::noncomponentLogPredicate = &cLog::defaultNoncomponentLogPredicate;
cLog::ComponentLogPredicate cLog::componentLogPredicate = &cLog::defaultComponentLogPredicate;

bool cLog::defaultNoncomponentLogPredicate(const void *object, LogLevel loglevel, const char *category)
{
    // log called from outside cComponent methods, use context component to decide enablement
    const cModule *contextModule = getSimulation()->getContextModule();
    return loglevel >= cLog::loglevel &&
           (!contextModule || loglevel >= contextModule->getLoglevel()) &&
           getEnvir()->isLoggingEnabled();
}

bool cLog::defaultComponentLogPredicate(const cComponent *sourceComponent, LogLevel loglevel, const char *category)
{
    // log called from a cComponent method, check whether logging for that component is enabled
    return loglevel >= cLog::loglevel &&
           loglevel >= sourceComponent->getLoglevel() &&
           getEnvir()->isLoggingEnabled();
}

}  // namespace omnetpp

