//==========================================================================
//  LOGFORMATTER.CC - part of
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

#include "logformatter.h"
#include "commonutil.h"
#include "cmodule.h"
#include "ccomponenttype.h"
#include "cconfiguration.h"

LogFormatter::LogFormatter(const char *format)
{
    parseFormat(format);
}

void LogFormatter::parseFormat(const char *format)
{
    char *current = const_cast<char *>(format);
    char *previous = current;
    while (true)
    {
        char ch = *current;
        if (ch == '\0')
        {
            if (previous != current)
                addPart(CONSTANT_TEXT, previous, current);
            break;
        }
        else if (ch == '%')
        {
            if (previous != current)
                addPart(CONSTANT_TEXT, previous, current);
            previous = current;
            current++;
            ch = *current;
            if (ch == '%')
                addPart(CONSTANT_TEXT, previous, current);
            else
                addPart(getDirective(ch), NULL, NULL);
            previous = current + 1;
        }
        current++;
    }
}

LogFormatter::FormatDirective LogFormatter::getDirective(char ch)
{
    if (strchr("lcetvanmosqNMOSQGRXYZpbdxyzufiwWHI", ch) == NULL)
        throw cRuntimeError("Unknown log format character '%c'", ch);
    return (LogFormatter::FormatDirective) ch;
}

void LogFormatter::addPart(FormatDirective directive, char *textBegin, char *textEnd)
{
    FormatPart part;
    part.directive = directive;
    if (textBegin && textEnd)
        part.text = std::string(textBegin, textEnd - textBegin);
    formatParts.push_back(part);
}

std::string LogFormatter::formatPrefix(cLogEntry *entry)
{
    std::stringstream stream;
    for (std::vector<FormatPart>::iterator it = formatParts.begin(); it != formatParts.end(); it++)
    {
        FormatPart& part = *it;
        switch (part.directive)
        {
            case CONSTANT_TEXT:
                stream << part.text; break;

            // log statement related
            case LOGLEVEL:
                stream << cLogLevel::getName(entry->loglevel); break;
            case LOGCATEGORY:
                stream << (entry->category ? entry->category : ""); break;

            // current simulation state related
            case CURRENT_EVENT_NUMBER:
                stream << simulation.getEventNumber(); break;
            case CURRENT_SIMULATION_TIME:
                stream << simulation.getSimTime(); break;

            case CURRENT_EVENT_NAME:
                if (ev.getCurrentEventName()) stream << ev.getCurrentEventName(); break;
            case CURRENT_EVENT_CLASSNAME:
                if (ev.getCurrentEventClassName()) stream << ev.getCurrentEventClassName(); break;

            case CURRENT_MODULE_NAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getFullName(); break;
            case CURRENT_MODULE_FULLPATH:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getFullPath(); break;
            case CURRENT_MODULE_CLASSNAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getClassName(); break;
            case CURRENT_MODULE_NEDTYPE_SIMPLENAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getComponentType()->getName(); break;
            case CURRENT_MODULE_NEDTYPE_QUALIFIEDNAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getComponentType()->getFullName(); break;

            case CONTEXT_MODULE_NAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getFullName(); break;
            case CONTEXT_MODULE_FULLPATH:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getFullPath(); break;
            case CONTEXT_MODULE_CLASSNAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getClassName(); break;
            case CONTEXT_MODULE_NEDTYPE_SIMPLENAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getComponentType()->getName(); break;
            case CONTEXT_MODULE_NEDTYPE_QUALIFIEDNAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getComponentType()->getFullName(); break;

            // simulation run related
            case CONFIGNAME:
                stream << simulation.getActiveEnvir()->getConfigEx()->getActiveConfigName(); break;
            case RUNNUMBER:
                stream << simulation.getActiveEnvir()->getConfigEx()->getActiveRunNumber(); break;

            case NETWORK_MODULE_CLASSNAME:
                stream << simulation.getSystemModule()->getClassName(); break;
            case NETWORK_MODULE_NEDTYPE_SIMPLENAME:
                stream << simulation.getNetworkType()->getName(); break;
            case NETWORK_MODULE_NEDTYPE_QUALIFIEDNAME:
                stream << simulation.getNetworkType()->getFullName(); break;

            // C++ source related
            case SOURCE_OBJECT_POINTER:
                if (entry->sourcePointer) stream << entry->sourcePointer; break;
            case SOURCE_OBJECT_NAME:
                if (entry->sourceObject) stream << entry->sourceObject->getFullName(); break;
            case SOURCE_OBJECT_FULLPATH:
                if (entry->sourceObject) stream << entry->sourceObject->getFullPath(); break;

            case SOURCE_COMPONENT_NEDTYPE_SIMPLENAME:
                if (entry->sourceComponent) stream << entry->sourceComponent->getComponentType()->getName(); break;
            case SOURCE_COMPONENT_NEDTYPE_QUALIFIEDNAME:
                if (entry->sourceComponent) stream << entry->sourceComponent->getComponentType()->getFullName(); break;

            case SOURCE_FILE:
                stream << entry->sourceFile; break;
            case SOURCE_LINE:
                stream << entry->sourceLine; break;

            case SOURCE_OBJECT_CLASSNAME:
                stream << (entry->sourceComponent ? entry->sourceComponent->getComponentType()->getName() :
                          (entry->sourceObject ? entry->sourceObject->getClassName() :
                          (entry->sourceClass ? opp_demangle_typename(entry->sourceClass) : ""))); break;
            case SOURCE_FUNCTION:
                stream << entry->sourceFunction; break;

            // operating system related
            case USERTIME:
                stream << (double)entry->userTime / (double)CLOCKS_PER_SEC; break;
            case WALLTIME:
            {
                // chop off newline at the end (no worries, this is slow anyway)
                time_t now = time(NULL);
                std::string nowstr(ctime(&now));
                stream << nowstr.substr(0, nowstr.length() - 1); break;
            }

            case HOSTNAME:
                stream << opp_gethostname(); break;
            case PROCESSID:
                stream << getpid(); break;

            default:
                throw cRuntimeError("Unknown format directive '%d'", part.directive);
        }
    }
    return stream.str();
}
