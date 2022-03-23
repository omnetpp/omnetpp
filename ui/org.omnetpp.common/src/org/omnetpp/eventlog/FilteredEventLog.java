package org.omnetpp.eventlog;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.TreeMap;
import java.util.TreeSet;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;
import org.omnetpp.eventlog.entry.SimulationBeginEntry;
import org.omnetpp.eventlog.entry.SimulationEndEntry;

/**
 * This is a "view" of the EventLog, including only a subset of events and their dependencies. This class
 * uses EventLog by delegation so that multiple instances might share the same EventLog object.
 * TODO: filtered eventlog must save extra info in the exported log file to be able to reproduce the
 * same in memory model for filtered message dependencies. These can be done by adding new tags to E lines.
 */
public class FilteredEventLog extends EventLogBase implements IEventLog
{
    protected IEventLog eventLog; // this will not be destructed because might be shared among multiple filtered eventlogs
    protected long approximateNumberOfEvents = -1;
    protected double approximateMatchingEventRatio;

    // event parameters
    protected long firstConsideredEventNumber = -1; // the first event to be considered by the filter or -1
    protected long lastConsideredEventNumber = -1; // the last event to be considered by the filter or -1
    protected TreeSet<Long> excludedEventNumbers = new TreeSet<Long>();

    // module filter
    protected boolean enableModuleFilter = false;
    protected MatchExpression moduleExpression = new MatchExpression();
    protected ArrayList<PatternMatcher> moduleNames = new ArrayList<PatternMatcher>();
    protected ArrayList<PatternMatcher> moduleClassNames = new ArrayList<PatternMatcher>();
    protected ArrayList<PatternMatcher> moduleNedTypeNames = new ArrayList<PatternMatcher>();
    protected ArrayList<Integer> moduleIds = new ArrayList<Integer>(); // events outside these modules will be filtered out, null means include all

    // message filter
    protected boolean enableMessageFilter = false;
    protected MatchExpression messageExpression = new MatchExpression();
    protected ArrayList<PatternMatcher> messageNames = new ArrayList<PatternMatcher>();
    protected ArrayList<PatternMatcher> messageClassNames = new ArrayList<PatternMatcher>();
    protected ArrayList<Long> messageIds = new ArrayList<Long>();
    protected ArrayList<Long> messageTreeIds = new ArrayList<Long>();
    protected ArrayList<Long> messageEncapsulationIds = new ArrayList<Long>();
    protected ArrayList<Long> messageEncapsulationTreeIds = new ArrayList<Long>();

    // trace filter
    protected long tracedEventNumber = -1; // the event number from which causes and consequences are followed or -1
    protected boolean traceCauses = true; // only when tracedEventNumber is given, includes events which cause the traced event even if through a chain of filtered events
    protected boolean traceConsequences = true; // only when tracedEventNumber is given
    protected boolean traceMessageReuses = true;
    protected boolean traceSelfMessages = true;

    // collection limits
    protected boolean collectMessageReuses = true; // if false message reuse dependencies will not be collected
    protected int maximumCauseDepth = 15; // maximum depth of message dependencies considered when collecting causes
    protected int maximumNumberOfCauses = 5; // maximum number of message dependencies collected for a single event
    protected int maximumCauseCollectionTime = 100; // in milliseconds
    protected int maximumConsequenceDepth = 15; // maximum depth of message dependencies considered when collecting consequences
    protected int maximumNumberOfConsequences = 5; // maximum number of message dependencies collected for a single event
    protected int maximumConsequenceCollectionTime = 100; // in milliseconds

    // internal state
    protected TreeMap<Long, FilteredEvent > eventNumberToFilteredEventMap = new TreeMap<Long, FilteredEvent >();

    protected TreeMap<Long, Boolean> eventNumberToFilterMatchesFlagMap = new TreeMap<Long, Boolean>(); // a cache of whether the given event number matches the filter or not
    protected TreeMap<Long, Boolean> eventNumberToTraceableEventFlagMap = new TreeMap<Long, Boolean>();
    protected LinkedList<Long> unseenTracedEventCauseEventNumbers = new LinkedList<Long>(); // the remaining cause event number of the traced event that is to be visited
    protected LinkedList<Long> unseenTracedEventConsequenceEventNumbers = new LinkedList<Long>(); // the remaining consequence event number of the traced event that is to be visited

    protected FilteredEvent firstMatchingEvent;
    protected FilteredEvent lastMatchingEvent;

    protected JavaMatchableObject javaMatchableObject = new JavaMatchableObject();

    public FilteredEventLog(IEventLog eventLog) {
        this.eventLog = eventLog;
        setModuleExpression("");
        setMessageExpression("");
        clearInternalState();
    }

