//==========================================================================
//  CLOG.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "clog.h"
#include "globals.h"
#include "cconfiguration.h"
#include "cconfigoption.h"
#include "cmodule.h"

NAMESPACE_BEGIN

Register_PerObjectConfigOption(CFGID_COMPONENT_LOGLEVEL, "log-level", KIND_MODULE, CFG_STRING, "DEBUG", "Specifies the per-component level of detail recorded by log statements, output below the specified level is omitted. Available values are (case insensitive): fatal, error, warn, info, detail, debug or trace. Note that the level of detail is also controlled by the globally specified runtime log level and the GLOBAL_COMPILETIME_LOGLEVEL macro that is used to completely remove log statements from the executable.")

cLogProxy::LogBuffer cLogProxy::buffer;
std::ostream cLogProxy::globalStream(&cLogProxy::buffer);
LogLevel cLogLevel::globalRuntimeLoglevel = LOGLEVEL_DEBUG;
cLogEntry cLogProxy::currentEntry;
LogLevel cLogProxy::previousLoglevel = (LogLevel)-1;
const char *cLogProxy::previousCategory = NULL;
cLogProxy::nullstream cLogProxy::dummyStream;

//----

const char *cLogLevel::getName(LogLevel loglevel)
{
    switch (loglevel)
    {
        case LOGLEVEL_FATAL:
            return "FATAL";
        case LOGLEVEL_ERROR:
            return "ERROR";
        case LOGLEVEL_WARN:
            return "WARN";
        case LOGLEVEL_INFO:
            return "INFO";
        case LOGLEVEL_DETAIL:
            return "DETAIL";
        case LOGLEVEL_DEBUG:
            return "DEBUG";
        case LOGLEVEL_TRACE:
            return "TRACE";
        default:
            throw cRuntimeError("Unknown log level '%d'", loglevel);
     }
}

LogLevel cLogLevel::getLevel(const char *name)
{
    if (!strcasecmp(name, "FATAL"))
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
    if (size != 0)
    {
        char *end = text + size;
        for (char *s = text; s != end; s++) {
            if (*s == '\n') {
                cLogProxy::currentEntry.text = text;
                cLogProxy::currentEntry.textLength = s - text + 1;
                ev.log(&cLogProxy::currentEntry);
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
    if (!buffer.isEmpty())
    {
        globalStream.put('\n');
        globalStream.flush();
    }
}

//----

cLogProxy::cLogProxy(const void *sourcePointer, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(category, loglevel, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = sourcePointer;
    currentEntry.sourceObject = currentEntry.sourceComponent = NULL;
}

cLogProxy::cLogProxy(const cObject *sourceObject, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(category, loglevel, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = const_cast<cObject *>(sourceObject);
    currentEntry.sourceComponent = NULL;
}

cLogProxy::cLogProxy(const cComponent *sourceComponent, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
{
    fillEntry(category, loglevel, sourceFile, sourceLine, sourceClass, sourceFunction);
    currentEntry.sourcePointer = currentEntry.sourceObject = currentEntry.sourceComponent = const_cast<cComponent *>(sourceComponent);
}

cLogProxy::~cLogProxy()
{
    globalStream.flush();
    previousLoglevel = currentEntry.loglevel;
    previousCategory = currentEntry.category;
}

bool cLogProxy::isEnabled(const void *sourceObject, const char *category, LogLevel loglevel)
{
    const cModule *contextModule = simulation.getContextModule();
    return !contextModule || isComponentEnabled(contextModule, category, loglevel);
}

bool cLogProxy::isEnabled(const cComponent *sourceComponent, const char *category, LogLevel loglevel)
{
    return isComponentEnabled(sourceComponent, category, loglevel);
}

bool cLogProxy::isComponentEnabled(const cComponent *component, const char *category, LogLevel loglevel)
{
    LogLevel componentLoglevel = component->getLoglevel();
    if (componentLoglevel == -1)
    {
        componentLoglevel = cLogLevel::getLevel(ev.getConfig()->getAsString(component->getFullPath().c_str(), CFGID_COMPONENT_LOGLEVEL).c_str());
        // NOTE: this is just a cache
        const_cast<cComponent *>(component)->setLoglevel(componentLoglevel);
    }
    return loglevel <= componentLoglevel;
}

void cLogProxy::fillEntry(const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction)
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

NAMESPACE_END