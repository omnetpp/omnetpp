package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

/**
 * Represents a chain of message dependencies starting with beginMessageDependency and ending with endMessageDependency.
 */
public class FilteredMessageDependency extends MessageDependencyBase implements IMessageDependency
{
    public enum Kind
    {
        UNDEFINED(0),
        SENDS(1),
        REUSES(2),
        MIXED(3); // this relies on the fact that: 1 | 2 = 3

        public static final int SIZE = java.lang.Integer.SIZE;

        private int intValue;
        private static java.util.HashMap<Integer, Kind> mappings;
        private static java.util.HashMap<Integer, Kind> getMappings()
        {
            if (mappings == null)
            {
                synchronized (Kind.class)
                {
                    if (mappings == null)
                    {
                        mappings = new java.util.HashMap<Integer, Kind>();
                    }
                }
            }
            return mappings;
        }

        private Kind(int value)
        {
            intValue = value;
            getMappings().put(value, this);
        }

        public int getValue()
        {
            return intValue;
        }

        public static Kind forValue(int value)
        {
            return getMappings().get(value);
        }
    }

    protected Kind kind;
    protected IMessageDependency beginMessageDependency;
    protected IMessageDependency endMessageDependency;


    /**************************************************/

    public FilteredMessageDependency(IEventLog eventLog, Kind kind, IMessageDependency beginMessageDependency, IMessageDependency endMessageDependency) {
        super(eventLog);
        this.kind = kind;
        this.beginMessageDependency = beginMessageDependency;
        this.endMessageDependency = endMessageDependency;
    }

    public final IMessageDependency getBeginMessageDependency() {
        return beginMessageDependency;
    }

    public final IMessageDependency getEndMessageDependency() {
        return endMessageDependency;
    }

    @Override
    public long getCauseEventNumber() {
        return beginMessageDependency.getCauseEventNumber();
    }

    @Override
    public IEvent getCauseEvent() {
        long causeEventNumber = beginMessageDependency.getCauseEventNumber();
        if (causeEventNumber < 0)
            return null;
        else
            return eventLog.getEventForEventNumber(causeEventNumber);
    }

    @Override
    public BigDecimal getCauseSimulationTime() {
        return beginMessageDependency.getCauseSimulationTime();
    }

    @Override
    public long getConsequenceEventNumber() {
        return endMessageDependency.getConsequenceEventNumber();
    }

    @Override
    public IEvent getConsequenceEvent() {
        long consequenceEventNumber = endMessageDependency.getConsequenceEventNumber();
        if (consequenceEventNumber < 0)
            return null;
        else
            return eventLog.getEventForEventNumber(consequenceEventNumber);
    }

    @Override
    public BigDecimal getConsequenceSimulationTime() {
        return endMessageDependency.getConsequenceSimulationTime();
    }

    @Override
    public MessageDescriptionEntry getBeginMessageDescriptionEntry() {
        return beginMessageDependency.getBeginMessageDescriptionEntry();
    }

    @Override
    public MessageDescriptionEntry getEndMessageDescriptionEntry() {
        return beginMessageDependency.getEndMessageDescriptionEntry();
    }

    public final Kind getKind() {
        return kind;
    }

    @Override
    public FilteredMessageDependency duplicate(IEventLog eventLog) {
        return new FilteredMessageDependency(eventLog, kind, beginMessageDependency.duplicate(eventLog), endMessageDependency.duplicate(eventLog));
    }

    @Override
    public boolean equals(IMessageDependency other) {
        FilteredMessageDependency otherFiltered = other instanceof FilteredMessageDependency ? (FilteredMessageDependency)other : null;
        return super.equals(other) && otherFiltered != null && beginMessageDependency.equals(otherFiltered.beginMessageDependency) && endMessageDependency.equals(otherFiltered.endMessageDependency);
    }

    @Override
    public void print(OutputStream stream) {
        beginMessageDependency.print(stream);
        endMessageDependency.print(stream);
    }
}