    public void close() {
        deleteAllocatedObjects();
    }

    public final long getFirstConsideredEventNumber() {
        return firstConsideredEventNumber;
    }

    public final void setFirstConsideredEventNumber(long firstConsideredEventNumber) {
        this.firstConsideredEventNumber = firstConsideredEventNumber;
    }

    public final long getLastConsideredEventNumber() {
        return lastConsideredEventNumber;
    }

    public final void setLastConsideredEventNumber(long lastConsideredEventNumber) {
        this.lastConsideredEventNumber = lastConsideredEventNumber;
    }

    public void setExcludedEventNumbers(final ArrayList<Long> excludedEventNumbers) {
        for (var eventNumber : excludedEventNumbers)
            this.excludedEventNumbers.add(eventNumber);
    }

    public final boolean getCollectMessageReuses() {
        return collectMessageReuses;
    }

    public final void setCollectMessageReuses(boolean collectMessageReuses) {
        this.collectMessageReuses = collectMessageReuses;
    }

    public final void setEnableModuleFilter(boolean enableModuleFilter) {
        this.enableModuleFilter = enableModuleFilter;
    }

    public final void setModuleExpression(String moduleExpression) {
        if (moduleExpression != null)
            this.moduleExpression.setPattern(moduleExpression, false, true, false);
    }

    public final void setModuleNames(ArrayList<String> moduleNames) {
        setPatternMatchers(this.moduleNames, moduleNames, true);
    }

    public final void setModuleClassNames(ArrayList<String> moduleClassNames) {
        setPatternMatchers(this.moduleClassNames, moduleClassNames);
    }

    public final void setModuleNedTypeNames(ArrayList<String> moduleNedTypeNames) {
        setPatternMatchers(this.moduleNedTypeNames, moduleNedTypeNames);
    }

    public final void setModuleIds(ArrayList<Integer> moduleIds) {
        this.moduleIds = new ArrayList<Integer>(moduleIds);
    }

    public final void setEnableMessageFilter(boolean enableMessageFilter) {
        this.enableMessageFilter = enableMessageFilter;
    }

    public final void setMessageExpression(String messageExpression) {
        if (messageExpression != null)
            this.messageExpression.setPattern(messageExpression, false, true, false);
    }

    public final void setMessageNames(ArrayList<String> messageNames) {
        setPatternMatchers(this.messageNames, messageNames);
    }

    public final void setMessageClassNames(ArrayList<String> messageClassNames) {
        setPatternMatchers(this.messageClassNames, messageClassNames);
    }

    public final void setMessageIds(ArrayList<Long> messageIds) {
        this.messageIds = new ArrayList<Long>(messageIds);
    }

    public final void setMessageTreeIds(ArrayList<Long> messageTreeIds) {
        this.messageTreeIds = new ArrayList<Long>(messageTreeIds);
    }

    public final void setMessageEncapsulationIds(ArrayList<Long> messageEncapsulationIds) {
        this.messageEncapsulationIds = new ArrayList<Long>(messageEncapsulationIds);
    }

    public final void setMessageEncapsulationTreeIds(ArrayList<Long> messageEncapsulationTreeIds) {
        this.messageEncapsulationTreeIds = new ArrayList<Long>(messageEncapsulationTreeIds);
    }

    public void setTracedEventNumber(long tracedEventNumber) {
        Assert.isTrue(this.tracedEventNumber == -1);
        this.tracedEventNumber = tracedEventNumber;
        unseenTracedEventCauseEventNumbers.add(tracedEventNumber);
        unseenTracedEventConsequenceEventNumbers.add(tracedEventNumber);
    }

    public final void setTraceCauses(boolean traceCauses) {
        this.traceCauses = traceCauses;
    }

    public final void setTraceConsequences(boolean traceConsequences) {
        this.traceConsequences = traceConsequences;
    }

    public final void setTraceSelfMessages(boolean traceSelfMessages) {
        this.traceSelfMessages = traceSelfMessages;
    }

    public final void setTraceMessageReuses(boolean traceMessageReuses) {
        this.traceMessageReuses = traceMessageReuses;
    }

    public final IEventLog getEventLog() {
        return eventLog;
    }

    public final int getMaximumCauseDepth() {
        return maximumCauseDepth;
    }

    public final void setMaximumCauseDepth(int maximumCauseDepth) {
        this.maximumCauseDepth = maximumCauseDepth;
    }

    public final int getMaximumNumberOfCauses() {
        return maximumNumberOfCauses;
    }

    public final void setMaximumNumberOfCauses(int maximumNumberOfCauses) {
        this.maximumNumberOfCauses = maximumNumberOfCauses;
    }

    public final int getMaximumCauseCollectionTime() {
        return maximumCauseCollectionTime;
    }

