//=========================================================================
//  FILTEREDEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_FILTEREDEVENTLOG_H
#define __OMNETPP_EVENTLOG_FILTEREDEVENTLOG_H

#include <sstream>
#include <deque>
#include "common/patternmatcher.h"
#include "common/matchexpression.h"
#include "eventlogdefs.h"
#include "ieventlog.h"
#include "eventlog.h"
#include "filteredevent.h"

namespace omnetpp {
namespace eventlog {

/**
 * This is a "view" of the EventLog, including only a subset of events and their dependencies. This class
 * uses EventLog by delegation so that multiple instances might share the same EventLog object.
 * TODO: filtered eventlog must save extra info in the exported log file to be able to reproduce the
 * same in memory model for filtered message dependencies. These can be done by adding new tags to E lines.
 */
class EVENTLOG_API FilteredEventLog : public IEventLog
{
    typedef omnetpp::common::MatchExpression MatchExpression;
    typedef omnetpp::common::PatternMatcher PatternMatcher;

    protected:
        IEventLog *eventLog; // this will not be destructed because might be shared among multiple filtered eventlogs
        eventnumber_t approximateNumberOfEvents;
        double approximateMatchingEventRatio;

        // event parameters
        eventnumber_t firstConsideredEventNumber = -1; // the first event to be considered by the filter or -1
        eventnumber_t lastConsideredEventNumber = -1; // the last event to be considered by the filter or -1
        std::set<eventnumber_t> excludedEventNumbers;

        // module filter
        bool enableModuleFilter = false;
        MatchExpression moduleExpression;
        std::vector<PatternMatcher> moduleNames;
        std::vector<PatternMatcher> moduleClassNames;
        std::vector<PatternMatcher> moduleNedTypeNames;
        std::vector<int> moduleIds; // events outside these modules will be filtered out, nullptr means include all

        // message filter
        bool enableMessageFilter = false;
        MatchExpression messageExpression;
        std::vector<PatternMatcher> messageNames;
        std::vector<PatternMatcher> messageClassNames;
        std::vector<msgid_t> messageIds;
        std::vector<msgid_t> messageTreeIds;
        std::vector<msgid_t> messageEncapsulationIds;
        std::vector<msgid_t> messageEncapsulationTreeIds;

        // trace filter
        eventnumber_t tracedEventNumber = -1; // the event number from which causes and consequences are followed or -1
        bool traceCauses = true; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
        bool traceConsequences = true; // only when tracedEventNumber is given
        bool traceMessageReuses = true;
        bool traceSelfMessages = true;

        // collection limits
        bool collectMessageReuses = true; // if false message reuse dependencies will not be collected
        int maximumCauseDepth = 15; // maximum depth of message dependencies considered when collecting causes
        int maximumNumberOfCauses = 5; // maximum number of message dependencies collected for a single event
        int maximumCauseCollectionTime = 100; // in milliseconds
        int maximumConsequenceDepth = 15; // maximum depth of message dependencies considered when collecting consequences
        int maximumNumberOfConsequences = 5; // maximum number of message dependencies collected for a single event
        int maximumConsequenceCollectionTime = 100; // in milliseconds

        // internal state
        typedef std::map<eventnumber_t, FilteredEvent *> EventNumberToFilteredEventMap;
        EventNumberToFilteredEventMap eventNumberToFilteredEventMap;

        typedef std::map<eventnumber_t, bool> EventNumberToBooleanMap;
        EventNumberToBooleanMap eventNumberToFilterMatchesFlagMap; // a cache of whether the given event number matches the filter or not
        EventNumberToBooleanMap eventNumberToTraceableEventFlagMap;
        std::deque<eventnumber_t> unseenTracedEventCauseEventNumbers; // the remaining cause event number of the traced event that is to be visited
        std::deque<eventnumber_t> unseenTracedEventConsequenceEventNumbers; // the remaining consequence event number of the traced event that is to be visited

        FilteredEvent *firstMatchingEvent;
        FilteredEvent *lastMatchingEvent;

    public:
        FilteredEventLog(IEventLog *eventLog);
        virtual ~FilteredEventLog();

    public:
        eventnumber_t getFirstConsideredEventNumber() { return firstConsideredEventNumber; }
        void setFirstConsideredEventNumber(eventnumber_t firstConsideredEventNumber) { this->firstConsideredEventNumber = firstConsideredEventNumber; }
        eventnumber_t getLastConsideredEventNumber() { return lastConsideredEventNumber; }
        void setLastConsideredEventNumber(eventnumber_t lastConsideredEventNumber) { this->lastConsideredEventNumber = lastConsideredEventNumber; }

        void setExcludedEventNumbers(const std::vector<eventnumber_t>& excludedEventNumbers);

