//==========================================================================
//  LOG.CC - part of
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

#include "log.h"
#include "commonutil.h"
#include "cmodule.h"
#include "ccomponenttype.h"
#include "cconfiguration.h"

cLogFormatter::cLogFormatter(const char *format)
{
    parseFormat(format);
}

void cLogFormatter::parseFormat(const char *format)
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

cLogFormatter::FormatDirective cLogFormatter::getDirective(char ch)
{
    switch (ch)
    {
    // log statement related
        case 'l': return LOGDIRECTIVE_LOGLEVEL;
        case 'c': return LOGDIRECTIVE_LOGCATEGORY;

        // current simulation state related
        case 'e': return LOGDIRECTIVE_CURRENTEVENTNUMBER;
        case 't': return LOGDIRECTIVE_CURRENTSIMULATIONTIME;
        case 'v': return LOGDIRECTIVE_CURRENTEVENTNAME;

        case 'm': return LOGDIRECTIVE_CURRENTMESSAGENAME;
        case 'a': return LOGDIRECTIVE_CURRENTMESSAGECLASSNAME;

        case 'n': return LOGDIRECTIVE_CURRENTMODULENAME;
        case 'p': return LOGDIRECTIVE_CURRENTMODULEPATH;
        case 'o': return LOGDIRECTIVE_CURRENTMODULECLASSNAME;
        case 's': return LOGDIRECTIVE_CURRENTMODULENEDTYPESIMPLENAME;
        case 'q': return LOGDIRECTIVE_CURRENTMODULENEDTYPEQUALIFIEDNAME;

        case 'N': return LOGDIRECTIVE_CONTEXTMODULENAME;
        case 'P': return LOGDIRECTIVE_CONTEXTMODULEPATH;
        case 'O': return LOGDIRECTIVE_CONTEXTMODULECLASSNAME;
        case 'S': return LOGDIRECTIVE_CONTEXTMODULENEDTYPESIMPLENAME;
        case 'Q': return LOGDIRECTIVE_CONTEXTMODULENEDTYPEQUALIFIEDNAME;

        // simulation run related
        case 'G': return LOGDIRECTIVE_CONFIGNAME;
        case 'R': return LOGDIRECTIVE_RUNNUMBER;

        case 'X': return LOGDIRECTIVE_NETWORKMODUECLASSNAME;
        case 'Y': return LOGDIRECTIVE_NETWORKMODUENEDTYPESIMPLENAME;
        case 'Z': return LOGDIRECTIVE_NETWORKMODUENEDTYPEQUALIFIEDNAME;

        // C++ source related
        case '*': return LOGDIRECTIVE_SOURCEOBJECTPOINTER;
        case 'b': return LOGDIRECTIVE_SOURCEOBJECTNAME;
        case 'd': return LOGDIRECTIVE_SOURCEOBJECTPATH;

        case 'x': return LOGDIRECTIVE_SOURCECOMPONENTNEDSIMPLENAME;
        case 'y': return LOGDIRECTIVE_SOURCECOMPONENTNEDQUALIFIEDNAME;

        case 'f': return LOGDIRECTIVE_SOURCEFILE;
        case 'i': return LOGDIRECTIVE_SOURCELINE;

        case 'z': return LOGDIRECTIVE_SOURCECLASS;
        case 'u': return LOGDIRECTIVE_SOURCEFUNCTION;

        // operating system related
        case 'w': return LOGDIRECTIVE_USERTIME;
        case 'W': return LOGDIRECTIVE_WALLTIME;

        case 'H': return LOGDIRECTIVE_HOSTNAME;
        case 'I': return LOGDIRECTIVE_PROCESSID;

        default: throw cRuntimeError("Unknown format character '%c'", ch);
    }
}

void cLogFormatter::addPart(FormatDirective directive, char *textBegin, char *textEnd)
{
    FormatPart part;
    part.directive = directive;
    if (textBegin && textEnd)
        part.text = std::string(textBegin, textEnd - textBegin);
    formatParts.push_back(part);
}

void cLogFormatter::formatEntry(std::ostream& stream, cLogEntry *entry)
{
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

            case LOGDIRECTIVE_CURRENTMESSAGENAME:
                if (ev.getCurrentEventName()) stream << ev.getCurrentEventName(); break;
            case LOGDIRECTIVE_CURRENTMESSAGECLASSNAME:
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

            case LOGDIRECTIVE_NETWORKMODUECLASSNAME:
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

            case LOGDIRECTIVE_SOURCECLASS:
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
//    stream << entry->text;
    stream.rdbuf()->sputn(entry->text, entry->textLength);
}
