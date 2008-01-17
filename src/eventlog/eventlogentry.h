//=========================================================================
//  EVENTLOGENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGENTRY_H_
#define __EVENTLOGENTRY_H_

#include <sstream>
#include "matchexpression.h"
#include "eventlogdefs.h"
#include "linetokenizer.h"

NAMESPACE_BEGIN

class Event;

/**
 * Base class for all kind of event log entries.
 * An entry is represented by a single line in the log file.
 */
class EVENTLOG_API EventLogEntry : public MatchExpression::Matchable
{
    public:
        int contextModuleId;
        unsigned int level;

    protected:
        Event* event; // back pointer
        static char buffer[128];
        static LineTokenizer tokenizer; // not thread safe

    public:
        EventLogEntry();
        virtual ~EventLogEntry() {}
        virtual void parse(char *line, int length) = 0;
        virtual void print(FILE *fout) = 0;
        virtual int getClassIndex() = 0;
        virtual const char *getClassName() = 0;

        Event *getEvent() { return event; }
        int getIndex();
        bool isMessageSend();

        virtual const std::vector<const char *> getAttributeNames() const = 0;
        virtual const char *getDefaultAttribute() const = 0;
        virtual const char *getAttribute(const char *name) const = 0;

        static EventLogEntry *parseEntry(Event *event, char *line, int length);
};

/**
 * Base class for entries represented by key value tokens.
 */
class EVENTLOG_API EventLogTokenBasedEntry : public EventLogEntry
{
    protected:
        char *getToken(char **tokens, int numTokens, const char *sign, bool mandatory);
        int getIntToken(char **tokens, int numTokens, const char *sign, bool mandatory, int defaultValue);
        long getLongToken(char **tokens, int numTokens, const char *sign, bool mandatory, long defaultValue);
        simtime_t getSimtimeToken(char **tokens, int numTokens, const char *sign, bool mandatory, simtime_t defaultValue);
        const char *getStringToken(char **tokens, int numTokens, const char *sign, bool mandatory, const char *defaultValue);

    public:
        virtual void parse(char *line, int length);
        virtual void parse(char **tokens, int numTokens) = 0;
};

/**
 * One line log message entry.
 */
class EVENTLOG_API EventLogMessageEntry : public EventLogEntry
{
    public:
        const char *text;

    public:
        EventLogMessageEntry(Event *event);
        virtual void parse(char *line, int length);
        virtual void print(FILE *fout);
        virtual int getClassIndex() { return 0; }
        virtual const char *getClassName() { return "EventLogMessageEntry"; }

        virtual const std::vector<const char *> getAttributeNames() const;
        virtual const char *getDefaultAttribute() const { return "-"; }
        virtual const char *getAttribute(const char *name) const;
};

NAMESPACE_END


#endif
