package org.omnetpp.eventlog;

import java.io.OutputStream;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.TreeSet;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.EventLogIndex.ReadToEventLineResult;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.CloneMessageEntry;
import org.omnetpp.eventlog.entry.ComponentMethodBeginEntry;
import org.omnetpp.eventlog.entry.ComponentMethodEndEntry;
import org.omnetpp.eventlog.entry.CustomEntry;
import org.omnetpp.eventlog.entry.DeleteMessageEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.EventEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;
import org.omnetpp.eventlog.entry.SendDirectEntry;
import org.omnetpp.eventlog.entry.SendHopEntry;

/**
 * Manages all eventlog entries for a single event. (all lines belonging to an "E" line)
 */
public class Event extends EventBase implements IEvent
{
    protected EventLog eventLog; // the corresponding eventlog
    protected long beginOffset = -1; // file offset where the event starts
    protected long endOffset = -1; // file offset where the event ends (including the following empty line, equals to the begin of the next thunk)
    protected EventEntry eventEntry; // the eventlog entry that corresponds to the actual event ("E" line)
    protected ModuleDescriptionEntry moduleDescriptionEntry;
    protected int numEventLogMessages;
    protected int numBeginSendEntries;
    protected int numCustomEntries;
    protected ArrayList<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>(); // all entries parsed from the file (lines below "E" line)

    /**
     * A is a cause of B if and only if B is a consequence of A.
     */
    protected MessageSendDependency cause; // the message send which is processed in this event
    protected ArrayList<IMessageDependency> causes; // the arrival message sends of messages which we send in this event
    protected ArrayList<IMessageDependency> consequences; // message sends in this event

    public Event(EventLog eventLog) {
        this.eventLog = eventLog;
    }

    /**
     * Parse an event starting at the given offset. Calling this function
     * clears the internal state of this event before parsing.
     */
    public final long parse(FileReader reader, long offset) {
        eventLog.progress();
        clearInternalState();
        numEventLogMessages = 0;
        numBeginSendEntries = 0;
        numCustomEntries = 0;
        Assert.isTrue(offset >= 0);
        beginOffset = offset;
//        System.out.println("Parsing event at offset: " + offset + "\n");
        return parseLines(reader, offset);
    }

    /**
     * Parse eventlog entries starting at the given offset. This function
     *
     * This may be called multiple times if the previous parse stopped
     * because it reached the end of the file before the event was completely
     * parsed.
     */
    public final long parseLines(FileReader reader, long offset) {
//        System.out.println("Parsing lines at offset: " + offset + "\n");
        reader.seekTo(offset);
        // prepare index based on the already loaded eventlog entries
        int index = eventLogEntries.size();
        // prepare context module ids based on the already loaded eventlog entries
        LinkedList<Integer> contextModuleIds = new LinkedList<Integer>();
        if (eventEntry != null)
            contextModuleIds.addFirst(eventEntry.moduleId);
        for (EventLogEntry it : eventLogEntries) {
            EventLogEntry eventLogEntry = it;
            // handle module method end
            ComponentMethodEndEntry componentMethodEndEntry = eventLogEntry instanceof ComponentMethodEndEntry ? (ComponentMethodEndEntry)eventLogEntry : null;
            if (componentMethodEndEntry != null)
                contextModuleIds.removeFirst();
            // handle module method begin
            ComponentMethodBeginEntry componentMethodBeginEntry = eventLogEntry instanceof ComponentMethodBeginEntry ? (ComponentMethodBeginEntry)eventLogEntry : null;
            if (componentMethodBeginEntry != null)
                contextModuleIds.addFirst(componentMethodBeginEntry.targetComponentId);
        }
        // parse lines one by one
        while (true) {
            String line = reader.getNextLine();
            if (line == null) {
                endOffset = reader.getFileSize();
                break;
            }
            EventLogEntry eventLogEntry = EventLogEntry.parseEntry(eventLog, this, index, reader.getCurrentLineStartOffset(), line, (int)reader.getCurrentLineLength());
            // stop at first empty line
            if (eventLogEntry == null) {
                endOffset = reader.getCurrentLineEndOffset();
                break;
            }
            else
                index++;
            // first line must be an event entry
            EventEntry readEventEntry = eventLogEntry instanceof EventEntry ? (EventEntry)eventLogEntry : null;
            Assert.isTrue((eventEntry == null && readEventEntry != null) || (eventEntry != null && readEventEntry == null));
            if (eventEntry == null) {
                eventEntry = readEventEntry;
                contextModuleIds.addFirst(eventEntry.moduleId);
            }
            // handle module method end
            ComponentMethodEndEntry componentMethodEndEntry = eventLogEntry instanceof ComponentMethodEndEntry ? (ComponentMethodEndEntry)eventLogEntry : null;
            if (componentMethodEndEntry != null)
                contextModuleIds.removeFirst();
            // store log entry
            eventLogEntry.level = contextModuleIds.size() - 1;
            eventLogEntry.contextModuleId = contextModuleIds.getFirst();
            eventLogEntries.add(eventLogEntry);
            // handle module method begin
            ComponentMethodBeginEntry componentMethodBeginEntry = eventLogEntry instanceof ComponentMethodBeginEntry ? (ComponentMethodBeginEntry)eventLogEntry : null;
            if (componentMethodBeginEntry != null)
                contextModuleIds.addFirst(componentMethodBeginEntry.targetComponentId);
            // count message entry
            if (eventLogEntry instanceof EventLogMessageEntry)
                numEventLogMessages++;
            // count begin send entry
            else if (eventLogEntry instanceof BeginSendEntry)
                numBeginSendEntries++;
            // count begin send entry
            else if (eventLogEntry instanceof CustomEntry)
                numCustomEntries++;
        }
        Assert.isTrue(eventEntry != null);
        return endOffset;
    }