    public final void setMaximumCauseCollectionTime(int maximumCauseCollectionTime) {
        this.maximumCauseCollectionTime = maximumCauseCollectionTime;
    }

    public final int getMaximumConsequenceDepth() {
        return maximumConsequenceDepth;
    }

    public final void setMaximumConsequenceDepth(int maximumConsequenceDepth) {
        this.maximumConsequenceDepth = maximumConsequenceDepth;
    }

    public final int getMaximumNumberOfConsequences() {
        return maximumNumberOfConsequences;
    }

    public final void setMaximumNumberOfConsequences(int maximumNumberOfConsequences) {
        this.maximumNumberOfConsequences = maximumNumberOfConsequences;
    }

    public final int getMaximumConsequenceCollectionTime() {
        return maximumConsequenceCollectionTime;
    }

    public final void setMaximumConsequenceCollectionTime(int maximumConsequenceCollectionTime) {
        this.maximumConsequenceCollectionTime = maximumConsequenceCollectionTime;
    }

    public final boolean matchesFilter(IEvent event) {
        Assert.isTrue(event != null);
        var it = eventNumberToFilterMatchesFlagMap.get(event.getEventNumber());
        // if cached, return it
        if (it != null)
            return it;
        // System.out.println("*** Matching filter to event: %ld\n", event->getEventNumber());
        boolean matches = matchesEvent(event) && matchesDependency(event);
        eventNumberToFilterMatchesFlagMap.put(event.getEventNumber(), matches);
        return matches;
    }

    public final boolean matchesModuleDescriptionEntry(ModuleDescriptionEntry moduleDescriptionEntry) {
        return matchesExpression(moduleExpression, moduleDescriptionEntry) || matchesPatterns(moduleNames, moduleDescriptionEntry.fullName) || matchesPatterns(moduleClassNames, moduleDescriptionEntry.moduleClassName) || matchesPatterns(moduleNedTypeNames, moduleDescriptionEntry.nedTypeName) || matchesList(moduleIds, moduleDescriptionEntry.moduleId);
    }

    public FilteredEvent getMatchingEventInDirection(long eventNumber, boolean forward) {
        return getMatchingEventInDirection(eventNumber, forward, -1);
    }

    public FilteredEvent getMatchingEventInDirection(long eventNumber, boolean forward, long stopEventNumber) {
        Assert.isTrue(eventNumber >= 0);
        IEvent event = eventLog.getEventForEventNumber(eventNumber);
        return getMatchingEventInDirection(event, forward, stopEventNumber);
    }

    public FilteredEvent getMatchingEventInDirection(IEvent event, boolean forward) {
        return getMatchingEventInDirection(event, forward, -1);
    }

    public FilteredEvent getMatchingEventInDirection(IEvent event, boolean forward, long stopEventNumber) {
        Assert.isTrue(event != null);
        // optimization
        if (forward) {
            if (firstMatchingEvent != null && event.getEventNumber() < firstMatchingEvent.getEventNumber()) {
                if (stopEventNumber != -1 && stopEventNumber < firstMatchingEvent.getEventNumber())
                    return null;
                else
                    return firstMatchingEvent;
            }
            if (firstConsideredEventNumber != -1 && event.getEventNumber() < firstConsideredEventNumber)
                event = eventLog.getEventForEventNumber(firstConsideredEventNumber, MatchKind.LAST_OR_NEXT);
        }
        else {
            if (lastMatchingEvent != null && lastMatchingEvent.getEventNumber() < event.getEventNumber())
            {
                if (stopEventNumber != -1 && lastMatchingEvent.getEventNumber() < stopEventNumber)
                    return null;
                else
                    return lastMatchingEvent;
            }
            if (lastConsideredEventNumber != -1 && lastConsideredEventNumber < event.getEventNumber())
                event = eventLog.getEventForEventNumber(lastConsideredEventNumber, MatchKind.FIRST_OR_PREVIOUS);
        }
        Assert.isTrue(event != null);
        // LONG RUNNING OPERATION
        // if none of firstEventNumber, lastEventNumber, stopEventNumber is set this might take a while
        while (event != null) {
            eventLog.progress();
            long eventNumber = event.getEventNumber();
            if (matchesFilter(event))
                return cacheFilteredEvent(eventNumber);
            if (forward) {
                eventNumber++;
                event = event.getNextEvent();
                if (lastConsideredEventNumber != -1 && eventNumber > lastConsideredEventNumber)
                    return null;
                if (stopEventNumber != -1 && eventNumber > stopEventNumber)
                    return null;
            }
            else {
                eventNumber--;
                event = event.getPreviousEvent();
                if (firstConsideredEventNumber != -1 && eventNumber < firstConsideredEventNumber)
                    return null;
                if (stopEventNumber != -1 && eventNumber < stopEventNumber)
                    return null;
            }
        }
        return null;
    }

