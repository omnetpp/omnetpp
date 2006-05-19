//=========================================================================
//  EVENTTRACE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTTRACE_H_
#define __EVENTTRACE_H_

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "filetokenizer.h"

/**
 * @brief A compound or simple module extracted from the log file
 */
class ModuleEntry
{
    public:
        std::string moduleClassName;
        std::string moduleFullName;
        int moduleId;

    public:
        ModuleEntry();
        ~ModuleEntry();
};

typedef std::vector<ModuleEntry*> ModuleEntryList;

class EventEntry;

/**
 * @brief A message sent from one module to another.
 */
class MessageEntry
{
    public:
        /** @brief Tells if this entry represents a message delivery or a message send */
        bool delivery;
        long lineNumber;
        std::string messageClassName;
        std::string messageName;
        std::vector<std::string> logMessages;

        EventEntry *source;
        EventEntry *target;

    public:
        MessageEntry();
        ~MessageEntry();
};

typedef std::vector<MessageEntry*> MessageEntryList;

/**
 * An event log entry as seen in the log file corresponding to a handleMessage call.
 */
class EventEntry
{
    public:
        long eventNumber;
        double simulationTime; // TODO: use simtime_t
        ModuleEntry *module;
        MessageEntry* cause;
        MessageEntryList causes;
        MessageEntryList consequences;

    public:
        EventEntry();
        ~EventEntry();
};

typedef std::vector<EventEntry*> EventEntryList;

/**
 * A trace utility to trace the causes and cosequences of a particular event back and forth in time.
 */
class EventLog
{
    protected:
        const char *logFileName;
        /** @brief Ordered list of ModuleEntries as read from the log file */
        ModuleEntryList moduleList;
        /** @brief Ordered list of EventEntries as read from the log file */
        EventEntryList eventList;
        /** @brief Ordered list of MessageEntries as read from the log file */
        MessageEntryList messageList;
        /** @brief Last traced event if any */
        EventEntry *tracedEvent;

    public:
        EventLog(const char *logFileName);
        ~EventLog();

        EventEntry *getEvent(long eventNumber);
        EventLog *traceEvent(long tracedEventNumber, bool causes = true, bool consequences = false);
        void writeTrace(FILE* fout);

    protected:
        EventLog();
        void parseLogFile();
        ModuleEntry* getModule(int moduleId, char *moduleClassName, char *moduleFullName);
        char *tokensToStr(int numTokens, char **vec);
};

#endif