package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.entry.BeginSendEntry;
import org.omnetpp.eventlog.entry.EndSendEntry;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

/**
 * Represents a message send dependency: the message was sent out at "cause", and has arrived at "consequence".
 */
public class MessageSendDependency extends MessageDependencyBase implements IMessageDependency
{
    protected long causeEventNumber = -1; // always present
    protected long consequenceEventNumber = EventNumberKind.EVENT_NOT_YET_CALCULATED;
    protected int eventLogEntryIndex; // refers to an entry of causeEvent

    public MessageSendDependency(IEventLog eventLog, long causeEventNumber, int eventLogEntryIndex) {
        super(eventLog);
        Assert.isTrue(causeEventNumber >= 0);
        this.causeEventNumber = causeEventNumber;
        this.eventLogEntryIndex = eventLogEntryIndex;
    }

    @Override
    public long getCauseEventNumber() {
        return causeEventNumber;
    }

    @Override
    public IEvent getCauseEvent() {
        return eventLog.getEventForEventNumber(causeEventNumber);
    }

    @Override
    public BigDecimal getCauseSimulationTime() {
        return getCauseEvent().getSimulationTime();
    }

    @Override
    public long getConsequenceEventNumber()
    {
        if (consequenceEventNumber == EventNumberKind.EVENT_NOT_YET_CALCULATED || consequenceEventNumber == EventNumberKind.EVENT_NOT_YET_REACHED) {
            // only cause is present, calculate consequence from it.
            // when a message is scheduled/sent, we don't know the arrival event number
            // yet, only the simulation time is recorded in the eventlog file.
            // So here we have to look through all events at the arrival time,
            // and find the one "caused by" our message.
            BigDecimal consequenceTime = getConsequenceSimulationTime();
            if (consequenceTime.equals(BigDecimal.getMinusOne()))
                consequenceEventNumber = EventNumberKind.NO_SUCH_EVENT;
            else if (consequenceTime.greater(eventLog.getLastEvent().getSimulationTime()))
                consequenceEventNumber = EventNumberKind.EVENT_NOT_YET_REACHED;
            else {
                IEvent event = eventLog.getEventForSimulationTime(consequenceTime, MatchKind.FIRST_OR_PREVIOUS);
                MessageDescriptionEntry messageDescriptionEntry = getBeginMessageDescriptionEntry();
                // LONG RUNNING OPERATION
                while (event != null) {
                    eventLog.progress();
                    if (event.getCauseEventNumber() == getCauseEventNumber() && event.getMessageId() == messageDescriptionEntry.messageId) {
                        consequenceEventNumber = event.getEventNumber();
                        break;
                    }
                    if (event.getSimulationTime().greater(consequenceTime)) {
                        // no more event at that simulation time, and consequence event
                        // still not found. It must have been cancelled (self message),
                        // or it is not in the file (filtered out by the user, etc).
                        consequenceEventNumber = EventNumberKind.NO_SUCH_EVENT;
                        break;
                    }
                    event = event.getNextEvent();
                }
                // end of file
                if (event == null)
                    consequenceEventNumber = EventNumberKind.EVENT_NOT_YET_REACHED;
            }
        }
        return consequenceEventNumber;
    }

    @Override
    public IEvent getConsequenceEvent() {
        long consequenceEventNumber = getConsequenceEventNumber();
        if (consequenceEventNumber < 0)
            return null;
        else
            return eventLog.getEventForEventNumber(consequenceEventNumber);
    }

    @Override
    public BigDecimal getConsequenceSimulationTime() {
        if (consequenceEventNumber >= 0)
            return getConsequenceEvent().getSimulationTime();
        else {
            // find the arrival time of the message
            IEvent event = getCauseEvent();
            BeginSendEntry beginSendEntry = (BeginSendEntry)(event.getEventLogEntry(eventLogEntryIndex));
            EndSendEntry endSendEntry = event.getEndSendEntry(beginSendEntry);
            if (endSendEntry != null)
                return endSendEntry.arrivalTime;
            else
                return BigDecimal.getMinusOne();
        }
    }

    @Override
    public MessageDescriptionEntry getBeginMessageDescriptionEntry() {
        Assert.isTrue(eventLogEntryIndex != -1);
        IEvent event = getCauseEvent();
        Assert.isTrue(event != null);
        return (MessageDescriptionEntry)event.getEventLogEntry(eventLogEntryIndex);
    }

    @Override
    public MessageDescriptionEntry getEndMessageDescriptionEntry() {
        IEvent event = getCauseEvent();
        BeginSendEntry beginSendEntry = (BeginSendEntry)(event.getEventLogEntry(eventLogEntryIndex));
        EndSendEntry endSendEntry = event.getEndSendEntry(beginSendEntry);
        return endSendEntry;
    }

    @Override
    public MessageSendDependency duplicate(IEventLog eventLog) {
        return new MessageSendDependency(eventLog, causeEventNumber, eventLogEntryIndex);
    }

    @Override
    public boolean equals(IMessageDependency other) {
        MessageSendDependency otherMessageSendDependency = other instanceof MessageSendDependency ? (MessageSendDependency)other : null;
        return super.equals(other) && otherMessageSendDependency != null && causeEventNumber == otherMessageSendDependency.causeEventNumber && consequenceEventNumber == otherMessageSendDependency.consequenceEventNumber && eventLogEntryIndex == otherMessageSendDependency.eventLogEntryIndex;
    }

    @Override
    public void print() {
        print(System.out);
    }

    @Override
    public void print(OutputStream stream) {
        getCauseEvent().getEventEntry().print(stream);
        getBeginMessageDescriptionEntry().print(stream);
    }
}
