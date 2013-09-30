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
                addPart(LOGDIRECTIVE_CONSTANTTEXT, previous, current);
            break;
        }
        else if (ch == '%')
        {
            if (previous != current)
                addPart(LOGDIRECTIVE_CONSTANTTEXT, previous, current);
            previous = current;
            current++;
            ch = *current;
            if (ch == '%')
                addPart(LOGDIRECTIVE_CONSTANTTEXT, previous, current);
            else
                addPart(getDirective(ch), NULL, NULL);
            previous = current + 1;
        }
        current++;
    }
}

LogFormatter::FormatDirective LogFormatter::getDirective(char ch)
{
    if (strchr("lcetvanmosqNMOSQGRXYZpbdxyzufiwWH", ch) == NULL)
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
            case LOGDIRECTIVE_CONSTANTTEXT:
                stream << part.text; break;

            // log statement related
            case LOGDIRECTIVE_LOGLEVEL:
                stream << cLogLevel::getName(entry->loglevel); break;
            case LOGDIRECTIVE_LOGCATEGORY:
                stream << (entry->category ? entry->category : ""); break;

            // current simulation state related
            case LOGDIRECTIVE_CURRENTEVENTNUMBER:
                stream << simulation.getEventNumber(); break;
            case LOGDIRECTIVE_CURRENTSIMULATIONTIME:
                stream << simulation.getSimTime(); break;

            case LOGDIRECTIVE_CURRENTEVENTNAME:
                if (ev.getCurrentEventName()) stream << ev.getCurrentEventName(); break;
            case LOGDIRECTIVE_CURRENTEVENTCLASSNAME:
                if (ev.getCurrentEventClassName()) stream << ev.getCurrentEventClassName(); break;

            case LOGDIRECTIVE_CURRENTMODULENAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getFullName(); break;
            case LOGDIRECTIVE_CURRENTMODULEPATH:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getFullPath(); break;
            case LOGDIRECTIVE_CURRENTMODULECLASSNAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getClassName(); break;
            case LOGDIRECTIVE_CURRENTMODULENEDTYPESIMPLENAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getComponentType()->getName(); break;
            case LOGDIRECTIVE_CURRENTMODULENEDTYPEQUALIFIEDNAME:
                if (ev.getCurrentEventModule()) stream << ev.getCurrentEventModule()->getComponentType()->getFullName(); break;

            case LOGDIRECTIVE_CONTEXTMODULENAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getFullName(); break;
            case LOGDIRECTIVE_CONTEXTMODULEPATH:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getFullPath(); break;
            case LOGDIRECTIVE_CONTEXTMODULECLASSNAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getClassName(); break;
            case LOGDIRECTIVE_CONTEXTMODULENEDTYPESIMPLENAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getComponentType()->getName(); break;
            case LOGDIRECTIVE_CONTEXTMODULENEDTYPEQUALIFIEDNAME:
                if (simulation.getContextModule()) stream << simulation.getContextModule()->getComponentType()->getFullName(); break;

            // simulation run related
            case LOGDIRECTIVE_CONFIGNAME:
                stream << simulation.getActiveEnvir()->getConfigEx()->getActiveConfigName(); break;
            case LOGDIRECTIVE_RUNNUMBER:
                stream << simulation.getActiveEnvir()->getConfigEx()->getActiveRunNumber(); break;

            case LOGDIRECTIVE_NETWORKMODULECLASSNAME:
                stream << simulation.getSystemModule()->getClassName(); break;
            case LOGDIRECTIVE_NETWORKMODUENEDTYPESIMPLENAME:
                stream << simulation.getNetworkType()->getName(); break;
            case LOGDIRECTIVE_NETWORKMODUENEDTYPEQUALIFIEDNAME:
                stream << simulation.getNetworkType()->getFullName(); break;

            // C++ source related
            case LOGDIRECTIVE_SOURCEOBJECTPOINTER:
                if (entry->sourcePointer) stream << entry->sourcePointer; break;
            case LOGDIRECTIVE_SOURCEOBJECTNAME:
                if (entry->sourceObject) stream << entry->sourceObject->getFullName(); break;
            case LOGDIRECTIVE_SOURCEOBJECTPATH:
                if (entry->sourceObject) stream << entry->sourceObject->getFullPath(); break;

            case LOGDIRECTIVE_SOURCECOMPONENTNEDSIMPLENAME:
                if (entry->sourceComponent) stream << entry->sourceComponent->getComponentType()->getName(); break;
            case LOGDIRECTIVE_SOURCECOMPONENTNEDQUALIFIEDNAME:
                if (entry->sourceComponent) stream << entry->sourceComponent->getComponentType()->getFullName(); break;

            case LOGDIRECTIVE_SOURCEFILE:
                stream << entry->sourceFile; break;
            case LOGDIRECTIVE_SOURCELINE:
                stream << entry->sourceLine; break;

            case LOGDIRECTIVE_SOURCEOBJECTCLASSNAME:
                stream << (entry->sourceComponent ? entry->sourceComponent->getComponentType()->getName() :
                          (entry->sourceObject ? entry->sourceObject->getClassName() :
                          (entry->sourceClass ? opp_demangle_typename(entry->sourceClass) : ""))); break;
            case LOGDIRECTIVE_SOURCEFUNCTION:
                stream << entry->sourceFunction; break;

            // operating system related
            case LOGDIRECTIVE_USERTIME:
                stream << (double)entry->userTime / (double)CLOCKS_PER_SEC; break;
            case LOGDIRECTIVE_WALLTIME:
            {
                // chop off newline at the end (no worries, this is slow anyway)
                time_t now = time(NULL);
                std::string nowstr(ctime(&now));
                stream << nowstr.substr(0, nowstr.length() - 1); break;
            }

            case LOGDIRECTIVE_HOSTNAME:
                stream << opp_gethostname(); break;
            case LOGDIRECTIVE_PROCESSID:
                stream << getpid(); break;

            default:
                throw cRuntimeError("Unknown format directive '%d'", part.directive);
        }
    }
    return stream.str();
}
