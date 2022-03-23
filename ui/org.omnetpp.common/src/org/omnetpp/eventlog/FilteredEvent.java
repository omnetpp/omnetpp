package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;
import java.util.LinkedList;

import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;

/**
 * Events stored in the FilteredEventLog. This class uses the Event class by delegation so that multiple
 * filtered events can share the same event object.
 *
 * Filtered events are in a lazy double-linked list based on event numbers.
 */
public class FilteredEvent extends EventBase implements IEvent
{
    protected FilteredEventLog filteredEventLog;
    protected long eventNumber = -1; // the corresponding event number
    protected long causeEventNumber = -1; // the event number from which the message was sent that is being processed in this event
    protected IMessageDependency cause; // the message send which is processed in this event
    protected ArrayList<IMessageDependency> causes; // the arrival message sends of messages which we send in this even and are in the filtered set
    protected ArrayList<IMessageDependency> consequences; // the message sends and arrivals from this event to another in the filtered set

    protected static class BreadthSearchItem {
        public IEvent event;
        public IMessageDependency firstSeenMessageDependency;
        public FilteredMessageDependency.Kind effectiveKind;
        public int level;

        public BreadthSearchItem(IEvent event, IMessageDependency firstSeenMessageDependency, FilteredMessageDependency.Kind effectiveKind, int level)
        {
            this.event = event;
            this.firstSeenMessageDependency = firstSeenMessageDependency;
            this.effectiveKind = effectiveKind;
            this.level = level;
        }
    }

    public FilteredEvent(FilteredEventLog filteredEventLog, long eventNumber) {
        this.filteredEventLog = filteredEventLog;
        this.eventNumber = eventNumber;
    }

    public final IEvent getEvent() {
        return filteredEventLog.getEventLog().getEventForEventNumber(eventNumber);
    }

    // IEvent interface
    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    deleteAllocatedObjects();
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    deleteConsequences();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    @Override
    public String getAsString() {
        return getEvent().getAsString();
    }

    @Override
    public String getAsString(String name) {
        return getEvent().getAsString(name);
    }

    @Override
    public IEventLog getEventLog() {
        return filteredEventLog;
    }

    @Override
    public long getBeginOffset() {
        return getEvent().getBeginOffset();
    }

    @Override
    public long getEndOffset() {
        return getEvent().getEndOffset();
    }

    @Override
    public EventEntry getEventEntry() {
        return getEvent().getEventEntry();
    }

    @Override
    public ModuleDescriptionEntry getModuleDescriptionEntry() {
        return getEvent().getModuleDescriptionEntry();
    }

    @Override
    public int getNumEventLogEntries() {
        return getEvent().getNumEventLogEntries();
    }

    @Override
    public EventLogEntry getEventLogEntry(int index) {
        return getEvent().getEventLogEntry(index);
    }

    @Override
    public int getNumEventLogMessages() {
        return getEvent().getNumEventLogMessages();
    }

    @Override
    public int getNumBeginSendEntries() {
        return getEvent().getNumBeginSendEntries();
    }

    @Override
    public int getNumCustomEntries() {
        return getEvent().getNumCustomEntries();
    }

    @Override
    public EventLogMessageEntry getEventLogMessage(int index) {
        return getEvent().getEventLogMessage(index);
    }

    @Override
    public long getEventNumber() {
        return eventNumber;
    }

    @Override
    public BigDecimal getSimulationTime() {
        return getEvent().getSimulationTime();
    }

    @Override
    public int getModuleId() {
        return getEvent().getModuleId();
    }

    @Override
    public long getMessageId() {
        return getEvent().getMessageId();
    }

    @Override
    public long getCauseEventNumber() {
        return getEvent().getCauseEventNumber();
    }

    @Override
    public boolean isSelfMessage(BeginSendEntry beginSendEntry) {
        return getEvent().isSelfMessage(beginSendEntry);
    }

    @Override
    public boolean isSelfMessageProcessingEvent() {
        return getEvent().isSelfMessageProcessingEvent();
    }

    @Override
    public EndSendEntry getEndSendEntry(BeginSendEntry beginSendEntry) {
        return getEvent().getEndSendEntry(beginSendEntry);
    }

    @Override
    public FilteredEvent getPreviousEvent() {
        if (previousEvent == null && filteredEventLog.getFirstEvent() != this) {
            previousEvent = filteredEventLog.getMatchingEventInDirection(getEvent().getPreviousEvent(), false);
            if (previousEvent != null)
                linkEvents(previousEvent, this);
        }
        return (FilteredEvent)previousEvent;
    }

    @Override
    public FilteredEvent getNextEvent() {
        if (nextEvent == null && filteredEventLog.getLastEvent() != this) {
            nextEvent = filteredEventLog.getMatchingEventInDirection(getEvent().getNextEvent(), true);
            if (nextEvent != null)
                Event.linkEvents(this, nextEvent);
        }
        return (FilteredEvent)nextEvent;
    }

