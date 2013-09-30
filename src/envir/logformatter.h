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
 *  - %v current message or event name
 *  - %a current message or event class name
 *
 *  - %n current module name
 *  - %m current module path
 *  - %o current module class name
 *  - %s current module NED type simple name
 *  - %q current module NED type fully qualified name
 *
 *  - %N context module name
 *  - %M context module path
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
 *  - %p source object pointer
 *  - %b source object name
 *  - %d source object path
 *
 *  - %x source component NED type simple name
 *  - %y source component NED type fully qualified
 *
 *  - %z source class name
 *  - %u source function name
 *
 *  - %f source file name
 *  - %i source line number
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
        LOGDIRECTIVE_LOGLEVEL = 'l',
        LOGDIRECTIVE_LOGCATEGORY = 'c',

        // current simulation state related
        LOGDIRECTIVE_CURRENTEVENTNUMBER = 'e',
        LOGDIRECTIVE_CURRENTSIMULATIONTIME = 't',

        LOGDIRECTIVE_CURRENTEVENTNAME = 'v',
        LOGDIRECTIVE_CURRENTEVENTCLASSNAME = 'a',

        LOGDIRECTIVE_CURRENTMODULENAME = 'n',
        LOGDIRECTIVE_CURRENTMODULEPATH = 'm',
        LOGDIRECTIVE_CURRENTMODULECLASSNAME = 'o',
        LOGDIRECTIVE_CURRENTMODULENEDTYPESIMPLENAME = 's',
        LOGDIRECTIVE_CURRENTMODULENEDTYPEQUALIFIEDNAME = 'q',

        LOGDIRECTIVE_CONTEXTMODULENAME = 'N',
        LOGDIRECTIVE_CONTEXTMODULEPATH = 'M',
        LOGDIRECTIVE_CONTEXTMODULECLASSNAME = 'O',
        LOGDIRECTIVE_CONTEXTMODULENEDTYPESIMPLENAME = 'S',
        LOGDIRECTIVE_CONTEXTMODULENEDTYPEQUALIFIEDNAME = 'Q',

        // simulation run related
        LOGDIRECTIVE_CONFIGNAME = 'G',
        LOGDIRECTIVE_RUNNUMBER = 'R',

        LOGDIRECTIVE_NETWORKMODULECLASSNAME = 'X',
        LOGDIRECTIVE_NETWORKMODUENEDTYPESIMPLENAME = 'Y',
        LOGDIRECTIVE_NETWORKMODUENEDTYPEQUALIFIEDNAME = 'Z',

        // C++ source related
        LOGDIRECTIVE_SOURCEOBJECTPOINTER = 'p',
        LOGDIRECTIVE_SOURCEOBJECTNAME = 'b',
        LOGDIRECTIVE_SOURCEOBJECTPATH = 'd',

        LOGDIRECTIVE_SOURCECOMPONENTNEDSIMPLENAME = 'x',
        LOGDIRECTIVE_SOURCECOMPONENTNEDQUALIFIEDNAME = 'y',

        LOGDIRECTIVE_SOURCEFILE = 'z',
        LOGDIRECTIVE_SOURCELINE = 'u',

        LOGDIRECTIVE_SOURCEOBJECTCLASSNAME = 'f',
        LOGDIRECTIVE_SOURCEFUNCTION = 'i',

        // operating system related
        LOGDIRECTIVE_USERTIME = 'w',
        LOGDIRECTIVE_WALLTIME = 'W',

        LOGDIRECTIVE_HOSTNAME = 'H',
        LOGDIRECTIVE_PROCESSID = 'I'
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
    std::string formatPrefix(cLogEntry *entry);

  private:
    void parseFormat(const char *format);
    FormatDirective getDirective(char ch);
    void addPart(FormatDirective directive, char *textBegin, char *textEnd);
};

#endif
