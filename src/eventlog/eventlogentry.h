//=========================================================================
//  EVENTLOGENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_EVENTLOGENTRY_H
#define __OMNETPP_EVENTLOG_EVENTLOGENTRY_H

#include <sstream>
#include "common/matchexpression.h"
#include "common/linetokenizer.h"
#include "common/filereader.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetpp/platdep/platmisc.h" // PRId64
#include "eventlogdefs.h"
#include "enums.h"

namespace omnetpp {
namespace eventlog {

class Event;
class EventLog;


/**
 * Base class for all kind of event log entries.
 * An entry is represented by a single line in the log file.
 */
class EVENTLOG_API EventLogEntry : public omnetpp::common::MatchExpression::Matchable
{
    public:
        int contextModuleId;
        int level; // indent level (method call depth)

    protected:
        Event* event; // back pointer
        int entryIndex;
        static char buffer[128];
        static omnetpp::common::LineTokenizer tokenizer; // not thread safe

    public:
        EventLogEntry();
        virtual ~EventLogEntry() {}
        virtual void parse(char *line, int length) = 0;
        virtual void print(FILE *fout) = 0;
        virtual int getClassIndex() = 0;
        virtual const char *getClassName() = 0;

        Event *getEvent() { return event; }
        int getEntryIndex() { return entryIndex; }

        virtual const std::vector<const char *> getAttributeNames() const = 0;
        virtual const char *getAsString() const = 0;
        virtual const char *getAsString(const char *attribute) const = 0;

        static EventLogEntry *parseEntry(EventLog *eventLog, Event *event, int entryIndex, file_offset_t offset, char *line, int length);
        static eventnumber_t parseEventNumber(const char *str);
        static simtime_t parseSimulationTime(const char *str);
};

/**
 * Base class for entries represented by key value tokens.
 */
class EVENTLOG_API EventLogTokenBasedEntry : public EventLogEntry
{
    public:
        static char *getToken(char **tokens, int numTokens, const char *sign, bool mandatory);
        static bool getBoolToken(char **tokens, int numTokens, const char *sign, bool mandatory, bool defaultValue);
        static int getIntToken(char **tokens, int numTokens, const char *sign, bool mandatory, int defaultValue);
        static short getShortToken(char **tokens, int numTokens, const char *sign, bool mandatory, short defaultValue);
        static long getLongToken(char **tokens, int numTokens, const char *sign, bool mandatory, long defaultValue);
        static int64_t getInt64Token(char **tokens, int numTokens, const char *sign, bool mandatory, int64_t defaultValue);
        static eventnumber_t getEventNumberToken(char **tokens, int numTokens, const char *sign, bool mandatory, eventnumber_t defaultValue);
        static simtime_t getSimtimeToken(char **tokens, int numTokens, const char *sign, bool mandatory, simtime_t defaultValue);
        static const char *getStringToken(char **tokens, int numTokens, const char *sign, bool mandatory, const char *defaultValue);

    public:
        virtual void parse(char *line, int length) override;
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
        EventLogMessageEntry(Event *event, int entryIndex);
        virtual void parse(char *line, int length) override;
        virtual void print(FILE *fout) override;
        virtual int getClassIndex() override { return 0; }
        virtual const char *getClassName() override { return "EventLogMessageEntry"; }

        virtual const std::vector<const char *> getAttributeNames() const override;
        virtual const char *getAsString() const override { return "-"; }
        virtual const char *getAsString(const char *attribute) const override;
};

} // namespace eventlog
}  // namespace omnetpp


#endif