    // IEvent interface
    @Override
    public void synchronize(int change) {
        if (change != FileReader.FileChange.UNCHANGED) {
            super.synchronize(change);
            switch (change) {
                case FileReader.FileChange.OVERWRITTEN:
                    clearInternalState();
                    break;
                case FileReader.FileChange.APPENDED:
                    clearInternalState();
                    break;
                default:
                    throw new RuntimeException("Unknown file change");
            }
        }
    }

    @Override
    public String getAsString() {
        return getModuleDescriptionEntry().getFullName();
    }

    @Override
    public String getAsString(String name) {
        if ("moduleName".equals(name))
            return getModuleDescriptionEntry().getFullName();
        else
            return null;
    }

    @Override
    public IEventLog getEventLog() {
        return eventLog;
    }

    @Override
    public long getBeginOffset() {
        return beginOffset;
    }

    @Override
    public long getEndOffset() {
        return endOffset;
    }

    @Override
    public EventEntry getEventEntry() {
        return eventEntry;
    }

    @Override
    public ModuleDescriptionEntry getModuleDescriptionEntry() {
        if (moduleDescriptionEntry == null)
            moduleDescriptionEntry = eventLog.getEventLogEntryCache().getModuleDescriptionEntry(getModuleId());
        return moduleDescriptionEntry;
    }

    @Override
    public int getNumEventLogEntries() {
        return eventLogEntries.size();
    }

    @Override
    public EventLogEntry getEventLogEntry(int index) {
        return eventLogEntries.get(index);
    }

    @Override
    public int getNumEventLogMessages() {
        return numEventLogMessages;
    }

    @Override
    public int getNumBeginSendEntries() {
        return numBeginSendEntries;
    }

    @Override
    public int getNumCustomEntries() {
        return numCustomEntries;
    }

    @Override
    public EventLogMessageEntry getEventLogMessage(int index) {
        Assert.isTrue(index >= 0);
        for (var eventLogEntry : eventLogEntries) {
            EventLogMessageEntry eventLogMessage = eventLogEntry instanceof EventLogMessageEntry ? (EventLogMessageEntry)eventLogEntry : null;
            if (eventLogMessage != null) {
                if (index == 0)
                    return eventLogMessage;
                else
                    index--;
            }
        }
        throw new RuntimeException("Index out of range");
    }

    @Override
    public long getEventNumber() {
        return eventEntry.eventNumber;
    }

