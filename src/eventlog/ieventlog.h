//=========================================================================
//  IEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __IEVENTLOG_H_
#define __IEVENTLOG_H_

#include <sstream>
#include <set>
#include "filereader.h"
#include "eventlogdefs.h"
#include "ievent.h"

class IEventLog;

class EVENTLOG_API ProgressMonitor
{
    public:
        typedef void (*MonitorFunction)(IEventLog *, void *);

        MonitorFunction monitorFunction;
        void *data;

    public:
        ProgressMonitor()
        {
            monitorFunction = NULL;
            data = NULL;
        }

        ProgressMonitor(MonitorFunction monitorFunction, void *data)
        {
            this->monitorFunction = monitorFunction;
            this->data = data;
        }

        void progress(IEventLog *eventLog) { if (monitorFunction) monitorFunction(eventLog, data); }
};

class EVENTLOG_API IEventLog
{
    protected:
        /**
         * Remembers the last IEvent returned by getNeighbourEvent so that subsequent calls might return much faster.
         */
        long lastNeighbourEventNumber;
        IEvent *lastNeighbourEvent;

    public:
        IEventLog();
        virtual ~IEventLog() {}

        /**
         * Sets the progress monitor which will be notified when a long running operation has some progress.
         */
        virtual ProgressMonitor setProgressMonitor(ProgressMonitor progressMonitor) = 0;
        /**
         * Set the minimum interval between progress callbacks for long running event log operations.
         */
        virtual void setProgressCallInterval(double seconds) = 0;

        /**
         * Synchorizes state when the underlying log file changes (new events are appended).
         */
		virtual void synchronize();

        /**
         * Returns the file reader used to read in events.
         */
        virtual FileReader *getFileReader() = 0;

        /**
         * Returns the number of events parsed so far.
         */
        virtual long getNumParsedEvents() = 0;
        /**
         * Returns the message names parsed so far.
         */
        virtual std::set<const char *>& getMessageNames() = 0;
        /**
         * Returns the message class names parsed so far.
         */
        virtual std::set<const char *>& getMessageClassNames() = 0;
        /**
         * Returns the entry which describes the module with the given id.
         */
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int moduleId) = 0;
        /**
         * Returns the number of module created entries which is actually the next unused module id.
         */
        virtual int getNumModuleCreatedEntries() = 0;

        /**
         * Returns true if the event log does not contain any events.
         */
        virtual bool isEmpty() { return !getFirstEvent(); }
        /**
         * Returns the first event or NULL if the log is empty.
         */
        virtual IEvent *getFirstEvent() = 0;
        /**
         * Returns the last event or NULL if the log is empty.
         */
        virtual IEvent *getLastEvent() = 0;
        /**
         * Returns the requested event or NULL if there's no such event included in the log.
         * The given event number may not be included in the log.
         */
        virtual IEvent *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT) = 0;
        /**
         * Returns the requested event or NULL if there's no such event included in the log.
         * The given simulation time may not be included in the log.
         */
        virtual IEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT) = 0;

        /**
         * Finds the closest event log entry containing the given text.
         */
        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward) = 0;

        /**
         * Returns the approximate number of events present in the log.
         * This value may be less, equal or greater than the real number of events.
         */
        virtual long getApproximateNumberOfEvents() = 0;
        virtual IEvent *getApproximateEventAt(double percentage) = 0;

        /**
         * Prints initialization entries present before the first event.
         */
        virtual void printInitializationLogEntries(FILE *file = stdout) = 0;

    public:
        /**
         * Returns the event at the given instance. 0 means the parameter event will be returned.
         */
        virtual IEvent *getNeighbourEvent(IEvent *event, long distance = 1);

        /**
         * Returns true if the event with the given event number is included in the log.
         */
        virtual bool isIncludedInLog(long eventNumber) { return getEventForEventNumber(eventNumber) != NULL; }
        virtual IEvent *getFirstEventNotBeforeEventNumber(long eventNumber) { return getEventForEventNumber(eventNumber, LAST_OR_NEXT); }
        virtual IEvent *getLastEventNotAfterEventNumber(long eventNumber) { return getEventForEventNumber(eventNumber, FIRST_OR_PREVIOUS); }
        virtual IEvent *getFirstEventNotBeforeSimulationTime(simtime_t simulationTime) { return getEventForSimulationTime(simulationTime, LAST_OR_NEXT); }
        virtual IEvent *getLastEventNotAfterSimulationTime(simtime_t simulationTime) { return getEventForSimulationTime(simulationTime, FIRST_OR_PREVIOUS); }

        virtual double getApproximatePercentageForEventNumber(long eventNumber);

        /**
         * Prints all or only the events in the requested range from the log.
         * The given event numbers may not be included in the log.
         */
        virtual void printEvents(FILE *file = stdout, long fromEventNumber = -1, long toEventNumber = -1, bool outputEventLogMessages = true);
        /**
         * Prints initialization entries and calls printEvents.
         * The given event numbers may not be included in the log.
         */
        virtual void print(FILE *file = stdout, long fromEventNumber = -1, long toEventNumber = -1, bool outputInitializationEntries = true, bool outputEventLogMessages = true);
};

#endif
