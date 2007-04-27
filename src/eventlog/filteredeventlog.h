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
#include "patternmatcher.h"
#include "eventlogdefs.h"
#include "ieventlog.h"
#include "eventlog.h"
#include "filteredevent.h"

/**
 * This is a "view" of the EventLog, including only a subset of events and their dependencies. This class
 * uses EventLog by delegation so that multiple instances might share the same EventLog object.
 * TODO: filtered event log must save extra info in the exported log file to be able to reproduce the
 * same in memory model for filtered message dependencies. These can be done by adding new tags to E lines.
 */
class EVENTLOG_API FilteredEventLog : public IEventLog
{
    protected:
        IEventLog *eventLog;
        long approximateNumberOfEvents;
        double approximateMatchingEventRatio;

        // filter parameters
        long tracedEventNumber; // the event number from which causes and consequences are followed or -1
        long firstEventNumber; // the first event to be considered by the filter or -1
        long lastEventNumber; // the last event to be considered by the filter or -1
        std::vector<PatternMatcher> moduleNames;
        std::vector<PatternMatcher> moduleTypes;
        std::vector<int> moduleIds; // events outside these modules will be filtered out, NULL means include all
        std::vector<PatternMatcher> messageNames;
        std::vector<PatternMatcher> messageTypes;
        std::vector<long> messageIds;
        std::vector<long> messageTreeIds;
        std::vector<long> messageEncapsulationIds;
        std::vector<long> messageEncapsulationTreeIds;
        bool traceCauses; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
        bool traceConsequences; // only when tracedEventNumber is given
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
        FilteredEventLog(IEventLog *eventLog);
        ~FilteredEventLog();

    public:
        void setModuleNames(std::vector<const char *> &moduleNames) { setPatternMatchers(this->moduleNames, moduleNames); }
        void setModuleTypes(std::vector<const char *> &moduleTypes) { setPatternMatchers(this->moduleTypes, moduleTypes); }
        void setModuleIds(std::vector<int> &moduleIds) { this->moduleIds = moduleIds; }
        void setMessageNames(std::vector<const char *> &messageNames) { setPatternMatchers(this->messageNames, messageNames); }
        void setMessageTypes(std::vector<const char *> &messageTypes) { setPatternMatchers(this->messageTypes, messageTypes); }
        void setMessageIds(std::vector<long> &messageIds) { this->messageIds = messageIds; }
        void setMessageTreeIds(std::vector<long> &messageTreeIds) { this->messageTreeIds = messageTreeIds; }
        void setMessageEncapsulationIds(std::vector<long> &messageEncapsulationIds) { this->messageEncapsulationIds = messageEncapsulationIds; }
        void setMessageEncapsulationTreeIds(std::vector<long> &messageEncapsulationTreeIds) { this->messageEncapsulationTreeIds = messageEncapsulationTreeIds; }
        void setTracedEventNumber(long tracedEventNumber) { this->tracedEventNumber = tracedEventNumber; }
        void setTraceCauses(bool traceCauses) { this->traceCauses = traceCauses; }
        void setTraceConsequences(bool traceConsequences) { this->traceConsequences = traceConsequences; }
        void setFirstEventNumber(long firstEventNumber) { this->firstEventNumber = firstEventNumber; }
        void setLastEventNumber(long lastEventNumber) { this->lastEventNumber = lastEventNumber; }

        IEventLog *getEventLog() { return eventLog; }
        int getMaxCauseDepth() { return maxCauseDepth; }
        int getMaxConsequenceDepth() { return maxConsequenceDepth; }

        bool matchesFilter(IEvent *event);
        FilteredEvent *getMatchingEventInDirection(long startEventNumber, bool forward);
        FilteredEvent *getMatchingEventInDirection(long startEventNumber, long stopEventNumber, bool forward);

        // IEventLog interface
        virtual long getNumParsedEvents() { return eventLog->getNumParsedEvents(); }
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int index) { return eventLog->getModuleCreatedEntry(index); }
        virtual int getNumModuleCreatedEntries() { return eventLog->getNumModuleCreatedEntries(); }

        virtual FilteredEvent *getFirstEvent();
        virtual FilteredEvent *getLastEvent();
        virtual FilteredEvent *getNeighbourEvent(IEvent *event, long distance = 1);
        virtual FilteredEvent *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT);
        virtual FilteredEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT);

        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward);

        virtual long getApproximateNumberOfEvents();
        virtual double getApproximatePercentageForEventNumber(long eventNumber);
        virtual FilteredEvent *getApproximateEventAt(double percentage);

        virtual void printInitializationLogEntries(FILE *file = stdout) {  eventLog->printInitializationLogEntries(file); }

    protected:
        FilteredEvent *cacheFilteredEvent(long eventNumber);
        FilteredEvent *cacheFilteredEvent(FilteredEvent *filteredEvent);
        bool matchesEvent(IEvent *event);
        bool matchesDependency(IEvent *event);
        bool matchesPatterns(std::vector<PatternMatcher> &patterns, const char *str);
        template <typename T> bool matchesList(std::vector<T> &elements, T element);
        bool isCauseOfTracedEvent(IEvent *cause);
        bool isConsequenceOfTracedEvent(IEvent *consequence);
        double getApproximateMatchingEventRatio();
        void setPatternMatchers(std::vector<PatternMatcher> &patternMatchers, std::vector<const char *> &patterns);
};

#endif