    @Override
    public BigDecimal getSimulationTime() {
        return eventEntry.simulationTime;
    }

    @Override
    public int getModuleId() {
        return eventEntry.moduleId;
    }

    @Override
    public long getMessageId() {
        return eventEntry.messageId;
    }

    @Override
    public long getCauseEventNumber() {
        return eventEntry.causeEventNumber;
    }

    @Override
    public boolean isSelfMessage(BeginSendEntry beginSendEntry) {
        Assert.isTrue(beginSendEntry != null && this == beginSendEntry.getEvent());
        int index = beginSendEntry.getEntryIndex();
        if (index + 1 < (int)eventLogEntries.size())
            return eventLogEntries.get(index + 1) instanceof EndSendEntry;
        else
            return false;
    }

    @Override
    public boolean isSelfMessageProcessingEvent() {
        BeginSendEntry beginSendEntry = getCauseBeginSendEntry();
        return beginSendEntry != null && beginSendEntry.getEvent().isSelfMessage(beginSendEntry);
    }

    @Override
    public EndSendEntry getEndSendEntry(BeginSendEntry beginSendEntry) {
        Assert.isTrue(beginSendEntry != null && this == beginSendEntry.getEvent());
        for (int i = beginSendEntry.getEntryIndex() + 1; i < (int)eventLogEntries.size(); i++) {
            EventLogEntry eventLogEntry = eventLogEntries.get(i);
            EndSendEntry endSendEntry = eventLogEntry instanceof EndSendEntry ? (EndSendEntry)eventLogEntry : null;
            if (endSendEntry != null)
                return endSendEntry;
            // message deleted on the channel (skip all deletes until the matching one is found)
            DeleteMessageEntry deleteMessageEntry = eventLogEntry instanceof DeleteMessageEntry ? (DeleteMessageEntry)eventLogEntry : null;
            if (deleteMessageEntry != null && deleteMessageEntry.messageId == beginSendEntry.messageId)
                return null;
        }
        throw new RuntimeException("Neither EndSendEntry nor DeleteMessageEntry found");
    }

    public ComponentMethodEndEntry getComponentMethodEndEntry(ComponentMethodBeginEntry componentMethodBeginEntry) {
        Assert.isTrue(componentMethodBeginEntry != null && this == componentMethodBeginEntry.getEvent());
        int level = 0;
        for (int i = componentMethodBeginEntry.getEntryIndex() + 1; i < (int)eventLogEntries.size(); i++) {
            EventLogEntry eventLogEntry = eventLogEntries.get(i);
            if (eventLogEntry instanceof ComponentMethodBeginEntry)
                level++;
            ComponentMethodEndEntry componentMethodEndEntry = eventLogEntry instanceof ComponentMethodEndEntry ? (ComponentMethodEndEntry)eventLogEntry : null;
            if (componentMethodEndEntry != null) {
                if (level == 0)
                    return componentMethodEndEntry;
                else
                    level--;
            }
        }
        // premature end of file
        return null;
    }

    public final BigDecimal getTransmissionDelay(BeginSendEntry beginSendEntry) {
        Assert.isTrue(beginSendEntry != null && this == beginSendEntry.getEvent());
        EndSendEntry endSendEntry = getEndSendEntry(beginSendEntry);
        int index = beginSendEntry.getEntryIndex() + 1;
        BigDecimal transmissionDelay = BigDecimal.getZero();
        // there is at most one entry which specifies a transmission delay
        while (index < (int)eventLogEntries.size()) {
            EventLogEntry eventLogEntry = eventLogEntries.get(index);
            if (eventLogEntry instanceof SendDirectEntry) {
                SendDirectEntry sendDirectEntry = (SendDirectEntry)eventLogEntry;
                transmissionDelay = sendDirectEntry.transmissionDelay;
            }
            else {
                if (eventLogEntry instanceof SendHopEntry) {
                    SendHopEntry sendHopEntry = (SendHopEntry)eventLogEntry;
                    transmissionDelay = sendHopEntry.transmissionDelay;
                }
                else if (eventLogEntry == endSendEntry || (!(eventLogEntry instanceof EventLogMessageEntry) && !(eventLogEntry instanceof MessageDescriptionEntry)))
                    break;
            }
            if (!transmissionDelay.equals(BigDecimal.getZero()))
                break;
            index++;
        }
        return transmissionDelay;
    }

