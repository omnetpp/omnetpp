//==========================================================================
//   LOGFORMATTER.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __LOGFORMATTER_H
#define __LOGFORMATTER_H

#include <ostream>
#include <vector>
#include "clog.h"

/**
 * This class prints log messages to a stream based on a format string. The format string
 * contains constant parts and special format characters that are substituted runtime with
 * the corresponding data from the context.
 *
 * Log statement related format characters:
 *  - %l log level name
 *  - %c log category
 *
 * Current simulation state related format characters:
 *  - %e current event number
 *  - %t current simulation time
 *  - %v current event name
 *
 *  - %m current message name
 *  - %a current message class name
 *
 *  - %n current module name
 *  - %p current module path
 *  - %o current module class name
 *  - %s current module NED type simple name
 *  - %q current module NED type fully qualified name
 *
 *  - %N context module name
 *  - %P context module path
 *  - %O context module class name
 *  - %S context module NED type simple name
 *  - %Q context module NED type fully qualified name
 *
 * Simulation run related format characters:
 *  - %G config name
 *  - %R run number
 *
 *  - %X network module class name
 *  - %Y network module NED type simple name
 *  - %Z network module NED type fully qualified name
 *
 * C++ source related (where the log statement appears) format characters:
 *  - %* source object pointer
 *  - %b source object name
 *  - %d source object path
 *
 *  - %x source component NED type simple name
 *  - %y source component NED type fully qualified
 *
 *  - %f source file name
 *  - %i source line number
 *
 *  - %z source class name
 *  - %u source function name
 *
 * Operating system related format characters:
 *  - %w user time in seconds
 *  - %W human readable wall time
 *
 *  - %H host name
 *  - %I process id
 *
 * Escaping % character:
 *  - %% one % character
 */
class LogFormatter
{
  private:
    enum FormatDirective {
        LOGDIRECTIVE_CONSTANTTEXT,

        // log statement related
        LOGDIRECTIVE_LOGLEVEL,
        LOGDIRECTIVE_LOGCATEGORY,

        // current simulation state related
        LOGDIRECTIVE_CURRENTEVENTNUMBER,
        LOGDIRECTIVE_CURRENTSIMULATIONTIME,
        LOGDIRECTIVE_CURRENTEVENTNAME,

        LOGDIRECTIVE_CURRENTMESSAGENAME,
        LOGDIRECTIVE_CURRENTMESSAGECLASSNAME,

        LOGDIRECTIVE_CURRENTMODULENAME,
        LOGDIRECTIVE_CURRENTMODULEPATH,
        LOGDIRECTIVE_CURRENTMODULECLASSNAME,
        LOGDIRECTIVE_CURRENTMODULENEDTYPESIMPLENAME,
        LOGDIRECTIVE_CURRENTMODULENEDTYPEQUALIFIEDNAME,

        LOGDIRECTIVE_CONTEXTMODULENAME,
        LOGDIRECTIVE_CONTEXTMODULEPATH,
        LOGDIRECTIVE_CONTEXTMODULECLASSNAME,
        LOGDIRECTIVE_CONTEXTMODULENEDTYPESIMPLENAME,
        LOGDIRECTIVE_CONTEXTMODULENEDTYPEQUALIFIEDNAME,

        // simulation run related
        LOGDIRECTIVE_CONFIGNAME,
        LOGDIRECTIVE_RUNNUMBER,

        LOGDIRECTIVE_NETWORKMODUECLASSNAME,
        LOGDIRECTIVE_NETWORKMODUENEDTYPESIMPLENAME,
        LOGDIRECTIVE_NETWORKMODUENEDTYPEQUALIFIEDNAME,

        // C++ source related
        LOGDIRECTIVE_SOURCEOBJECTPOINTER,
        LOGDIRECTIVE_SOURCEOBJECTNAME,
        LOGDIRECTIVE_SOURCEOBJECTPATH,

        LOGDIRECTIVE_SOURCECOMPONENTNEDSIMPLENAME,
        LOGDIRECTIVE_SOURCECOMPONENTNEDQUALIFIEDNAME,

        LOGDIRECTIVE_SOURCEFILE,
        LOGDIRECTIVE_SOURCELINE,

        LOGDIRECTIVE_SOURCECLASS,
        LOGDIRECTIVE_SOURCEFUNCTION,

        // operating system related
        LOGDIRECTIVE_USERTIME,
        LOGDIRECTIVE_WALLTIME,

        LOGDIRECTIVE_HOSTNAME,
        LOGDIRECTIVE_PROCESSID
    };

    struct FormatPart {
        FormatDirective directive;
        std::string text;
    };

    std::vector<FormatPart> formatParts;

  public:
    LogFormatter() { }
    LogFormatter(const char *format);

    void setFormat(const char *format) { formatParts.clear(); parseFormat(format); }
    void formatEntry(std::ostream& stream, cLogEntry *entry);

  private:
    void parseFormat(const char *format);
    FormatDirective getDirective(char ch);
    void addPart(FormatDirective directive, char *textBegin, char *textEnd);
};

#endif
