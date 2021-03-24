package org.omnetpp.eventlog;

import java.io.OutputStream;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.eventlog.entry.MessageDescriptionEntry;

public abstract class MessageDependencyBase
{
    protected IEventLog eventLog;

    public MessageDependencyBase(IEventLog eventLog) {
        this.eventLog = eventLog;
    }

    public static boolean corresponds(IMessageDependency dependency1, IMessageDependency dependency2) {
        if (dependency1 == null || dependency2 == null)
            return false;
        else {
            MessageDescriptionEntry entry1 = dependency1.getBeginMessageDescriptionEntry();
            MessageDescriptionEntry entry2 = dependency2.getBeginMessageDescriptionEntry();
            if (entry1 == null || entry2 == null)
                return false;
            else
                return (entry1.messageId != -1 && entry1.messageId == entry2.messageId) || (entry1.messageTreeId != -1 && entry1.messageTreeId == entry2.messageTreeId) || (entry1.messageEncapsulationId != -1 && entry1.messageEncapsulationId == entry2.messageEncapsulationId) || (entry1.messageEncapsulationTreeId != -1 && entry1.messageEncapsulationTreeId == entry2.messageEncapsulationTreeId);
        }
    }

    public boolean equals(MessageDependencyBase other) {
        Assert.isTrue(eventLog == other.eventLog);
        return true;
    }

    public void print() {
        print(System.out);
    }

    public abstract void print(OutputStream stream);
}