    public final BigDecimal getRemainingDuration(BeginSendEntry beginSendEntry) {
        Assert.isTrue(beginSendEntry != null && this == beginSendEntry.getEvent());
        EndSendEntry endSendEntry = getEndSendEntry(beginSendEntry);
        int index = beginSendEntry.getEntryIndex() + 1;
        BigDecimal remainingDuration = BigDecimal.getZero();
        // there is at most one entry which specifies a transmission delay
        while (index < (int)eventLogEntries.size()) {
            EventLogEntry eventLogEntry = eventLogEntries.get(index);
            if (eventLogEntry instanceof SendDirectEntry) {
                SendDirectEntry sendDirectEntry = (SendDirectEntry)eventLogEntry;
                remainingDuration = sendDirectEntry.remainingDuration;
            }
            else {
                if (eventLogEntry instanceof SendHopEntry) {
                    SendHopEntry sendHopEntry = (SendHopEntry)eventLogEntry;
                    remainingDuration = sendHopEntry.remainingDuration;
                }
                else if (eventLogEntry == endSendEntry || (!(eventLogEntry instanceof EventLogMessageEntry) && !(eventLogEntry instanceof MessageDescriptionEntry)))
                    break;
            }
            if (!remainingDuration.equals(BigDecimal.getZero()))
                break;
            index++;
        }
        return remainingDuration;
    }

    @Override
    public Event getPreviousEvent() {
        if (previousEvent == null && eventLog.getFirstEvent() != this) {
            ReadToEventLineResult result = new EventLog.ReadToEventLineResult();
            if (eventLog.readToEventLine(false, beginOffset, result)) {
                previousEvent = eventLog.getEventForBeginOffset(result.lineBeginOffset);
                if (previousEvent != null)
                    linkEvents(previousEvent, this);
            }
        }
        return (Event)previousEvent;
    }

    @Override
    public Event getNextEvent() {
        if (nextEvent == null && eventLog.getLastEvent() != this) {
            ReadToEventLineResult result = new EventLog.ReadToEventLineResult();
            if (eventLog.readToEventLine(true, endOffset, result)) {
                nextEvent = eventLog.getEventForBeginOffset(result.lineBeginOffset);
                if (nextEvent != null)
                    Event.linkEvents(this, nextEvent);
            }
        }
        return (Event)nextEvent;
    }

    @Override
    public Event getCauseEvent() {
        long causeEventNumber = getCauseEventNumber();
        if (causeEventNumber != -1)
            return (Event)eventLog.getEventForEventNumber(causeEventNumber);
        else
            return null;
    }

    @Override
    public BeginSendEntry getCauseBeginSendEntry() {
        MessageSendDependency cause = getCause();
        if (cause != null)
            return (BeginSendEntry)cause.getBeginMessageDescriptionEntry();
        else
            return null;
    }

    @Override
    public MessageSendDependency getCause() {
        if (cause == null) {
            Event event = getCauseEvent();
            if (event != null) {
                int index = event.findBeginSendEntryIndex(getMessageId());
                if (index != -1)
                    cause = new MessageSendDependency(eventLog, getCauseEventNumber(), index);
            }
        }
        return cause;
    }

    @Override
    public ArrayList<IMessageDependency> getCauses() {
        if (causes == null) {
            causes = new ArrayList<IMessageDependency>();
            // using "ce" from "E" line
            MessageSendDependency cause = getCause();
            if (cause != null)
                causes.add(cause);
            // add message reuses
            long eventNumber = getEventNumber();
            for (int i = 0; i < (int)eventLogEntries.size(); i++) {
                MessageDescriptionEntry messageDescriptionEntry = eventLogEntries.get(i) instanceof MessageDescriptionEntry ? (MessageDescriptionEntry)eventLogEntries.get(i) : null;
                if (messageDescriptionEntry != null && messageDescriptionEntry.previousEventNumber != -1 && messageDescriptionEntry.previousEventNumber != eventNumber)
                    causes.add(new MessageReuseDependency(eventLog, eventNumber, i));
            }
        }
        return causes;
    }