        bool getCollectMessageReuses() { return collectMessageReuses; }
        void setCollectMessageReuses(bool collectMessageReuses) { this->collectMessageReuses = collectMessageReuses; }

        void setEnableModuleFilter(bool enableModuleFilter) { this->enableModuleFilter = enableModuleFilter; }
        void setModuleExpression(const char *moduleExpression) { if (moduleExpression) this->moduleExpression.setPattern(moduleExpression, false, true, false); }
        void setModuleNames(std::vector<std::string> &moduleNames) { setPatternMatchers(this->moduleNames, moduleNames, true); }
        void setModuleClassNames(std::vector<std::string> &moduleClassNames) { setPatternMatchers(this->moduleClassNames, moduleClassNames); }
        void setModuleNedTypeNames(std::vector<std::string> &moduleNedTypeNames) { setPatternMatchers(this->moduleNedTypeNames, moduleNedTypeNames); }
        void setModuleIds(std::vector<int> &moduleIds) { this->moduleIds = moduleIds; }

        void setEnableMessageFilter(bool enableMessageFilter) { this->enableMessageFilter = enableMessageFilter; }
        void setMessageExpression(const char *messageExpression) { if (messageExpression) this->messageExpression.setPattern(messageExpression, false, true, false); }
        void setMessageNames(std::vector<std::string> &messageNames) { setPatternMatchers(this->messageNames, messageNames); }
        void setMessageClassNames(std::vector<std::string> &messageClassNames) { setPatternMatchers(this->messageClassNames, messageClassNames); }
        void setMessageIds(std::vector<msgid_t> &messageIds) { this->messageIds = messageIds; }
        void setMessageTreeIds(std::vector<msgid_t> &messageTreeIds) { this->messageTreeIds = messageTreeIds; }
        void setMessageEncapsulationIds(std::vector<msgid_t> &messageEncapsulationIds) { this->messageEncapsulationIds = messageEncapsulationIds; }
        void setMessageEncapsulationTreeIds(std::vector<msgid_t> &messageEncapsulationTreeIds) { this->messageEncapsulationTreeIds = messageEncapsulationTreeIds; }

        void setTracedEventNumber(eventnumber_t tracedEventNumber);
        void setTraceCauses(bool traceCauses) { this->traceCauses = traceCauses; }
        void setTraceConsequences(bool traceConsequences) { this->traceConsequences = traceConsequences; }
        void setTraceSelfMessages(bool traceSelfMessages) { this->traceSelfMessages = traceSelfMessages; }
        void setTraceMessageReuses(bool traceMessageReuses) { this->traceMessageReuses = traceMessageReuses; }

        IEventLog *getEventLog() { return eventLog; }

        int getMaximumCauseDepth() { return maximumCauseDepth; }
        void setMaximumCauseDepth(int maximumCauseDepth) { this->maximumCauseDepth = maximumCauseDepth; }
        int getMaximumNumberOfCauses() { return maximumNumberOfCauses; }
        void setMaximumNumberOfCauses(int maximumNumberOfCauses) { this->maximumNumberOfCauses = maximumNumberOfCauses; }
        int getMaximumCauseCollectionTime() { return maximumCauseCollectionTime; }
        void setMaximumCauseCollectionTime(int maximumCauseCollectionTime) { this->maximumCauseCollectionTime = maximumCauseCollectionTime; }

        int getMaximumConsequenceDepth() { return maximumConsequenceDepth; }
        void setMaximumConsequenceDepth(int maximumConsequenceDepth) { this->maximumConsequenceDepth = maximumConsequenceDepth; }
        int getMaximumNumberOfConsequences() { return maximumNumberOfConsequences; }
        void setMaximumNumberOfConsequences(int maximumNumberOfConsequences) { this->maximumNumberOfConsequences = maximumNumberOfConsequences; }
        int getMaximumConsequenceCollectionTime() { return maximumConsequenceCollectionTime; }
        void setMaximumConsequenceCollectionTime(int maximumConsequenceCollectionTime) { this->maximumConsequenceCollectionTime = maximumConsequenceCollectionTime; }

        bool matchesFilter(IEvent *event);
        bool matchesModuleDescriptionEntry(ModuleDescriptionEntry *moduleDescriptionEntry);
        FilteredEvent *getMatchingEventInDirection(eventnumber_t startEventNumber, bool forward, eventnumber_t stopEventNumber = -1);
        FilteredEvent *getMatchingEventInDirection(IEvent *event, bool forward, eventnumber_t stopEventNumber = -1);

