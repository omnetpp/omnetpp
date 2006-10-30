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
#include "defs.h"
#include "linetokenizer.h"

class Event;

/**
 * Base class for all kind of event log entries.
 * An entry is represented by a single line in the log file.
 */
class EventLogEntry
{
    protected:
        Event* event; // back pointer
        static LineTokenizer tokenizer; // not thread safe

    public:
        virtual ~EventLogEntry() {}
        virtual void parse(char *line, int length) = 0;
        virtual void print(FILE *fout) = 0;
        virtual int getClassIndex() = 0;
        virtual const char *getClassName() = 0;

        Event *getEvent() { return event; }
        bool isMessageSend();

        static EventLogEntry *parseEntry(Event *event, char *line, int length);
};

/**
 * Base class for entries represented by key value tokens.
 */
class EventLogTokenBasedEntry : public EventLogEntry
{
    protected:
        char *getToken(char **tokens, int numTokens, const char *sign);
        int getIntToken(char **tokens, int numTokens, const char *sign);
        long getLongToken(char **tokens, int numTokens, const char *sign);
        simtime_t getSimtimeToken(char **tokens, int numTokens, const char *sign);
        const char *getStringToken(char **tokens, int numTokens, const char *sign);

    public:
        virtual void parse(char *line, int length);
        virtual void parse(char **tokens, int numTokens) = 0;
};

/**
 * One line log message entry.
 */
class EventLogMessage : public EventLogEntry
{
    public:
        const char *text;

    public: 
        EventLogMessage(Event *event);
        virtual void parse(char *line, int length);
        virtual void print(FILE *fout);
        virtual int getClassIndex() { return 0; }
        virtual const char *getClassName() { return "EventLogMessage"; }
};

#endif


