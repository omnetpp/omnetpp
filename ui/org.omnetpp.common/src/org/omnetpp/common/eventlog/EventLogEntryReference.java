package org.omnetpp.common.eventlog;

import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogEntryReference {
	private int eventNumber;

	private int eventEntryIndex;

	public EventLogEntryReference(EventLogEntry eventLogEntry) {
		IEvent event = eventLogEntry.getEvent();
		eventNumber = event.getEventNumber();
		eventEntryIndex = eventLogEntry.getIndex();
	}

	public int getEventNumber() {
		return eventNumber;
	}
	
	public int getEventEntryIndex() {
		return eventEntryIndex;
	}
	
	public EventLogEntry getEventLogEntry(EventLogInput eventLogInput) {
		return getEventLogEntry(eventLogInput.getEventLog());
	}

	public EventLogEntry getEventLogEntry(IEventLog eventLog) {
		return eventLog.getEventForEventNumber(eventNumber).getEventLogEntry(eventEntryIndex);
	}
	
	public boolean isPresent(IEventLog eventLog) {
	    return eventLog.getEventForEventNumber(eventNumber) != null;
	}

	@Override
	public String toString() {
		return "#" + eventNumber + ":" + eventEntryIndex;
	}

	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + eventEntryIndex;
		result = prime * result + eventNumber;
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
		final EventLogEntryReference other = (EventLogEntryReference) obj;
		if (eventEntryIndex != other.eventEntryIndex)
			return false;
		if (eventNumber != other.eventNumber)
			return false;
		return true;
	}
}
