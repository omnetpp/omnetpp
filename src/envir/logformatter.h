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
 *  - %N context component name
 *  - %M context component path
 *  - %O context component class name
 *  - %S context component NED type simple name
 *  - %Q context component NED type fully qualified name
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
 *  - %z source class name
 *  - %u source function name
 *
 *  - %x source component NED type simple name
 *  - %y source component NED type fully qualified
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
        CONSTANT_TEXT,

        // log statement related
        LOGLEVEL = 'l',
        LOGCATEGORY = 'c',

        // current simulation state related
        CURRENT_EVENT_NUMBER = 'e',
        CURRENT_SIMULATION_TIME = 't',

        CURRENT_EVENT_NAME = 'v',
        CURRENT_EVENT_CLASSNAME = 'a',

        CURRENT_MODULE_NAME = 'n',
        CURRENT_MODULE_FULLPATH = 'm',
        CURRENT_MODULE_CLASSNAME = 'o',
        CURRENT_MODULE_NEDTYPE_SIMPLENAME = 's',
        CURRENT_MODULE_NEDTYPE_QUALIFIEDNAME = 'q',

        CONTEXT_COMPONENT_NAME = 'N',
        CONTEXT_COMPONENT_FULLPATH = 'M',
        CONTEXT_COMPONENT_CLASSNAME = 'O',
        CONTEXT_COMPONENT_NEDTYPE_SIMPLENAME = 'S',
        CONTEXT_COMPONENT_NEDTYPE_QUALIFIEDNAME = 'Q',

        // simulation run related
        CONFIGNAME = 'G',
        RUNNUMBER = 'R',

        NETWORK_MODULE_CLASSNAME = 'X',
        NETWORK_MODULE_NEDTYPE_SIMPLENAME = 'Y',
        NETWORK_MODULE_NEDTYPE_QUALIFIEDNAME = 'Z',

        // C++ source related
        SOURCE_OBJECT_POINTER = 'p',
        SOURCE_OBJECT_NAME = 'b',
        SOURCE_OBJECT_FULLPATH = 'd',
        SOURCE_OBJECT_CLASSNAME = 'z',
        SOURCE_FUNCTION = 'u',

        SOURCE_COMPONENT_NEDTYPE_SIMPLENAME = 'x',
        SOURCE_COMPONENT_NEDTYPE_QUALIFIEDNAME = 'y',

        SOURCE_FILE = 'f',
        SOURCE_LINE = 'i',

        // operating system related
        USERTIME = 'w',
        WALLTIME = 'W',

        HOSTNAME = 'H',
        PROCESSID = 'I'
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