    // IEventLog interface

    @Override
    public void setProgressMonitor(IProgressMonitor progressMonitor) {
        eventLog.setProgressMonitor(progressMonitor);
    }

    @Override
    public void setProgressCallInterval(double seconds) {
        eventLog.setProgressCallInterval(seconds);
    }

    @Override
    public void progress() {
        eventLog.progress();
    }

    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    eventLog.synchronize(change);
                    deleteAllocatedObjects();
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    eventLog.synchronize(change);
                    for (var it : eventNumberToFilteredEventMap.values())
                        it.synchronize(change);
                    if (lastMatchingEvent != null) {
                        long eventNumber = lastMatchingEvent.getEventNumber();
                        eventNumberToFilteredEventMap.remove(eventNumber);
                        eventNumberToFilterMatchesFlagMap.remove(eventNumber);
                        eventNumberToTraceableEventFlagMap.remove(eventNumber);
                        if (firstMatchingEvent == lastMatchingEvent)
                            firstMatchingEvent = null;
                        lastMatchingEvent = null;
                    }
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    @Override
    public FileReader getFileReader() {
        return eventLog.getFileReader();
    }

    @Override
    public EventLogEntryCache getEventLogEntryCache() {
        return eventLog.getEventLogEntryCache();
    }

    @Override
    public long getNumParsedEvents() {
        return eventLog.getNumParsedEvents();
    }

    @Override
    public TreeSet<String> getMessageNames() {
        return eventLog.getMessageNames();
    }

    @Override
    public TreeSet<String> getMessageClassNames() {
        return eventLog.getMessageClassNames();
    }

    @Override
    public SimulationBeginEntry getSimulationBeginEntry() {
        return eventLog.getSimulationBeginEntry();
    }

    @Override
    public SimulationEndEntry getSimulationEndEntry() {
        return eventLog.getSimulationEndEntry();
    }

    @Override
    public boolean isEmpty() {
        if (tracedEventNumber != -1) {
            IEvent event = eventLog.getEventForEventNumber(tracedEventNumber);
            if (event != null && matchesFilter(event))
                return false;
        }
        return super.isEmpty();
    }

    @Override
    public FilteredEvent getFirstEvent() {
        if (firstMatchingEvent == null && !eventLog.isEmpty()) {
            long startEventNumber = firstConsideredEventNumber == -1 ? eventLog.getFirstEvent().getEventNumber() : Math.max(eventLog.getFirstEvent().getEventNumber(), firstConsideredEventNumber);
            firstMatchingEvent = getMatchingEventInDirection(startEventNumber, true);
        }
        return firstMatchingEvent;
    }

    @Override
    public long getFirstEventNumber() {
        return getFirstEvent() != null ? getFirstEvent().getEventNumber() : -1;
    }

    @Override
    public BigDecimal getFirstSimulationTime() {
        return getFirstEvent() != null ? getFirstEvent().getSimulationTime() : BigDecimal.ZERO;
    }

    @Override
    public FilteredEvent getLastEvent() {
        if (lastMatchingEvent == null && !eventLog.isEmpty()) {
            long startEventNumber = lastConsideredEventNumber == -1 ? eventLog.getLastEvent().getEventNumber() : Math.min(eventLog.getLastEvent().getEventNumber(), lastConsideredEventNumber);
            lastMatchingEvent = getMatchingEventInDirection(startEventNumber, false);
        }
        return lastMatchingEvent;
    }

    @Override
    public long getLastEventNumber() {
        return getLastEvent() != null ? getLastEvent().getEventNumber() : -1;
    }

    @Override
    public BigDecimal getLastSimulationTime() {
        return getLastEvent() != null ? getLastEvent().getSimulationTime() : BigDecimal.ZERO;
    }

    public FilteredEvent getEventForEventNumber(long eventNumber, MatchKind matchKind, boolean useCacheOnly) {
        Assert.isTrue(eventNumber >= 0);
        var it = eventNumberToFilteredEventMap.get(eventNumber);
        if (it != null)
            return it;
        IEvent event = eventLog.getEventForEventNumber(eventNumber, matchKind, useCacheOnly);
        if (event != null) {
            switch (matchKind) {
                case EXACT:
                    if (matchesFilter(event))
                        return cacheFilteredEvent(event.getEventNumber());
                    break;
                case FIRST_OR_PREVIOUS:
                    if (event.getEventNumber() == eventNumber && matchesFilter(event))
                        return cacheFilteredEvent(event.getEventNumber());
                    else if (!useCacheOnly)
                        return getMatchingEventInDirection(event, false);
                    break;
                case FIRST_OR_NEXT:
                    if (!useCacheOnly)
                        return getMatchingEventInDirection(event, true);
                    break;
                case LAST_OR_PREVIOUS:
                    if (!useCacheOnly)
                        return getMatchingEventInDirection(event, false);
                    break;
                case LAST_OR_NEXT:
                    if (event.getEventNumber() == eventNumber && matchesFilter(event))
                        return cacheFilteredEvent(event.getEventNumber());
                    else if (!useCacheOnly)
                        return getMatchingEventInDirection(event, true);
                    break;
            }
        }
        return null;
    }