        // IEventLog interface
        virtual ProgressMonitor setProgressMonitor(ProgressMonitor progressMonitor) override { return eventLog->setProgressMonitor(progressMonitor); }
        virtual void setProgressCallInterval(double seconds) override { eventLog->setProgressCallInterval(seconds); }
        virtual void progress() override { eventLog->progress(); }
        virtual void synchronize(FileReader::FileChange change) override;
        virtual FileReader *getFileReader() override { return eventLog->getFileReader(); }
        virtual EventLogEntryCache *getEventLogEntryCache() override { return eventLog->getEventLogEntryCache(); }
        virtual eventnumber_t getNumParsedEvents() override { return eventLog->getNumParsedEvents(); }
        virtual std::set<const char *>& getMessageNames() override { return eventLog->getMessageNames(); }
        virtual std::set<const char *>& getMessageClassNames() override { return eventLog->getMessageClassNames(); }
        virtual SimulationBeginEntry *getSimulationBeginEntry() override { return eventLog->getSimulationBeginEntry(); }
        virtual SimulationEndEntry *getSimulationEndEntry() override { return eventLog->getSimulationEndEntry(); }

        virtual bool isEmpty() override;

        virtual FilteredEvent *getFirstEvent() override;
        virtual eventnumber_t getFirstEventNumber() override { return getFirstEvent() != nullptr ? getFirstEvent()->getEventNumber() : -1; }
        virtual simtime_t getFirstSimulationTime() override { return getFirstEvent() != nullptr ? getFirstEvent()->getSimulationTime() : 0; }

        virtual FilteredEvent *getLastEvent() override;
        virtual eventnumber_t getLastEventNumber() override { return getLastEvent() != nullptr ? getLastEvent()->getEventNumber() : -1; }
        virtual simtime_t getLastSimulationTime() override { return getLastEvent() != nullptr ? getLastEvent()->getSimulationTime() : 0; }

        virtual FilteredEvent *getNeighbourEvent(IEvent *event, eventnumber_t distance = 1) override;
        virtual FilteredEvent *getEventForEventNumber(eventnumber_t eventNumber, MatchKind matchKind = EXACT, bool useCacheOnly = false) override;
        virtual FilteredEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT, bool useCacheOnly = false) override;

        virtual EventLogEntry *findEventLogEntry(EventLogEntry *start, const char *search, bool forward, bool caseSensitive) override;

        virtual Index *getFirstIndex() override { return eventLog->getFirstIndex(); }
        virtual Index *getLastIndex() override { return eventLog->getLastIndex(); }
        virtual Index *getIndex(eventnumber_t eventNumber, MatchKind matchKind = EXACT) override { return eventLog->getIndex(eventNumber, matchKind); }

        virtual Snapshot *getFirstSnapshot() override { return eventLog->getFirstSnapshot(); }
        virtual Snapshot *getLastSnapshot() override { return eventLog->getLastSnapshot(); }
        virtual Snapshot *getSnapshot(eventnumber_t eventNumber, MatchKind matchKind = EXACT) override { return eventLog->getSnapshot(eventNumber, matchKind); }

        virtual eventnumber_t getApproximateNumberOfEvents() override;
        virtual double getApproximatePercentageForEventNumber(eventnumber_t eventNumber) override;
        virtual FilteredEvent *getApproximateEventAt(double percentage) override;

        virtual void print(FILE *file = stdout, eventnumber_t fromEventNumber = -1, eventnumber_t toEventNumber = -1, bool outputEventLogMessages = true) override;

    protected:
        /**
         * Caches the given filtered event in the event cache if not present yet.
         * The event must be known to match this filter.
         */
        FilteredEvent *cacheFilteredEvent(eventnumber_t eventNumber);

        /**
         * Checks whether the given event matches this filter.
         */
        bool matchesEvent(IEvent *event);
        bool matchesDependency(IEvent *event);
        bool matchesBeginSendEntry(BeginSendEntry *beginSendEntry);
        bool matchesExpression(MatchExpression &matchExpression, EventLogEntry *eventLogEntry);
        bool matchesPatterns(std::vector<PatternMatcher> &patterns, const char *str);

        template <typename T> bool matchesList(std::vector<T> &elements, T element);
        bool isCauseOfTracedEvent(IEvent *cause);
        bool isConsequenceOfTracedEvent(IEvent *consequence);
        double getApproximateMatchingEventRatio();
        void setPatternMatchers(std::vector<PatternMatcher> &patternMatchers, std::vector<std::string> &patterns, bool dottedPath = false);

        void clearInternalState();
        void deleteConsequences();
        void deleteAllocatedObjects();
        bool isAncestorModuleDescriptionEntry(ModuleDescriptionEntry *ancestor, ModuleDescriptionEntry *descendant);
};

}  // namespace eventlog
}  // namespace omnetpp


#endif
