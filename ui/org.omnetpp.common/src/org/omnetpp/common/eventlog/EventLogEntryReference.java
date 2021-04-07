/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;

public class EventLogEntryReference {
    private long eventNumber;

    private int entryIndex;

    public EventLogEntryReference(long eventNumber, int entryIndex) {
        this.eventNumber = eventNumber;
        this.entryIndex = entryIndex;
        Assert.isTrue(eventNumber != -1 && entryIndex != -1);
    }

    public EventLogEntryReference(EventLogEntry eventLogEntry) {
        this(eventLogEntry.getEventNumber(), eventLogEntry.getEntryIndex());
    }

    public long getEventNumber() {
        return eventNumber;
    }

    public int getEntryIndex() {
        return entryIndex;
    }

    public IEvent getEvent(EventLogInput eventLogInput) {
        return getEvent(eventLogInput.getEventLog());
    }

    public IEvent getEvent(IEventLog eventLog) {
        return eventLog.getEventForEventNumber(eventNumber);
    }

    public EventLogEntry getEventLogEntry(EventLogInput eventLogInput) {
        return getEventLogEntry(eventLogInput.getEventLog());
    }

    public EventLogEntry getEventLogEntry(IEventLog eventLog) {
        return eventLog.getEventForEventNumber(eventNumber).getEventLogEntry(entryIndex);
    }

    public boolean isPresent(IEventLog eventLog) {
        return eventLog.getEventForEventNumber(eventNumber) != null;
    }

    @Override
    public String toString() {
        return "#" + eventNumber + ":" + entryIndex;
    }

    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + entryIndex;
        result = prime * result + (int) (eventNumber ^ (eventNumber >>> 32));
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        EventLogEntryReference other = (EventLogEntryReference) obj;
        if (entryIndex != other.entryIndex)
            return false;
        if (eventNumber != other.eventNumber)
            return false;
        return true;
    }
}