    @Override
    public FilteredEvent getCauseEvent() {
        if (causeEventNumber == -1) {
            IEvent causeEvent = getEvent().getCauseEvent();
            // LONG RUNNING OPERATION
            // walk backwards on the cause chain until we find an event matched by the filter
            // this might read all events backward if none of the causes matches the filter
            while (causeEvent != null) {
                filteredEventLog.progress();
                if (causeEvent.getEventNumber() < filteredEventLog.getFirstConsideredEventNumber())
                    return null;
                if (filteredEventLog.matchesFilter(causeEvent))
                    return (FilteredEvent)filteredEventLog.getEventForEventNumber(causeEvent.getEventNumber());
                causeEvent = causeEvent.getCauseEvent();
            }
        }
        return (FilteredEvent)filteredEventLog.getEventForEventNumber(causeEventNumber);
    }

    @Override
    public IMessageDependency getCause() {
        if (cause == null) {
            IEvent causeEvent = getEvent();
            IMessageDependency causeMessageDependency = causeEvent.getCause();
            if (causeMessageDependency != null) {
                IMessageDependency messageDependency;
                // LONG RUNNING OPERATION
                // this might read all events backward if none of the causes matches the filter
                while (causeEvent != null && causeEvent.getCause() != null) {
                    messageDependency = causeEvent.getCause();
                    filteredEventLog.progress();
                    if (causeEvent.getEventNumber() < filteredEventLog.getFirstConsideredEventNumber())
                        return null;
                    if (filteredEventLog.matchesFilter(messageDependency.getCauseEvent())) {
                        if (messageDependency == causeMessageDependency)
                            cause = messageDependency.duplicate(filteredEventLog);
                        else
                            cause = new FilteredMessageDependency(filteredEventLog, FilteredMessageDependency.Kind.SENDS, messageDependency.duplicate(filteredEventLog.getEventLog()), causeMessageDependency.duplicate(filteredEventLog.getEventLog()));
                        break;
                    }
                    causeEvent = causeEvent.getCauseEvent();
                }
            }
        }
        return cause;
    }

    @Override
    public BeginSendEntry getCauseBeginSendEntry() {
        IMessageDependency cause = getCause();
        if (cause != null) {
            MessageDescriptionEntry messageDescriptionEntry = cause.getBeginMessageDescriptionEntry();
            return messageDescriptionEntry instanceof BeginSendEntry ? (BeginSendEntry)messageDescriptionEntry : null;
        }
        else
            return null;
    }

    public static FilteredMessageDependency.Kind getMessageDependencyKind(IMessageDependency messageDependency) {
        if (messageDependency instanceof MessageSendDependency)
            return FilteredMessageDependency.Kind.SENDS;
        else if (messageDependency instanceof MessageReuseDependency)
            return FilteredMessageDependency.Kind.REUSES;
        else
            return FilteredMessageDependency.Kind.MIXED;
    }

    @Override
    public ArrayList<IMessageDependency> getCauses() {
        if (causes == null) {
            // returns a list of dependencies, where the consequence is this event,
            // and the other end is no further away than getMaximumCauseDepth() and
            // no events in between match the filter
            long begin = System.currentTimeMillis();
            causes = new ArrayList<IMessageDependency>();
            LinkedList<BreadthSearchItem> todoList = new LinkedList<BreadthSearchItem>();
            todoList.addLast(new BreadthSearchItem(getEvent(), null, FilteredMessageDependency.Kind.UNDEFINED, 0));
            // LONG RUNNING OPERATION
            // this is recursive and might take some time
            while (!todoList.isEmpty()) {
                filteredEventLog.progress();
                if (System.currentTimeMillis() - begin >= filteredEventLog.getMaximumCauseCollectionTime())
                    break;

                BreadthSearchItem searchItem = todoList.getFirst();
                todoList.removeFirst();
                // unpack
                IEvent currentEvent = searchItem.event;
                IMessageDependency endMessageDependency = searchItem.firstSeenMessageDependency;
                FilteredMessageDependency.Kind currentKind = searchItem.effectiveKind;
                int level = searchItem.level;

                ArrayList<IMessageDependency> eventCauses = currentEvent.getCauses();
                for (var messageDependency : eventCauses) {
                    IEvent causeEvent = messageDependency.getCauseEvent();
                    if (causeEvent != null && (filteredEventLog.getCollectMessageReuses() || !(messageDependency instanceof MessageReuseDependency))) {
                        FilteredMessageDependency.Kind effectiveKind = FilteredMessageDependency.Kind.forValue(currentKind.getValue() | getMessageDependencyKind(messageDependency).getValue());
                        if (filteredEventLog.matchesFilter(causeEvent) && (level == 0 || MessageDependencyBase.corresponds(messageDependency, endMessageDependency))) {
                            if (level == 0)
                                causes.add(messageDependency.duplicate(filteredEventLog));
                            else
                                pushNewFilteredMessageDependency(causes, effectiveKind, messageDependency, endMessageDependency);
                            if (countFilteredMessageDependencies(causes) == filteredEventLog.getMaximumNumberOfCauses())
                                return causes;
                        }
                        else if (level < filteredEventLog.getMaximumCauseDepth())
                            todoList.addLast(new BreadthSearchItem(causeEvent, level == 0 ? messageDependency : endMessageDependency, effectiveKind, level + 1));
                    }
                }
            }
        }
        return causes;
    }

