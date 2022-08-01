//==========================================================================
//  CLOG.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/clog.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cmodule.h"

namespace omnetpp {

OPP_THREAD_LOCAL bool cLog::loggingEnabled = true;
OPP_THREAD_LOCAL LogLevel cLog::logLevel = LOGLEVEL_TRACE;
OPP_THREAD_LOCAL cLog::NoncomponentLogPredicate cLog::noncomponentLogPredicate = &cLog::defaultNoncomponentLogPredicate;
OPP_THREAD_LOCAL cLog::ComponentLogPredicate cLog::componentLogPredicate = &cLog::defaultComponentLogPredicate;

namespace internal {

OPP_THREAD_LOCAL cLogProxy::LogBuffer cLogProxy::buffer;
OPP_THREAD_LOCAL std::ostream cLogProxy::stream(&cLogProxy::buffer);
OPP_THREAD_LOCAL cLogEntry cLogProxy::currentEntry;
OPP_THREAD_LOCAL LogLevel cLogProxy::previousLogLevel = LOGLEVEL_NOTSET;
OPP_THREAD_LOCAL const char *cLogProxy::previousCategory = nullptr;
OPP_THREAD_LOCAL cLogProxy::nullstream cLogProxy::dummyStream;

}

//----

const char *cLog::getLogLevelName(LogLevel logLevel)
{
    switch (logLevel) {
        case LOGLEVEL_OFF: return "OFF";
        case LOGLEVEL_FATAL: return "FATAL";
        case LOGLEVEL_ERROR: return "ERROR";
        case LOGLEVEL_WARN:  return "WARN";
        case LOGLEVEL_INFO:  return "INFO";
        case LOGLEVEL_DETAIL:return "DETAIL";
        case LOGLEVEL_DEBUG: return "DEBUG";
        case LOGLEVEL_TRACE: return "TRACE";
        default: throw cRuntimeError("Unknown log level '%d'", logLevel);
    }
}

LogLevel cLog::resolveLogLevel(const char *name)
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

bool cLog::defaultNoncomponentLogPredicate(const void *object, LogLevel logLevel, const char *category)
{
    // log called from outside cComponent methods, use context component to decide enablement
    cSimulation *simulation = cSimulation::getActiveSimulation();
    const cModule *contextModule = simulation->getContextModule();
    return isLoggingEnabled() && logLevel >= cLog::logLevel && (!contextModule || logLevel >= contextModule->getLogLevel());
}

bool cLog::defaultComponentLogPredicate(const cComponent *sourceComponent, LogLevel logLevel, const char *category)
{
    // log called from a cComponent method, check whether logging for that component is enabled
    return isLoggingEnabled() && logLevel >= cLog::logLevel && logLevel >= sourceComponent->getLogLevel();
}

//----

namespace internal {

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
                cSimulation::getActiveEnvir()->log(&cLogProxy::currentEntry);
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

//----

cLogProxy::cLogProxy(const void *sourcePointer, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction)
{
    fillEntry(logLevel, category, sourceFile, sourceLine, sourceFunction);
    currentEntry.sourcePointer = sourcePointer;
    currentEntry.sourceObject = currentEntry.sourceComponent = nullptr;
}

cLogProxy::cLogProxy(const cObject *sourceObject, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction)
{
    fillEntry(logLevel, category, sourceFile, sourceLine, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = const_cast<cObject *>(sourceObject);
    currentEntry.sourceComponent = nullptr;
}

cLogProxy::cLogProxy(const cComponent *sourceComponent, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction)
{
    fillEntry(logLevel, category, sourceFile, sourceLine, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = currentEntry.sourceComponent = const_cast<cComponent *>(sourceComponent);
}

cLogProxy::~cLogProxy()
{
    stream.flush();
    previousLogLevel = currentEntry.logLevel;
    previousCategory = currentEntry.category;
}

void cLogProxy::fillEntry(LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction)
{
    if (previousLogLevel != logLevel || (previousCategory != category && strcmp(previousCategory ? previousCategory : "", category ? category : "")))
        flushLastLine();
    currentEntry.category = category;
    currentEntry.logLevel = logLevel;
    currentEntry.sourceFile = sourceFile;
    currentEntry.sourceLine = sourceLine;
    currentEntry.sourceFunction = sourceFunction;
    currentEntry.userTime = clock();
}

void cLogProxy::flushLastLine()
{
    if (!buffer.isEmpty()) {
        stream.put('\n');
        stream.flush();
    }
}

}  // namespace internal

}  // namespace omnetpp

