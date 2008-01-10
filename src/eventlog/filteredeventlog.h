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
#include "matchexpression.h"
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
        IEventLog *eventLog; // this will not be destructed because might be shared among multiple filtered event logs
        long approximateNumberOfEvents;
        double approximateMatchingEventRatio;

        // filter parameters
        long firstEventNumber; // the first event to be considered by the filter or -1
        long lastEventNumber; // the last event to be considered by the filter or -1

        bool enableModuleFilter;
        MatchExpression moduleExpression;
        std::vector<PatternMatcher> moduleNames;
        std::vector<PatternMatcher> moduleClassNames;
        std::vector<int> moduleIds; // events outside these modules will be filtered out, NULL means include all

        bool enableMessageFilter;
        MatchExpression messageExpression;
        std::vector<PatternMatcher> messageNames;
        std::vector<PatternMatcher> messageClassNames;
        std::vector<long> messageIds;
        std::vector<long> messageTreeIds;
        std::vector<long> messageEncapsulationIds;
        std::vector<long> messageEncapsulationTreeIds;

        long tracedEventNumber; // the event number from which causes and consequences are followed or -1
        bool traceCauses; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
        bool traceConsequences; // only when tracedEventNumber is given
        bool traceMessageReuses;
        bool traceSelfMessages;
        int maximumCauseDepth; // maximum depth of message dependencies considered when collecting causes
        int maximumNumberOfCauses; // maximum number of message dependencies collected
        int maximumConsequenceDepth; // maximum depth of message dependencies considered when collecting consequences
        int maximumNumberOfConsequences; // maximum number of message dependencies collected

		// internal state
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
        void setFirstEventNumber(long firstEventNumber) { this->firstEventNumber = firstEventNumber; }
        void setLastEventNumber(long lastEventNumber) { this->lastEventNumber = lastEventNumber; }

        void setEnableModuleFilter(bool enableModuleFilter) { this->enableModuleFilter = enableModuleFilter; }
        void setModuleExpression(const char *moduleExpression) { if (moduleExpression) this->moduleExpression.setPattern(moduleExpression, false, true, false); }
        void setModuleNames(std::vector<std::string> &moduleNames) { setPatternMatchers(this->moduleNames, moduleNames, true); }
        void setModuleClassNames(std::vector<std::string> &moduleClassNames) { setPatternMatchers(this->moduleClassNames, moduleClassNames); }
        void setModuleIds(std::vector<int> &moduleIds) { this->moduleIds = moduleIds; }

        void setEnableMessageFilter(bool enableMessageFilter) { this->enableMessageFilter = enableMessageFilter; }
        void setMessageExpression(const char *messageExpression) { if (messageExpression) this->messageExpression.setPattern(messageExpression, false, true, false); }
        void setMessageNames(std::vector<std::string> &messageNames) { setPatternMatchers(this->messageNames, messageNames); }
        void setMessageClassNames(std::vector<std::string> &messageClassNames) { setPatternMatchers(this->messageClassNames, messageClassNames); }
        void setMessageIds(std::vector<long> &messageIds) { this->messageIds = messageIds; }
        void setMessageTreeIds(std::vector<long> &messageTreeIds) { this->messageTreeIds = messageTreeIds; }
        void setMessageEncapsulationIds(std::vector<long> &messageEncapsulationIds) { this->messageEncapsulationIds = messageEncapsulationIds; }
        void setMessageEncapsulationTreeIds(std::vector<long> &messageEncapsulationTreeIds) { this->messageEncapsulationTreeIds = messageEncapsulationTreeIds; }

        void setTracedEventNumber(long tracedEventNumber) { this->tracedEventNumber = tracedEventNumber; }
        void setTraceCauses(bool traceCauses) { this->traceCauses = traceCauses; }
        void setTraceConsequences(bool traceConsequences) { this->traceConsequences = traceConsequences; }
        void setTraceSelfMessages(bool traceSelfMessages) { this->traceSelfMessages = traceSelfMessages; }
        void setTraceMessageReuses(bool traceMessageReuses) { this->traceMessageReuses = traceMessageReuses; }

        IEventLog *getEventLog() { return eventLog; }
        int getMaximumCauseDepth() { return maximumCauseDepth; }
        int getMaximumConsequenceDepth() { return maximumConsequenceDepth; }

        bool matchesFilter(IEvent *event);
        FilteredEvent *getMatchingEventInDirection(long startEventNumber, bool forward, long stopEventNumber = -1);
        FilteredEvent *getMatchingEventInDirection(IEvent *event, bool forward, long stopEventNumber = -1);
        std::vector<int> getSelectedModuleIds();

        // IEventLog interface
        virtual ProgressMonitor setProgressMonitor(ProgressMonitor progressMonitor) { return eventLog->setProgressMonitor(progressMonitor); }
        virtual void setProgressCallInterval(double seconds) { eventLog->setProgressCallInterval(seconds); }
        virtual void synchronize();
        virtual FileReader *getFileReader() { return eventLog->getFileReader(); }
        virtual long getNumParsedEvents() { return eventLog->getNumParsedEvents(); }
        virtual std::set<const char *>& getMessageNames() { return eventLog->getMessageNames(); }
        virtual std::set<const char *>& getMessageClassNames() { return eventLog->getMessageClassNames(); }
        virtual ModuleCreatedEntry *getModuleCreatedEntry(int index) { return eventLog->getModuleCreatedEntry(index); }
        virtual int getNumModuleCreatedEntries() { return eventLog->getNumModuleCreatedEntries(); }

        virtual bool isEmpty();
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
        /**
         * Caches the given filtered event in the event cache if not present yet.
         * The event must be known to match this filter.
         */
        FilteredEvent *cacheFilteredEvent(long eventNumber);

        /**
         * Checks whether the given event matches this filter.
         */
        bool matchesEvent(IEvent *event);
        bool matchesDependency(IEvent *event);
        bool matchesModuleCreatedEntry(ModuleCreatedEntry *moduleCreatedEntry);
        bool matchesBeginSendEntry(BeginSendEntry *beginSendEntry);
        bool matchesExpression(MatchExpression &matchExpression, EventLogEntry *eventLogEntry);
        bool matchesPatterns(std::vector<PatternMatcher> &patterns, const char *str);

        template <typename T> bool matchesList(std::vector<T> &elements, T element);
        bool isCauseOfTracedEvent(IEvent *cause);
        bool isConsequenceOfTracedEvent(IEvent *consequence);
        double getApproximateMatchingEventRatio();
        void setPatternMatchers(std::vector<PatternMatcher> &patternMatchers, std::vector<std::string> &patterns, bool dottedPath = false);

		void deleteAllocatedObjects();
		void clearInternalState(FileReader::FileChangedState change = FileReader::OVERWRITTEN);
};

#endif
