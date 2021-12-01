//=========================================================================
//  EVENTLOGENTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "eventlogdefs.h"
#include "ichunk.h"

namespace omnetpp {
namespace eventlog {

class Event;
class EventLog;


/**
 * Base class for all kind of eventlog entries.
 * An entry is represented by a single line in the log file.
 */
class EVENTLOG_API EventLogEntry : public omnetpp::common::MatchExpression::Matchable
{
    public:
        int contextModuleId = -1;
        int level = -1; // indent level (method call depth)

    protected:
        file_offset_t offset = -1;
        IChunk *chunk = nullptr; // back pointer to event/snapshot/index
        int entryIndex = -1; // index within the event
        static char buffer[128]; // non-thread safe buffer temporarily used to parse eventlog entries
        static const char *currentLine;
        static int currentLineLength;

    public:
        EventLogEntry() {}
        virtual ~EventLogEntry() {}
        virtual void print(FILE *fout) = 0;
        virtual int getClassIndex() = 0;
        virtual const char *getClassName() = 0;

        file_offset_t getOffset() { return offset; }
        IChunk *getChunk() { return chunk; }
        Event *getEvent();
        eventnumber_t getEventNumber() { return chunk->getEventNumber(); }
        simtime_t getSimulationTime() { return chunk->getSimulationTime(); }
        int getEntryIndex() { return entryIndex; }
        EventLogEntry *getPreviousEventLogEntry();
        EventLogEntry *getNextEventLogEntry();

        virtual const std::vector<const char *> getAttributeNames() const = 0;
        virtual const char *getAsString() const = 0;
        virtual const char *getAsString(const char *attribute) const = 0;

        static EventLogEntry *parseEntry(EventLog *eventLog, IChunk *chunk, int entryIndex, file_offset_t offset, char *line, int length);
        static eventnumber_t parseEventNumber(const char *str);
        static simtime_t parseSimulationTime(const char *str);
};

typedef std::vector<EventLogEntry *> EventLogEntryList;

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
        virtual void parse(char **tokens, int numTokens) = 0;
};

/**
 * One line log message entry.
 */
class EVENTLOG_API EventLogMessageEntry : public EventLogEntry
{
    public:
        const char *text = nullptr;

    public:
        EventLogMessageEntry(IChunk *chunk, int entryIndex);
        virtual void parse(char *line, int length);
        virtual void print(FILE *fout) override;
        virtual int getClassIndex() override { return 0; }
        virtual const char *getClassName() override { return "EventLogMessageEntry"; }

        virtual const std::vector<const char *> getAttributeNames() const override;
        virtual const char *getAsString() const override { return "-"; }
        virtual const char *getAsString(const char *attribute) const override;
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