    @Override
    public ArrayList<IMessageDependency> getConsequences() {
        if (consequences == null) {
            consequences = new ArrayList<IMessageDependency>();
            // collect all message ids that are used in this event (eliminating duplicates)
            TreeSet<Long> messageIds = new TreeSet<Long>();
            if (getMessageId() != -1)
                messageIds.add(getMessageId());
            for (int i = 0; i < (int)eventLogEntries.size(); i++) {
                MessageDescriptionEntry messageDescriptionEntry = eventLogEntries.get(i) instanceof MessageDescriptionEntry ? (MessageDescriptionEntry)eventLogEntries.get(i) : null;
                if (messageDescriptionEntry != null) {
                    DeleteMessageEntry deleteMessageEntry = eventLogEntries.get(i) instanceof DeleteMessageEntry ? (DeleteMessageEntry)eventLogEntries.get(i) : null;
                    if (deleteMessageEntry != null) {
                        if (messageIds.contains(deleteMessageEntry.messageId))
                            messageIds.remove(deleteMessageEntry.messageId);
                    }
                    else {
                        messageIds.add(messageDescriptionEntry.messageId);
                        CloneMessageEntry cloneMessageEntry = eventLogEntries.get(i) instanceof CloneMessageEntry ? (CloneMessageEntry)eventLogEntries.get(i) : null;
                        if (cloneMessageEntry != null)
                            messageIds.add(cloneMessageEntry.cloneId);
                    }
                }
            }
            // collect reuses
            for (Long it : messageIds) {
                MessageDescriptionEntry reuseMessageDescriptionEntry = findReuseMessageDescriptionEntry(it);
                if (reuseMessageDescriptionEntry != null) {
                    Assert.isTrue(reuseMessageDescriptionEntry.previousEventNumber == getEventNumber());
                    consequences.add(new MessageReuseDependency(eventLog, reuseMessageDescriptionEntry.getEvent().getEventNumber(), reuseMessageDescriptionEntry.getEntryIndex()));
                }
            }
            // handle begin sends separately
            for (int i = 0; i < (int)eventLogEntries.size(); i++) {
                eventLog.progress();
                // using "t" from "ES" lines
                BeginSendEntry beginSendEntry = eventLogEntries.get(i) instanceof BeginSendEntry ? (BeginSendEntry)eventLogEntries.get(i) : null;
                if (beginSendEntry != null && messageIds.contains(beginSendEntry.messageId))
                    consequences.add(new MessageSendDependency(eventLog, getEventNumber(), i));
            }
        }
        return consequences;
    }

    @Override
    public void print(OutputStream stream) {
        print(stream, true);
    }

    @Override
    public void print() {
        print(System.out, true);
    }

    @Override
    public void print(OutputStream stream, boolean outputEventLogMessages) {
        for (var eventLogEntry : eventLogEntries)
            if (outputEventLogMessages || !(eventLogEntry instanceof EventLogMessageEntry))
                eventLogEntry.print(stream);
    }

    protected final void clearInternalState() {
        super.clearInternalState();
        beginOffset = -1;
        endOffset = -1;
        numEventLogMessages = -1;
        numBeginSendEntries = -1;
        numCustomEntries = -1;
        eventEntry = null;
        moduleDescriptionEntry = null;
        cause = null;
        causes = null;
        consequences = null;
        eventLogEntries.clear();
    }

