//=========================================================================
//  IEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_IEVENTLOG_H
#define __OMNETPP_EVENTLOG_IEVENTLOG_H

#include <sstream>
#include <set>
#include <vector>
#include "common/filereader.h"
#include "eventlogentrycache.h"
#include "ievent.h"

namespace omnetpp {
namespace eventlog {

class IEventLog;
class Index;
class Snapshot;

class EVENTLOG_API ProgressMonitor
{
    public:
        typedef void (*MonitorFunction)(IEventLog *, void *);
        MonitorFunction monitorFunction;
        void *data;  // needs to be public due to swig wrapper code

    public:
        ProgressMonitor(MonitorFunction monitorFunction=nullptr, void *data=nullptr) : monitorFunction(monitorFunction), data(data) {}
        void progress(IEventLog *eventLog) { if (monitorFunction) monitorFunction(eventLog, data); }
};

// TODO: remove those methods which are not really different among implementations, simplify class design
class EVENTLOG_API IEventLog
{
    protected:
        // Remembers the last IEvent returned by getNeighbourEvent so that subsequent calls might return much faster.
        eventnumber_t lastNeighbourEventNumber;
        IEvent *lastNeighbourEvent;

    public:
        IEventLog();
        virtual ~IEventLog() {}

        /**
         * Sets the progress monitor which will be notified when a long running operation has some progress.
         */
        virtual ProgressMonitor setProgressMonitor(ProgressMonitor progressMonitor) = 0;

        /**
         * Set the minimum interval between progress callbacks for long running eventlog operations.
         */
        virtual void setProgressCallInterval(double seconds) = 0;

        /**
         * Progress notification. May be used to cancel long running operations.
         */
        virtual void progress() = 0;

        /**
         * Synchronizes state when the underlying log file has been changed.
         * Either new events were appended or the whole file has been overwritten.
         */
        virtual void synchronize(FileReader::FileChange change);

        /**
         * Returns the file reader used to read in events.
         */
        virtual FileReader *getFileReader() = 0;

        /**
         * Returns the eventlog entry cache for the whole eventlog.
         */
        virtual EventLogEntryCache *getEventLogEntryCache() = 0;

        /**
         * Returns the number of events parsed so far.
         */
        virtual eventnumber_t getNumParsedEvents() = 0;

        /**
         * Returns the message names parsed so far.
         */
        virtual std::set<const char *>& getMessageNames() = 0;

        /**
         * Returns the message class names parsed so far.
         */
        virtual std::set<const char *>& getMessageClassNames() = 0;

        /**
         * Returns the eventlog entry describing the beginning of the simulation.
         */
        virtual SimulationBeginEntry *getSimulationBeginEntry() = 0;

        /**
         * Returns the eventlog entry describing the end of the simulation.
         */
        virtual SimulationEndEntry *getSimulationEndEntry() = 0;

        /**
         * Returns true if the eventlog does not contain any events.
         */
        virtual bool isEmpty() { return !getFirstEvent(); }

        /**
         * Returns the first event number or -1 if the log is empty.
         */
        virtual eventnumber_t getFirstEventNumber() = 0;

        /**
         * Returns the first simulation time or 0 if the log is empty.
         */
        virtual simtime_t getFirstSimulationTime() = 0;

        /**
         * Returns the first event or nullptr if the log is empty.
         */
        virtual IEvent *getFirstEvent() = 0;

        /**
         * Returns the last simulation time or 0 if the log is empty.
         */
        virtual simtime_t getLastSimulationTime() = 0;

        /**
         * Returns the last event number or -1 if the log is empty.
         */
        virtual eventnumber_t getLastEventNumber() = 0;

        /**
         * Returns the last event or nullptr if the log is empty.
         */
        virtual IEvent *getLastEvent() = 0;

        /**
         * Returns the requested event or nullptr if there is no such event included in the log.
         * The given event number may not be included in the log.
         */
        virtual IEvent *getEventForEventNumber(eventnumber_t eventNumber, MatchKind matchKind = EXACT, bool useCacheOnly = false) = 0;

        /**
         * Returns the requested event or nullptr if there is no such event included in the log.
         * The given simulation time may not be included in the log.
         */
        virtual IEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT, bool useCacheOnly = false) = 0;

        /**
         * Finds the closest eventlog entry containing the given text.
         */
        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward, bool caseSensitive) = 0;

        /**
         * Returns the first index or nullptr.
         */
        virtual Index *getFirstIndex() = 0;

        /**
         * Returns the last index or nullptr.
         */
        virtual Index *getLastIndex() = 0;

        /**
         * Returns the index with the provided event or nullptr if none found.
         * Returns the index that doesn't have the provided event number when searching for the next or the previous.
         */
        virtual Index *getIndex(eventnumber_t eventNumber, MatchKind matchKind = EXACT) = 0;

        /**
         * Returns the first snapshot or nullptr.
         */
        virtual Snapshot *getFirstSnapshot() = 0;

        /**
         * Returns the last snapshot or nullptr.
         */
        virtual Snapshot *getLastSnapshot() = 0;

        /**
         * Returns the snapshot with the provided event number or nullptr if none found.
         * Returns the snapshot that doesn't have the provided event number when searching for the next or the previous.
         */
        virtual Snapshot *getSnapshot(eventnumber_t eventNumber, MatchKind matchKind = EXACT) = 0;

        /**
         * Returns the approximate number of events present in the log.
         * This value may be less, equal or greater than the real number of events if there are many.
         */
        virtual eventnumber_t getApproximateNumberOfEvents() = 0;

        /**
         * Returns an event approximately at the given percentage in terms of eventlog size.
         */
        virtual IEvent *getApproximateEventAt(double percentage) = 0;

        /**
         * Prints the contents into specified file.
         * The given event numbers may not be present in the log.
         */
        virtual void print(FILE *file = stdout, eventnumber_t fromEventNumber = -1, eventnumber_t toEventNumber = -1, bool outputEventLogMessages = true) = 0;

        /**
         * Returns the event at the given distance. 0 means the parameter event will be returned.
         */
        virtual IEvent *getNeighbourEvent(IEvent *event, eventnumber_t distance = 1);

        /**
         * Returns true if the event with the given event number is included in the log.
         */
        virtual bool isIncludedInLog(eventnumber_t eventNumber) { return getEventForEventNumber(eventNumber) != nullptr; }
        virtual IEvent *getFirstEventNotBeforeEventNumber(eventnumber_t eventNumber) { return getEventForEventNumber(eventNumber, LAST_OR_NEXT); }
        virtual IEvent *getLastEventNotAfterEventNumber(eventnumber_t eventNumber) { return getEventForEventNumber(eventNumber, FIRST_OR_PREVIOUS); }
        virtual IEvent *getFirstEventNotBeforeSimulationTime(simtime_t simulationTime) { return getEventForSimulationTime(simulationTime, LAST_OR_NEXT); }
        virtual IEvent *getLastEventNotAfterSimulationTime(simtime_t simulationTime) { return getEventForSimulationTime(simulationTime, FIRST_OR_PREVIOUS); }

        virtual double getApproximatePercentageForEventNumber(eventnumber_t eventNumber);

    protected:
        void clearInternalState();
};

}  // namespace eventlog
}  // namespace omnetpp


#endif
