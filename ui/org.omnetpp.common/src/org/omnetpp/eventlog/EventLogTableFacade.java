package org.omnetpp.eventlog;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.JavaMatchableObject;
import org.omnetpp.common.engine.MatchExpression;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;

public class EventLogTableFacade extends EventLogFacade
{
    protected long approximateNumberOfEntries = -1; // cached value, -1 means not yet calculated
    protected long lastMatchedEventNumber = -1; // -1 means unused
    protected int lastNumMatchingEventLogEntries; // -1 means unused
    protected EventLogTableFilterMode filterMode; // can be set with the public API
    protected String customFilter; // can be set with the public API
    protected MatchExpression filterMatchExpression = new MatchExpression(); // cached expression
    protected JavaMatchableObject javaMatchableObject = new JavaMatchableObject();

    protected static final class RefObject<T> {
        public T argValue;

        public RefObject(T refArg) {
            argValue = refArg;
        }
    }

    public EventLogTableFacade(IEventLog eventLog) {
        super(eventLog);
        clearInternalState();
        setCustomFilter("*");
        setFilterMode(EventLogTableFilterMode.ALL_ENTRIES);
    }

    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                case FileReader.FileChange.APPENDED:
                    super.synchronize(change);
                    clearInternalState();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    public final EventLogTableFilterMode getFilterMode() {
        return filterMode;
    }

    public final void setFilterMode(EventLogTableFilterMode filterMode) {
        this.filterMode = filterMode;
        clearInternalState();
    }

    public final boolean matchesFilter(EventLogEntry eventLogEntry) {
        switch (filterMode) {
            case ALL_ENTRIES:
                return true;
            case EVENT_AND_SEND_AND_MESSAGE_ENTRIES:
                return eventLogEntry instanceof EventEntry || eventLogEntry instanceof BeginSendEntry || eventLogEntry instanceof EventLogMessageEntry;
            case EVENT_AND_MESSAGE_ENTRIES:
                return eventLogEntry instanceof EventEntry || eventLogEntry instanceof EventLogMessageEntry;
            case EVENT_ENTRIES:
                return eventLogEntry instanceof EventEntry;
            case CUSTOM_ENTRIES: {
                javaMatchableObject.setJavaObject(eventLogEntry);
                return filterMatchExpression.matches(javaMatchableObject);
            }
            default:
                throw new RuntimeException("Unknown eventlog table filter");
        }
    }

    public final int getNumMatchingEventLogEntries(IEvent event) {
        if (lastMatchedEventNumber == event.getEventNumber())
            return lastNumMatchingEventLogEntries;
        else {
            lastMatchedEventNumber = event.getEventNumber();
            switch (filterMode) {
                case ALL_ENTRIES:
                    lastNumMatchingEventLogEntries = event.getNumEventLogEntries();
                    break;
                case EVENT_AND_SEND_AND_MESSAGE_ENTRIES:
                    lastNumMatchingEventLogEntries = event.getNumBeginSendEntries() + event.getNumEventLogMessages() + 1;
                    break;
                case EVENT_AND_MESSAGE_ENTRIES:
                    lastNumMatchingEventLogEntries = event.getNumEventLogMessages() + 1;
                    break;
                case EVENT_ENTRIES:
                    lastNumMatchingEventLogEntries = 1;
                    break;
                case CUSTOM_ENTRIES: {
                    int count = 0;
                    int num = event.getNumEventLogEntries();
                    for (int i = 0; i < num; i++) {
                        if (matchesFilter(event.getEventLogEntry(i)))
                            count++;
                    }
                    lastNumMatchingEventLogEntries = count;
                    break;
                }
                default:
                    throw new RuntimeException("Unknown eventlog table filter");
            }
            return lastNumMatchingEventLogEntries;
        }
    }

    public final void setCustomFilter(String pattern) {
        customFilter = pattern;
        filterMatchExpression.setPattern("E or (" + customFilter + ")", false, true, false);
        clearInternalState();
    }

    public final String getCustomFilter() {
        return customFilter;
    }

    public EventLogEntry getEventLogEntry(long eventNumber, int eventLogEntryIndex) {
        IEvent event = eventLog.getEventForEventNumber(eventNumber);
        Assert.isTrue(event != null);
        return event.getEventLogEntry(eventLogEntryIndex);
    }