    /**
     * Searches for the first eventlog entry that is reusing the provided message id.
     * The search starts from the immediately following event and ends at the end of
     * the eventlog. Returns null if no such eventlog entry is found.
     */
    protected final MessageDescriptionEntry findReuseMessageDescriptionEntry(long messageId) {
        // TODO: optimization: cache first message description entries between this event and the first index
        // 1. linear search among the events starting from the immediately following event until the first index
        Index index = null;
        Event event = getNextEvent();
        long previousEventNumber = getEventNumber();
        while (event != null) {
            if (event.getMessageId() == messageId)
                // we found an EventEntry but that is not a message reuse
                return null;
            else {
                MessageDescriptionEntry messageDescriptionEntry = event.findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
                if (messageDescriptionEntry != null)
                    return messageDescriptionEntry;
            }
            index = eventLog.getIndex(event.getEventNumber());
            if (index != null)
                break;
            event = event.getNextEvent();
        }
        // 2. linear search among the indices starting from the first index after this event until the last index
        event = null;
        while (index != null) {
            // check if there's any reference to this event
            if (index.containsReferenceRemovedEntry(previousEventNumber, -1)) {
                ArrayList<EventLogEntry> removedEntries = index.getRemovedEventLogEntries();
                for (int i = 0; i < (int)removedEntries.size(); i++) {
                    EventLogEntry eventLogEntry = removedEntries.get(i);
                    EventEntry eventEntry = eventLogEntry instanceof EventEntry ? (EventEntry)eventLogEntry : null;
                    MessageDescriptionEntry removedMessageDescriptionEntry = eventLogEntry instanceof MessageDescriptionEntry ? (MessageDescriptionEntry)eventLogEntry : null;
                    // looking for a removed eventlog entry (from this event) with the given message id
                    if ((removedMessageDescriptionEntry != null && removedMessageDescriptionEntry.messageId == messageId && removedMessageDescriptionEntry.getEvent() == this) || (eventEntry != null && eventEntry.messageId == messageId && eventEntry.getEvent() == this)) {
                        // there can be another entry that removed the one we are looking for
                        ArrayList<EventLogEntry> addedEntries = index.getAddedEventLogEntries();
                        for (int j = 0; j < (int)addedEntries.size(); j++) {
                            MessageDescriptionEntry messageDescriptionEntry = addedEntries.get(j) instanceof MessageDescriptionEntry ? (MessageDescriptionEntry)addedEntries.get(j) : null;
                            // we must make sure that the previousEventNumber matches
                            if (messageDescriptionEntry != null && messageDescriptionEntry.messageId == messageId && messageDescriptionEntry.previousEventNumber == previousEventNumber)
                                return messageDescriptionEntry;
                        }
                        // since there was no such entry the message is deleted between the two indices
                        // do a linear search between the previous and this index to find out what happened between them
                        Index previousIndex = index.getPreviousIndex();
                        if (previousIndex != null) {
                            event = (Event)eventLog.getEventForEventNumber(previousIndex.getIndexEntry().eventNumber);
                            if (event != null)
                                event = event.getNextEvent();
                            while (event != null) {
                                if (event.getMessageId() == messageId)
                                    // we found an EventEntry but that is not a message reuse
                                    return null;
                                else {
                                    MessageDescriptionEntry messageDescriptionEntry = event.findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
                                    if (messageDescriptionEntry != null)
                                        return messageDescriptionEntry;
                                }
                                if (event.getEventNumber() == index.getIndexEntry().eventNumber)
                                    break;
                                event = event.getNextEvent();
                            }
                        }
                        // we can stop here, because there is no sign of the message
                        return null;
                    }
                }
            }
            Index nextIndex = index.getNextIndex();
            if (nextIndex == null)
                event = index.getEvent();
            index = nextIndex;
        }
        // 3. linear search among the remaining events (after the last index)
        while (event != null) {
            if (event.getMessageId() == messageId)
                // we found an EventEntry but that is not a message reuse
                return null;
            else {
                MessageDescriptionEntry messageDescriptionEntry = event.findLocalReuseMessageDescriptionEntry(previousEventNumber, messageId);
                if (messageDescriptionEntry != null)
                    return messageDescriptionEntry;
            }
            event = event.getNextEvent();
        }
        // reached end of eventlog
        return null;
    }

    protected MessageDescriptionEntry findLocalReuseMessageDescriptionEntry(long previousEventNumber, long messageId) {
        for (EventLogEntry it : eventLogEntries) {
            if (it instanceof MessageDescriptionEntry) {
                MessageDescriptionEntry messageDescriptionEntry = (MessageDescriptionEntry)it;
                if (messageDescriptionEntry.messageId == messageId && messageDescriptionEntry.previousEventNumber == previousEventNumber)
                    return messageDescriptionEntry;
            }
        }
        return null;
    }
}