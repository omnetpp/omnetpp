package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.omnetpp.common.util.BigDecimal;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

public interface IMessageDependency
{
    public long getCauseEventNumber();
    public IEvent getCauseEvent();
    public BigDecimal getCauseSimulationTime();

    public long getConsequenceEventNumber();
    public IEvent getConsequenceEvent();
    public BigDecimal getConsequenceSimulationTime();

    public MessageDescriptionEntry getBeginMessageDescriptionEntry();
    public MessageDescriptionEntry getEndMessageDescriptionEntry();

    public IMessageDependency duplicate(IEventLog eventLog);
    public boolean equals(IMessageDependency other);

    public void print();
    public void print(OutputStream stream);
}