    public FilteredEvent getEventForSimulationTime(BigDecimal simulationTime, MatchKind matchKind, boolean useCacheOnly) {
        IEvent event = eventLog.getEventForSimulationTime(simulationTime, matchKind, useCacheOnly);
        if (event != null) {
            switch (matchKind) {
                case EXACT:
                    if (matchesFilter(event))
                        return cacheFilteredEvent(event.getEventNumber());
                    break;
                case FIRST_OR_PREVIOUS:
                    if (!useCacheOnly) {
                        if (event.getSimulationTime() == simulationTime) {
                            IEvent lastEvent = eventLog.getEventForSimulationTime(simulationTime, MatchKind.LAST_OR_NEXT);
                            FilteredEvent matchingEvent = getMatchingEventInDirection(event, true, lastEvent.getEventNumber());
                            if (matchingEvent != null)
                                return matchingEvent;
                        }
                        return getMatchingEventInDirection(event, false);
                    }
                    break;
                case FIRST_OR_NEXT:
                    if (!useCacheOnly)
                        return getMatchingEventInDirection(event, true);
                    break;
                case LAST_OR_PREVIOUS:
                    if (!useCacheOnly)
                        return getMatchingEventInDirection(event, false);
                    break;
                case LAST_OR_NEXT:
                    if (!useCacheOnly) {
                        if (event.getSimulationTime() == simulationTime) {
                            IEvent firstEvent = eventLog.getEventForSimulationTime(simulationTime, MatchKind.FIRST_OR_PREVIOUS);
                            FilteredEvent matchingEvent = getMatchingEventInDirection(event, false, firstEvent.getEventNumber());
                            if (matchingEvent != null)
                                return matchingEvent;
                        }
                        return getMatchingEventInDirection(event, true);
                    }
                    break;
            }
        }
        return null;
    }

    @Override
    public EventLogEntry findEventLogEntry(EventLogEntry start, String search, boolean forward, boolean caseSensitive) {
        EventLogEntry eventLogEntry = start;
        do {
            eventLogEntry = eventLog.findEventLogEntry(eventLogEntry, search, forward, caseSensitive);
        }
        while (eventLogEntry != null && !matchesFilter(eventLogEntry.getEvent()));
        return eventLogEntry;
    }

    @Override
    public Index getFirstIndex() {
        return eventLog.getFirstIndex();
    }

    @Override
    public Index getLastIndex() {
        return eventLog.getLastIndex();
    }

    @Override
    public Index getIndex(long eventNumber) {
        return getIndex(eventNumber, MatchKind.EXACT);
    }

    @Override
    public Index getIndex(long eventNumber, MatchKind matchKind) {
        return eventLog.getIndex(eventNumber, matchKind);
    }

    @Override
    public Snapshot getFirstSnapshot() {
        return eventLog.getFirstSnapshot();
    }

    @Override
    public Snapshot getLastSnapshot() {
        return eventLog.getLastSnapshot();
    }

    @Override
    public Snapshot getSnapshot(long eventNumber) {
        return getSnapshot(eventNumber, MatchKind.EXACT);
    }

    @Override
    public Snapshot getSnapshot(long eventNumber, MatchKind matchKind) {
        return eventLog.getSnapshot(eventNumber, matchKind);
    }

