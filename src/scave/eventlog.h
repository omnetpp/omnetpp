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
 * A compound or simple module extracted from the log file
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
 * A message sent from one module to another.
 */
class MessageEntry
{
    public:
        /** Tells if this entry represents a message delivery or a message send */
        bool delivery;
        long lineNumber;
        std::string messageClassName;
        std::string messageName;

         /** These log messages actually belong to the target event, but this way we can preserve ordering of message entries within the event */
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
        MessageEntry *cause;
        MessageEntryList causes;
        MessageEntryList consequences;

    public:
        EventEntry();
        ~EventEntry();
};

/**
 * A trace utility to trace the causes and consequences of a particular event back and forth in time.
 */
class EventLog
{
    protected:
        typedef std::vector<EventEntry*> EventEntryList;

        /** If this is a filtered event log, the parent log owns the event entries and message entries */
        EventLog *parent;
        /** Name of the file */;
        const char *logFileName;
        /** Modules that occur in the events */
        ModuleEntryList moduleList;
        /** Ordered list of EventEntries as read from the log file */
        EventEntryList eventList;
        /** Ordered list of MessageEntries as read from the log file */
        MessageEntryList messageList;
        /** Last traced event if any */
        EventEntry *tracedEvent;

    public:
        EventLog(const char *logFileName);
        ~EventLog();

        /**
         * Returns the number of events. Events are guaranteed to be in
         * increasing eventNumber and time order.
         */
        int getNumEvents();
        EventEntry *getEvent(int pos);
        int findEvent(EventEntry *event); // returns pos, or -1 if not found
        EventEntry *getEventByNumber(long eventNumber);
        EventEntry *getFirstEventAfter(double t);

        int getNumModules();
        ModuleEntry *getModule(int pos);

        EventLog *traceEvent(EventEntry *tracedEvent, bool wantCauses, bool wantConsequences);
        void writeTrace(FILE* fout);

    protected:
        EventLog(EventLog *parent);
        void parseLogFile();
        ModuleEntry *getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullName);
        char *tokensToStr(int numTokens, char **vec);
};

#endif