    public final EventLogEntry getFirstEntry() {
        IEvent event = eventLog.getFirstEvent();
        if (event == null)
            return null;
        else
            return getEntryInEvent(event, 0);
    }

    public final EventLogEntry getLastEntry() {
        IEvent event = eventLog.getLastEvent();
        if (event == null)
            return null;
        else
            return getEntryInEvent(event, getNumMatchingEventLogEntries(event) - 1);
    }

    public final int getEntryIndexInEvent(EventLogEntry eventLogEntry) {
        IEvent event = eventLogEntry.getEvent();
        int index = 0;
        int num = event.getNumEventLogEntries();
        for (int i = 0; i < num; i++) {
            EventLogEntry currentEventLogEntry = event.getEventLogEntry(i);
            if (matchesFilter(currentEventLogEntry)) {
                if (currentEventLogEntry == eventLogEntry)
                    return index;
                else
                    index++;
            }
        }
        throw new RuntimeException("No eventlog entry found in event: " + event.getEventNumber());
    }

    public final EventLogEntry getEntryInEvent(IEvent event, int index) {
        Assert.isTrue(index >= 0 && index < getNumMatchingEventLogEntries(event));
        if (filterMode == EventLogTableFilterMode.ALL_ENTRIES)
            return event.getEventLogEntry(index);
        else {
            int num = event.getNumEventLogEntries();
            for (int i = 0; i < num; i++) {
                EventLogEntry eventLogEntry = event.getEventLogEntry(i);
                if (matchesFilter(eventLogEntry)) {
                    if (index == 0)
                        return eventLogEntry;
                    else
                        index--;
                }
            }
        }
        throw new RuntimeException("No eventlog entry with index: " + index + " in event: " + event.getEventNumber());
    }

    public long getDistanceToEntry(EventLogEntry sourceEventLogEntry, EventLogEntry targetEventLogEntry, long limit) {
        var reachedDistance = new RefObject<Long>(null);
        getEntryAndDistance(sourceEventLogEntry, targetEventLogEntry, limit, reachedDistance);
        return reachedDistance.argValue;
    }

    public final EventLogEntry getClosestEntryInEvent(EventLogEntry eventLogEntry) {
        Assert.isTrue(eventLogEntry != null);
        IEvent event = eventLogEntry.getEvent();
        for (int i = eventLogEntry.getEntryIndex(); i >= 0; i--) {
            eventLogEntry = event.getEventLogEntry(i);
            if (matchesFilter(eventLogEntry))
                return eventLogEntry;
        }
        throw new RuntimeException("EventEntry should always match the filter");
    }

    public long getDistanceToFirstEntry(EventLogEntry eventLogEntry, long limit) {
        var reachedDistance = new RefObject<Long>(null);
        getEntryAndDistance(eventLogEntry, getFirstEntry(), -limit, reachedDistance);
        return -reachedDistance.argValue;
    }


    public long getDistanceToLastEntry(EventLogEntry eventLogEntry, long limit) {
        var reachedDistance = new RefObject<Long>(null);
        getEntryAndDistance(eventLogEntry, getLastEntry(), limit, reachedDistance);
        return reachedDistance.argValue;
    }

    public EventLogEntry getNeighbourEntry(EventLogEntry eventLogEntry, long distance) {
        var reachedDistance = new RefObject<Long>(null);
        return getEntryAndDistance(eventLogEntry, null, distance, reachedDistance);
    }

    public final double getApproximatePercentageForEntry(EventLogEntry eventLogEntry) {
        IEvent beforeEvent = eventLogEntry.getEvent();
        IEvent afterEvent = beforeEvent.getNextEvent();
        double beforePercentage = eventLog.getApproximatePercentageForEventNumber(beforeEvent.getEventNumber());
        double afterPercentage = afterEvent != null ? eventLog.getApproximatePercentageForEventNumber(afterEvent.getEventNumber()) : 1.0;
        int index = getEntryIndexInEvent(eventLogEntry);
        return beforePercentage + (afterPercentage - beforePercentage) * index / getNumMatchingEventLogEntries(beforeEvent);
    }