    @Override
    public long getApproximateNumberOfEvents() {
        if (approximateNumberOfEvents == -1) {
            if (tracedEventNumber != -1) {
                // TODO: this is clearly not good and should return a much better approximation
                // TODO: maybe start from traced event number and go forward/backward and return approximation based on that?
                if (firstConsideredEventNumber != -1 && lastConsideredEventNumber != -1)
                    return lastConsideredEventNumber - firstConsideredEventNumber;
                else
                    return 1000;
            }
            else {
                // TODO: what if filter is event range limited?
                FilteredEvent firstEvent = getFirstEvent();
                FilteredEvent lastEvent = getLastEvent();

                if (firstEvent == null)
                    approximateNumberOfEvents = 0;
                else {
                    long beginOffset = firstEvent.getBeginOffset();
                    long endOffset = lastEvent.getEndOffset();
                    int sumMatching = 0;
                    int sumNotMatching = 0;
                    int count = 0;
                    int eventCount = 100;
                    // TODO: perhaps it would be better to read in random events
                    for (int i = 0; i < eventCount; i++) {
                        if (firstEvent != null) {
                            FilteredEvent previousEvent = firstEvent;
                            sumMatching += firstEvent.getEndOffset() - firstEvent.getBeginOffset();
                            count++;
                            firstEvent = firstEvent.getNextEvent();
                            if (firstEvent != null)
                                sumNotMatching += firstEvent.getBeginOffset() - previousEvent.getEndOffset();
                        }
                        if (lastEvent != null) {
                            FilteredEvent previousEvent = lastEvent;
                            sumMatching += lastEvent.getEndOffset() - lastEvent.getBeginOffset();
                            count++;
                            lastEvent = lastEvent.getPreviousEvent();
                            if (lastEvent != null)
                                sumNotMatching += previousEvent.getBeginOffset() - lastEvent.getEndOffset();
                        }
                    }
                    double averageMatching = (double)sumMatching / count;
                    double averageNotMatching = (double)sumNotMatching / count;
                    approximateMatchingEventRatio = averageMatching / (averageMatching + averageNotMatching);
                    approximateNumberOfEvents = (int)((endOffset - beginOffset) / averageMatching * approximateMatchingEventRatio);
                }
            }
        }
        return approximateNumberOfEvents;
    }

    public double getApproximatePercentageForEventNumber(long eventNumber) {
        if (tracedEventNumber != -1)
            // TODO: this is clearly not good and should return a much better approximation
            return super.getApproximatePercentageForEventNumber(eventNumber);
        else
            // TODO: what if filter is event range limited
            return super.getApproximatePercentageForEventNumber(eventNumber);
    }

    @Override
    public FilteredEvent getApproximateEventAt(double percentage) {
        if (isEmpty())
            return null;
        else {
            double firstEventPercentage = eventLog.getApproximatePercentageForEventNumber(getFirstEvent().getEventNumber());
            double lastEventPercentage = eventLog.getApproximatePercentageForEventNumber(getLastEvent().getEventNumber());
            percentage = firstEventPercentage + percentage * (lastEventPercentage - firstEventPercentage);
            IEvent event = eventLog.getApproximateEventAt(percentage);
            FilteredEvent filteredEvent = getMatchingEventInDirection(event, true);
            if (filteredEvent != null)
                return filteredEvent;
            filteredEvent = getMatchingEventInDirection(event, false);
            if (filteredEvent != null)
                return filteredEvent;
            Assert.isTrue(false);
            return null;
        }
    }

    /**
     * Caches the given filtered event in the event cache if not present yet.
     * The event must be known to match this filter.
     */
    public FilteredEvent cacheFilteredEvent(long eventNumber) {
        var it = eventNumberToFilteredEventMap.get(eventNumber);
        if (it != null)
            return it;
        else {
            FilteredEvent filteredEvent = new FilteredEvent(this, eventNumber);
            eventNumberToFilteredEventMap.put(eventNumber, filteredEvent);
            return filteredEvent;
        }
    }

    /**
     * Checks whether the given event matches this filter.
     */
    protected final boolean matchesEvent(IEvent event)
    {
        // event outside of considered range
        if ((firstConsideredEventNumber != -1 && event.getEventNumber() < firstConsideredEventNumber) || (lastConsideredEventNumber != -1 && event.getEventNumber() > lastConsideredEventNumber))
            return false;
        if (excludedEventNumbers.contains(event.getEventNumber()))
            return false;
        // event's module
        if (enableModuleFilter) {
            ModuleDescriptionEntry eventModuleDescriptionEntry = event.getModuleDescriptionEntry();
            ModuleDescriptionEntry moduleDescriptionEntry = eventModuleDescriptionEntry;
            // match parent chain of event's module (to handle compound modules too)
          OUTER:
            while (moduleDescriptionEntry != null) {
                if (matchesModuleDescriptionEntry(moduleDescriptionEntry)) {
                    if (moduleDescriptionEntry == eventModuleDescriptionEntry)
                        break;
                    else {
                        // check if the event has a cause or consequence referring
                        // outside the matching compound module
                        ArrayList<IMessageDependency > causes = event.getCauses();
                        for (var cause : causes) {
                            IEvent causeEvent = cause.getCauseEvent();
                            if (causeEvent != null && !isAncestorModuleDescriptionEntry(moduleDescriptionEntry, causeEvent.getModuleDescriptionEntry()))
                                break OUTER;
                        }

                        ArrayList<IMessageDependency > consequences = event.getConsequences();
                        for (var consequence : consequences) {
                            IEvent consequenceEvent = consequence.getConsequenceEvent();
                            if (consequenceEvent != null && !isAncestorModuleDescriptionEntry(moduleDescriptionEntry, consequenceEvent.getModuleDescriptionEntry()))
                                break OUTER;
                        }
                    }
                }
                moduleDescriptionEntry = getEventLogEntryCache().getModuleDescriptionEntry(moduleDescriptionEntry.parentModuleId);
            }
            if (moduleDescriptionEntry == null)
                return false;
        }
        // event's message
        if (enableMessageFilter) {
            BeginSendEntry beginSendEntry = event.getCauseBeginSendEntry();
            boolean matches = beginSendEntry != null ? matchesBeginSendEntry(beginSendEntry) : false;
            for (int i = 0; i < event.getNumEventLogEntries(); i++) {
                beginSendEntry = event.getEventLogEntry(i) instanceof BeginSendEntry ? (BeginSendEntry)event.getEventLogEntry(i) : null;
                if (beginSendEntry != null && matchesBeginSendEntry(beginSendEntry)) {
                    matches = true;
                    break;
                }
            }
            if (!matches)
                return false;
        }
        return true;
    }

