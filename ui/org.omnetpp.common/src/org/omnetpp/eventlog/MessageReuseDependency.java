package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

/**
 * Represents a message reuse dependency: the message has arrived at "cause", and it was sent out again at "consequence".
 */
public class MessageReuseDependency extends MessageDependencyBase implements IMessageDependency
{
    protected long causeEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
    protected long consequenceEventNumber = -1; // always present
    protected int eventLogEntryIndex; // refers to an entry of consequenceEvent

    public MessageReuseDependency(IEventLog eventLog, long consequenceEventNumber, int eventLogEntryIndex) {
        super(eventLog);
        this.consequenceEventNumber = consequenceEventNumber;
        this.eventLogEntryIndex = eventLogEntryIndex;
    }

    @Override
    public long getCauseEventNumber() {
        if (causeEventNumber == EventNumberKind.EVENT_NOT_YET_CALCULATED) {
            // only consequence is present, calculate cause from it
            IEvent consequenceEvent = getConsequenceEvent();
            Assert.isTrue(consequenceEvent != null);
            MessageDescriptionEntry messageDescriptionEntry = (MessageDescriptionEntry)consequenceEvent.getEventLogEntry(eventLogEntryIndex);
            causeEventNumber = messageDescriptionEntry.previousEventNumber;
        }
        return causeEventNumber;
    }

    @Override
    public IEvent getCauseEvent() {
        long causeEventNumber = getCauseEventNumber();
        if (causeEventNumber < 0)
            return null;
        else
            return eventLog.getEventForEventNumber(causeEventNumber);
    }

    @Override
    public BigDecimal getCauseSimulationTime() {
        if (causeEventNumber >= 0)
            return getCauseEvent().getSimulationTime();
        else
            return BigDecimal.getMinusOne();
    }

    @Override
    public long getConsequenceEventNumber() {
        return consequenceEventNumber;
    }

    @Override
    public IEvent getConsequenceEvent() {
        return eventLog.getEventForEventNumber(consequenceEventNumber);
    }

    @Override
    public BigDecimal getConsequenceSimulationTime() {
        return getConsequenceEvent().getSimulationTime();
    }

    @Override
    public MessageDescriptionEntry getBeginMessageDescriptionEntry() {
        Assert.isTrue(eventLogEntryIndex != -1);
        IEvent event = getConsequenceEvent();
        Assert.isTrue(event != null);
        return (MessageDescriptionEntry)event.getEventLogEntry(eventLogEntryIndex);
    }

    @Override
    public MessageDescriptionEntry getEndMessageDescriptionEntry() {
        return getBeginMessageDescriptionEntry();
    }

    @Override
    public MessageReuseDependency duplicate(IEventLog eventLog) {
        return new MessageReuseDependency(eventLog, consequenceEventNumber, eventLogEntryIndex);
    }

    @Override
    public boolean equals(IMessageDependency other) {
        MessageReuseDependency otherMessageReuseDependency = other instanceof MessageReuseDependency ? (MessageReuseDependency)other : null;
        return super.equals(other) && otherMessageReuseDependency != null && causeEventNumber == otherMessageReuseDependency.causeEventNumber && consequenceEventNumber == otherMessageReuseDependency.consequenceEventNumber && eventLogEntryIndex == otherMessageReuseDependency.eventLogEntryIndex;
    }

    @Override
    public void print() {
        print(System.out);
    }

    @Override
    public void print(OutputStream stream) {
        getConsequenceEvent().getEventEntry().print(stream);
        getBeginMessageDescriptionEntry().print(stream);
    }
}
