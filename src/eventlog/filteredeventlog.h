//=========================================================================
//  FILTEREDEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILTEREDEVENTLOG_H_
#define __FILTEREDEVENTLOG_H_

#include <sstream>
#include "eventlogdefs.h"
#include "ieventlog.h"
#include "eventlog.h"
#include "filteredevent.h"

/**
 * This is a "view" of the EventLog, including only a subset of events and their dependencies. This class
 * uses EventLog by delegation so that multiple instances might share the same EventLog object.
 */
class FilteredEventLog : public IEventLog
{
    protected:
        IEventLog *eventLog;
        long approximateNumberOfEvents;
        double approximateMatchingEventRatio;

        // filter parameters
        long tracedEventNumber; // the event number from which causes and consequences are followed or -1
        long firstEventNumber; // the first event to be considered by the filter or -1
        long lastEventNumber; // the last event to be considered by the filter or -1
        std::set<int> *includeModuleIds; // events outside these modules will be filtered out, NULL means include all
        bool includeCauses; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
        bool includeConsequences; // only when tracedEventNumber is given
        int maxCauseDepth; // maximum number of message dependencies considered when collecting causes
        int maxConsequenceDepth; // maximum number of message dependencies considered when collecting consequences

        // state
        long numEventsApproximation;

        typedef std::map<long, FilteredEvent *> EventNumberToFilteredEventMap;
        EventNumberToFilteredEventMap eventNumberToFilteredEventMap;

        typedef std::map<long, bool> EventNumberToBooleanMap;
        EventNumberToBooleanMap eventNumberToFilterMatchesFlagMap; // a cache of whether the given event number matches the filter or not
        EventNumberToBooleanMap eventNumberToTraceableEventFlagMap;

        FilteredEvent *firstMatchingEvent;
        FilteredEvent *lastMatchingEvent;

    public:
        FilteredEventLog(IEventLog *eventLog,
                         std::set<int> *includeModuleIds,
                         long tracedEventNumber = -1,
                         bool includeCauses = false,
                         bool includeConsequences = false,
                         long firstEventNumber = -1,
                         long lastEventNumber = -1);
        ~FilteredEventLog();

    public:
        IEventLog *getEventLog() { return eventLog; }
        int getMaxCauseDepth() { return maxCauseDepth; }
        int getMaxConsequenceDepth() { return maxConsequenceDepth; }

        bool matchesFilter(IEvent *event);
        FilteredEvent *getMatchingEventInDirection(long startEventNumber, bool forward);

        // IEventLog interface
        virtual long getNumParsedEvents() { return eventLog->getNumParsedEvents(); }
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int index) { return eventLog->getModuleCreatedEntry(index); }
        virtual int getNumModuleCreatedEntries() { return eventLog->getNumModuleCreatedEntries(); }

        virtual FilteredEvent *getFirstEvent();
        virtual FilteredEvent *getLastEvent();
        virtual FilteredEvent *getNeighbourEvent(IEvent *event, long distance = 1);
        virtual FilteredEvent *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        virtual FilteredEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);

        virtual long getApproximateNumberOfEvents();
        virtual double getApproximatePercentageForEventNumber(long eventNumber);
        virtual FilteredEvent *getApproximateEventAt(double percentage);

        virtual void printInitializationLogEntries(FILE *file = stdout) {  eventLog->printInitializationLogEntries(file); }

    protected:
        FilteredEvent *cacheFilteredEvent(long eventNumber);
        FilteredEvent *cacheFilteredEvent(FilteredEvent *filteredEvent);
        bool matchesEvent(IEvent *event);
        bool matchesDependency(IEvent *event);
        bool isCauseOfTracedEvent(IEvent *cause);
        bool isConsequenceOfTracedEvent(IEvent *consequence);
        double getApproximateMatchingEventRatio();
};

#endif