    protected final boolean matchesDependency(IEvent event) {
        // if there's no traced event
        if (tracedEventNumber == -1)
            return true;
        // if this is the traced event
        if (event.getEventNumber() == tracedEventNumber)
            return true;
        // event is cause of traced event
        if (tracedEventNumber > event.getEventNumber() && traceCauses)
            return isCauseOfTracedEvent(event);
        // event is consequence of traced event
        if (tracedEventNumber < event.getEventNumber() && traceConsequences)
            return isConsequenceOfTracedEvent(event);
        return false;
    }

    protected final boolean matchesBeginSendEntry(BeginSendEntry beginSendEntry) {
        return matchesExpression(messageExpression, beginSendEntry) || matchesPatterns(messageNames, beginSendEntry.messageName) || matchesPatterns(messageClassNames, beginSendEntry.messageClassName) || matchesList(messageIds, beginSendEntry.messageId) || matchesList(messageTreeIds, beginSendEntry.messageTreeId) || matchesList(messageEncapsulationIds, beginSendEntry.messageEncapsulationId) || matchesList(messageEncapsulationTreeIds, beginSendEntry.messageEncapsulationTreeId);
    }

    protected final boolean matchesExpression(MatchExpression matchExpression, EventLogEntry eventLogEntry)
    {
        javaMatchableObject.setJavaObject(eventLogEntry);
        return matchExpression.matches(javaMatchableObject);
    }

    protected final boolean matchesPatterns(ArrayList<PatternMatcher> patterns, String str) {
        if (patterns.isEmpty())
            return false;
        for (var pattern : patterns) {
            if (pattern.matches(str))
                return true;
        }
        return false;
    }

    protected final <T> boolean matchesList(ArrayList<T> elements, T element) {
        if (elements.isEmpty())
            return false;
        else
            return elements.contains(element);
    }

    // LONG RUNNING OPERATION
    // this does a recursive depth search
    protected final boolean isCauseOfTracedEvent(IEvent causeEvent) {
        Assert.isTrue(causeEvent != null);
        eventLog.progress();
        var it = eventNumberToTraceableEventFlagMap.get(causeEvent.getEventNumber());
        if (it != null)
            return it;
        // System.out.println("Checking if %ld is cause of %ld\n", causeEvent->getEventNumber(), tracedEventNumber);
        long causeEventNumber = causeEvent.getEventNumber();
        while (!unseenTracedEventCauseEventNumbers.isEmpty() && unseenTracedEventCauseEventNumbers.getFirst() >= causeEventNumber) {
            long unseenTracedEventCauseEventNumber = unseenTracedEventCauseEventNumbers.getFirst();
            unseenTracedEventCauseEventNumbers.removeFirst();
            IEvent unseenTracedEventCauseEvent = eventLog.getEventForEventNumber(unseenTracedEventCauseEventNumber);
            if (unseenTracedEventCauseEvent != null) {
                ArrayList<IMessageDependency > causes = unseenTracedEventCauseEvent.getCauses();
                for (var messageDependency : causes) {
                    IEvent newUnseenTracedEventCauseEvent = messageDependency.getCauseEvent();
                    if (newUnseenTracedEventCauseEvent != null && (traceSelfMessages || !newUnseenTracedEventCauseEvent.isSelfMessageProcessingEvent()) && (traceMessageReuses || !(messageDependency instanceof MessageReuseDependency))) {
                        long newUnseenTracedEventCauseEventNumber = newUnseenTracedEventCauseEvent.getEventNumber();
                        eventNumberToTraceableEventFlagMap.put(newUnseenTracedEventCauseEventNumber, true);
                        unseenTracedEventCauseEventNumbers.addLast(newUnseenTracedEventCauseEventNumber);
                        if (newUnseenTracedEventCauseEventNumber == causeEventNumber)
                            return true;
                    }
                }
                // TODO: this is far from being optimal, inserting the items in the right place would be more desirable
                Collections.sort(unseenTracedEventCauseEventNumbers);
            }
        }
        return eventNumberToTraceableEventFlagMap.put(causeEventNumber, false);
    }