    @Override
    public ArrayList<IMessageDependency> getConsequences() {
        if (consequences == null) {
            // similar to getCause
            long begin = System.currentTimeMillis();
            consequences = new ArrayList<IMessageDependency>();
            LinkedList<BreadthSearchItem> todoList = new LinkedList<BreadthSearchItem>();
            todoList.addLast(new BreadthSearchItem(getEvent(), null, FilteredMessageDependency.Kind.UNDEFINED, 0));
            // LONG RUNNING OPERATION
            // this is recursive and might take some time
            while (!todoList.isEmpty()) {
                filteredEventLog.progress();
                if (System.currentTimeMillis() - begin >= filteredEventLog.getMaximumConsequenceCollectionTime())
                    break;

                BreadthSearchItem searchItem = todoList.getFirst();
                todoList.removeFirst();
                // unpack
                IEvent currentEvent = searchItem.event;
                IMessageDependency beginMessageDependency = searchItem.firstSeenMessageDependency;
                FilteredMessageDependency.Kind currentKind = searchItem.effectiveKind;
                int level = searchItem.level;

                ArrayList<IMessageDependency> eventConsequences = currentEvent.getConsequences();
                for (var messageDependency : eventConsequences) {
                    IEvent consequenceEvent = messageDependency.getConsequenceEvent();
                    if (consequenceEvent != null && (filteredEventLog.getCollectMessageReuses() || !(messageDependency instanceof MessageReuseDependency))) {
                        FilteredMessageDependency.Kind effectiveKind = FilteredMessageDependency.Kind.forValue(currentKind.getValue() | getMessageDependencyKind(messageDependency).getValue());
                        if (filteredEventLog.matchesFilter(consequenceEvent) && (level == 0 || MessageDependencyBase.corresponds(beginMessageDependency, messageDependency))) {
                            if (level == 0)
                                consequences.add(messageDependency.duplicate(filteredEventLog));
                            else
                                pushNewFilteredMessageDependency(consequences, effectiveKind, beginMessageDependency, messageDependency);
                            if (countFilteredMessageDependencies(consequences) == filteredEventLog.getMaximumNumberOfConsequences())
                                return consequences;
                        }
                        else if (level < filteredEventLog.getMaximumConsequenceDepth())
                            todoList.addLast(new BreadthSearchItem(consequenceEvent, level == 0 ? messageDependency : beginMessageDependency, effectiveKind, level + 1));
                    }
                }
            }
        }
        return consequences;
    }

    @Override
    public void print() {
        print(System.out, true);
    }

    @Override
    public void print(OutputStream stream) {
        print(stream, true);
    }

    @Override
    public void print(OutputStream stream, boolean outputEventLogMessages) {
        getEvent().print(stream, outputEventLogMessages);
    }

    protected final void clearInternalState() {
        causeEventNumber = -1;
        cause = null;
        causes = null;
        consequences = null;
    }

    protected final void deleteConsequences() {
        consequences = null;
    }

    protected final void deleteAllocatedObjects() {
        cause = null;
        causes = null;
        deleteConsequences();
    }

    public int countFilteredMessageDependencies(ArrayList<IMessageDependency> messageDependencies) {
        int count = 0;
        for (var messageDependencie : messageDependencies)
            if (messageDependencie instanceof FilteredMessageDependency)
                count++;
        return count;
    }

    public void pushNewFilteredMessageDependency(ArrayList<IMessageDependency> messageDependencies, FilteredMessageDependency.Kind kind, IMessageDependency beginMessageDependency, IMessageDependency endMessageDependency) {
        FilteredMessageDependency newMessageDependency = new FilteredMessageDependency(filteredEventLog, kind, beginMessageDependency.duplicate(filteredEventLog.getEventLog()), endMessageDependency.duplicate(filteredEventLog.getEventLog()));
        for (var messageDependencie : messageDependencies)
            if (messageDependencie.equals(newMessageDependency))
                return;
        messageDependencies.add(newMessageDependency);
    }
}
