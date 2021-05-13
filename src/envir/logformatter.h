//==========================================================================
//   LOGFORMATTER.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_LOGFORMATTER_H
#define __OMNETPP_ENVIR_LOGFORMATTER_H

#include <ostream>
#include <vector>
#include "omnetpp/clog.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

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
 *  - %g current fingerprint if fingerprint verification is enabled in the configuration, otherwise empty
 *  - %v current message or event name
 *  - %a current message or event class name
 *
 *  - %n module name of current event
 *  - %m module path of current event
 *  - %o module class name of current event
 *  - %s simple NED type name of module of current event
 *  - %q fully qualified NED type name of module of current event
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
 * Compound fields
 *  - %E event object (class name, name)
 *  - %U module of current event (NED type, full path)
 *  - %C context component (NED type, full path)
 *  - %K context component, if different from current module (NED type, full path)
 *  - %J source component or object (NED type or class, full path or pointer)
 *  - %L source component or object, if different from context component (NED type or class, full path or pointer)
 *
 * Escaping % character:
 *  - %% one % character
 *
 * Padding with spaces:
 *  - %[0-9]+ add spaces until specified column
 *  - %| adaptive tabstop: add padding until longest prefix seen so far
 *  - %> function call depth times 2-space indentation (see Enter_Method, Enter_Method_Silent)
 *  - %< remove preceding whitespace characters
 *
 * Conditional constant text:
 *  - %? ignore the following constant part if the preceding directive didn't print anything (useful for separators)
 */
class ENVIR_API LogFormatter
{
  private:
    enum FormatDirective {
        CONSTANT_TEXT,
        PADDING,
        ADAPTIVE_TAB = '|',
        INDENT = '>',
        TRIM = '<',

        // log statement related
        LOGLEVEL = 'l',
        LOGCATEGORY = 'c',

        // current simulation state related
        EVENT_NUMBER = 'e',
        SIMULATION_TIME = 't',
        FINGERPRINT = 'g',

        EVENT_OBJECT_NAME = 'v',
        EVENT_OBJECT_CLASSNAME = 'a',

        EVENT_MODULE_NAME = 'n',
        EVENT_MODULE_FULLPATH = 'm',
        EVENT_MODULE_CLASSNAME = 'o',
        EVENT_MODULE_NEDTYPE_SIMPLENAME = 's',
        EVENT_MODULE_NEDTYPE_QUALIFIEDNAME = 'q',

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
        PROCESSID = 'I',

        // compound fields
        EVENT_OBJECT = 'E',
        EVENT_MODULE = 'U',
        CONTEXT_COMPONENT = 'C',
        CONTEXT_COMPONENT_IF_DIFFERENT = 'K',
        SOURCE_COMPONENT_OR_OBJECT = 'J',
        SOURCE_COMPONENT_OR_OBJECT_IF_DIFFERENT = 'L',
    };

    struct FormatPart {
        FormatDirective directive;
        int padding;
        std::string text;
        bool conditional;
    };

    bool isBlank_ = true;
    std::vector<FormatPart> formatParts;
    std::vector<int> adaptiveTabColumns;

  public:
    LogFormatter() {}
    LogFormatter(const char *format);

    void setFormat(const char *format) { parseFormat(format); }
    bool isBlank() const { return isBlank_; }
    bool usesEventName() const {return containsDirective(EVENT_OBJECT) || containsDirective(EVENT_OBJECT_NAME);}
    bool usesEventClassName() const {return containsDirective(EVENT_OBJECT) || containsDirective(EVENT_OBJECT_CLASSNAME);}
    std::string formatPrefix(cLogEntry *entry);
    void resetAdaptiveTabs();

  private:
    void parseFormat(const char *format);
    FormatDirective getDirective(char ch);
    void addPart(FormatDirective directive, char *textBegin, char *textEnd, bool conditional);
    bool containsDirective(FormatDirective directive) const;
};

}  // namespace envir
}  // namespace omnetpp

#endif