    public final EventLogEntry getApproximateEventLogEntryAt(double percentage) {
        Assert.isTrue(0.0 <= percentage && percentage <= 1.0);
        if (percentage == 1) {
            IEvent event = eventLog.getLastEvent();
            if (event == null)
                return null;
            else
                return getEntryInEvent(event, getNumMatchingEventLogEntries(event) - 1);
        }
        else {
            IEvent beforeEvent = eventLog.getApproximateEventAt(percentage);
            if (beforeEvent == null)
                return null;
            else {
                double beforePercentage = eventLog.getApproximatePercentageForEventNumber(beforeEvent.getEventNumber());
                double afterPercentage = 1;
                IEvent afterEvent = beforeEvent.getNextEvent();
                if (afterEvent != null)
                    afterPercentage = eventLog.getApproximatePercentageForEventNumber(afterEvent.getEventNumber());
                if (percentage < beforePercentage || percentage > afterPercentage)
                    return beforeEvent.getEventEntry();
                else {
                    double entryPercentage = (percentage - beforePercentage) / (afterPercentage - beforePercentage);
                    Assert.isTrue(0.0 <= entryPercentage && entryPercentage <= 1.0);
                    int index = (int)Math.floor((getNumMatchingEventLogEntries(beforeEvent) - 1) * entryPercentage);
                    return getEntryInEvent(beforeEvent, index);
                }
            }
        }
    }

    public final long getApproximateNumberOfEntries() {
        if (approximateNumberOfEntries == -1) {
            IEvent firstEvent = eventLog.getFirstEvent();
            IEvent lastEvent = eventLog.getLastEvent();
            if (firstEvent == null)
                approximateNumberOfEntries = 0;
            else {
                int sum = 0;
                int count = 0;
                int eventCount = 100;
                for (int i = 0; i < eventCount; i++) {
                    if (firstEvent != null) {
                        sum += getNumMatchingEventLogEntries(firstEvent);
                        count++;
                        firstEvent = firstEvent.getNextEvent();
                    }
                    if (lastEvent != null) {
                        sum += getNumMatchingEventLogEntries(lastEvent);
                        count++;
                        lastEvent = lastEvent.getPreviousEvent();
                    }
                }
                double average = (double)sum / count;
                approximateNumberOfEntries = (int)(eventLog.getApproximateNumberOfEvents() * average);
            }
        }
        return approximateNumberOfEntries;
    }

    protected final EventLogEntry getPreviousEntry(EventLogEntry eventLogEntry, RefObject<Integer> index) {
        IEvent event = eventLog.getEventForEventNumber(eventLogEntry.getEvent().getEventNumber());
        index.argValue--;
        if (index.argValue == -1) {
            event = event.getPreviousEvent();
            if (event != null)
                index.argValue = getNumMatchingEventLogEntries(event) - 1;
        }
        if (event == null)
            return null;
        else
            return getEntryInEvent(event, index.argValue);
    }

    protected final EventLogEntry getNextEntry(EventLogEntry eventLogEntry, RefObject<Integer> index) {
        IEvent event = eventLog.getEventForEventNumber(eventLogEntry.getEvent().getEventNumber());
        index.argValue++;
        if (index.argValue == getNumMatchingEventLogEntries(event)) {
            event = event.getNextEvent();
            index.argValue = 0;
        }
        if (event == null)
            return null;
        else
            return getEntryInEvent(event, index.argValue);
    }

    public EventLogEntry getEntryAndDistance(EventLogEntry sourceEventLogEntry, EventLogEntry targetEventLogEntry, long distance, RefObject<Long> reachedDistance) {
        Assert.isTrue(sourceEventLogEntry != null);
        EventLogEntry eventLogEntry = sourceEventLogEntry;
        reachedDistance.argValue = (long)0;
        var index = new RefObject<Integer>(getEntryIndexInEvent(eventLogEntry));
        Assert.isTrue(index.argValue >= 0);
        while (distance != 0 && eventLogEntry != null && eventLogEntry != targetEventLogEntry) {
            if (distance > 0) {
                eventLogEntry = getNextEntry(eventLogEntry, index);
                distance--;
                reachedDistance.argValue++;
            }
            else {
                eventLogEntry = getPreviousEntry(eventLogEntry, index);
                distance++;
                reachedDistance.argValue--;
            }
        }
        if (eventLogEntry == null)
            reachedDistance.argValue += distance;
        return eventLogEntry;
    }

    protected final void clearInternalState() {
        approximateNumberOfEntries = -1;
        lastMatchedEventNumber = -1;
        lastNumMatchingEventLogEntries = -1;
    }
}