    // LONG RUNNING OPERATION
    // this does a recursive depth search
    protected final boolean isConsequenceOfTracedEvent(IEvent consequenceEvent) {
        Assert.isTrue(consequenceEvent != null);
        eventLog.progress();
        var it = eventNumberToTraceableEventFlagMap.get(consequenceEvent.getEventNumber());
        if (it != null)
            return it;
        // System.out.println("Checking if %ld is consequence of %ld\n", consequence->getEventNumber(), tracedEventNumber);
        // like isCauseOfTracedEvent(), but searching from the opposite direction
        long consequenceEventNumber = consequenceEvent.getEventNumber();
        while (!unseenTracedEventConsequenceEventNumbers.isEmpty() && unseenTracedEventConsequenceEventNumbers.getFirst() <= consequenceEventNumber) {
            long unseenTracedEventConsequenceEventNumber = unseenTracedEventConsequenceEventNumbers.getFirst();
            unseenTracedEventConsequenceEventNumbers.removeFirst();
            IEvent unseenTracedEventConsequenceEvent = eventLog.getEventForEventNumber(unseenTracedEventConsequenceEventNumber);
            if (unseenTracedEventConsequenceEvent != null) {
                ArrayList<IMessageDependency > consequences = unseenTracedEventConsequenceEvent.getConsequences();
                for (var messageDependency : consequences) {
                    IEvent newUnseenTracedEventConsequenceEvent = messageDependency.getConsequenceEvent();
                    if (newUnseenTracedEventConsequenceEvent != null && (traceSelfMessages || !newUnseenTracedEventConsequenceEvent.isSelfMessageProcessingEvent()) && (traceMessageReuses || !(messageDependency instanceof MessageReuseDependency))) {
                        long newUnseenTracedEventConsequenceEventNumber = newUnseenTracedEventConsequenceEvent.getEventNumber();
                        eventNumberToTraceableEventFlagMap.put(newUnseenTracedEventConsequenceEventNumber, true);
                        unseenTracedEventConsequenceEventNumbers.addLast(newUnseenTracedEventConsequenceEventNumber);
                        if (newUnseenTracedEventConsequenceEventNumber == consequenceEventNumber)
                            return true;
                    }
                }
                // TODO: this is far from being optimal, inserting the items in the right place would be more desirable
                Collections.sort(unseenTracedEventConsequenceEventNumbers);
            }
        }
        return eventNumberToTraceableEventFlagMap.put(consequenceEventNumber, false);
    }

    protected final void setPatternMatchers(java.util.ArrayList<PatternMatcher> patternMatchers, java.util.ArrayList<String> patterns) {
        setPatternMatchers(patternMatchers, patterns, false);
    }

    protected final void setPatternMatchers(ArrayList<PatternMatcher> patternMatchers, ArrayList<String> patterns, boolean dottedPath) {
        for (var pattern : patterns) {
            PatternMatcher matcher = new PatternMatcher();
            matcher.setPattern(pattern, dottedPath, true, false);
            patternMatchers.add(matcher);
        }
    }

    protected final void clearInternalState() {
        firstMatchingEvent = null;
        lastMatchingEvent = null;
        approximateNumberOfEvents = -1;
        approximateMatchingEventRatio = -1;
        eventNumberToFilteredEventMap.clear();
        eventNumberToFilterMatchesFlagMap.clear();
        eventNumberToTraceableEventFlagMap.clear();
        unseenTracedEventCauseEventNumbers.clear();
        unseenTracedEventConsequenceEventNumbers.clear();
    }

    protected final void deleteAllocatedObjects() {
        eventNumberToFilteredEventMap.clear();
        eventNumberToFilterMatchesFlagMap.clear();
        eventNumberToTraceableEventFlagMap.clear();
    }

    protected final boolean isAncestorModuleDescriptionEntry(ModuleDescriptionEntry ancestor, ModuleDescriptionEntry descendant) {
        while (descendant != null) {
            if (descendant == ancestor)
                return true;
            else
                descendant = getEventLogEntryCache().getModuleDescriptionEntry(descendant.parentModuleId);
        }
        return false;
    }
}
