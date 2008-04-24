//=========================================================================
//  IEVENT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __IEVENT_H_
#define __IEVENT_H_

#include <sstream>
#include "eventlogentry.h"
#include "eventlogentries.h"
#include "messagedependency.h"

NAMESPACE_BEGIN

class EVENTLOG_API IEvent
{
    protected:
        IEvent *previousEvent;
        IEvent *nextEvent;

    public:
        // gui state for EventLogTable
        bool isExpandedInEventLogTable;

        // gui state for SequenceChart
        double cachedTimelineCoordinate;
        long cachedTimelineCoordinateSystemVersion;

    public:
        IEvent();
        virtual ~IEvent() {}

        /**
         * Synchorizes state when the underlying log file changes (new events are appended).
         */
        virtual void synchronize() = 0;

        /**
         * Returns the corresponding event log.
         */
        virtual IEventLog *getEventLog() = 0;

        /**
         * Returns the begin offset of the event in the log file.
         */
        virtual file_offset_t getBeginOffset() = 0;
        /**
         * Returns the end offset of the event in the log file.
         */
        virtual file_offset_t getEndOffset() = 0;

        /**
         * Returns the 'E' entry (line) corresponding to this event.
         */
        virtual EventEntry *getEventEntry() = 0;
        /**
         * Returns the number of log file entries (lines) for this event.
         */
        virtual int getNumEventLogEntries() = 0;
        /**
         * Returns the nth entry (line) for this event.
         */
        virtual EventLogEntry *getEventLogEntry(int index) = 0;
        virtual bool isSelfMessage(BeginSendEntry *beginSendEntry) = 0;

        // simple text lines
        virtual int getNumEventLogMessages() = 0;
        virtual int getNumBeginSendEntries() = 0;
        virtual EventLogMessageEntry *getEventLogMessage(int index) = 0;

        // some of the data found in the 'E' entry (line), to get additional data query the entries
        virtual long getEventNumber() = 0;
        virtual simtime_t& getSimulationTime() = 0;
        virtual int getModuleId() = 0;
        virtual long getMessageId() = 0;
        virtual long getCauseEventNumber() = 0;

        /**
         * Returns the immediately preceding event or NULL if there's no such event.
         */
        virtual IEvent *getPreviousEvent() = 0;
        /**
         * Returns the immediately following event or NULL if there's no such event.
         */
        virtual IEvent *getNextEvent() = 0;

        /**
         * Returns the closest preceding event which caused this event by sending a message.
         */
        virtual IEvent *getCauseEvent() = 0;
        virtual BeginSendEntry *getCauseBeginSendEntry() = 0;
        virtual IMessageDependency *getCause() = 0;
        virtual IMessageDependencyList *getCauses() = 0;
        virtual IMessageDependencyList *getConsequences() = 0;

        /**
         * Print all entries of this event.
         */
        virtual void print(FILE *file = stdout, bool outputEventLogMessages = true) = 0;

        /**
         * Returns the log entry which describes the event's module.
         */
        virtual ModuleCreatedEntry *getModuleCreatedEntry() = 0;

        /**
         * Returns the index of the begin send entry where the given message was sent.
         */
        virtual int findBeginSendEntryIndex(int messageId);

        /**
         * Returns true if the received message was a self message.
         */
        virtual bool isSelfMessageProcessingEvent() = 0;

        /**
         * Used to maintain the double linked list.
         */
        static void linkEvents(IEvent *previousEvent, IEvent *nextEvent);
        static void unlinkEvents(IEvent *previousEvent, IEvent *nextEvent);
};

NAMESPACE_END


#endif
