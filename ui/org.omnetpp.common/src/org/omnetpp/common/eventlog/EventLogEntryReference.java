package org.omnetpp.common.eventlog;

import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogEntryReference {
	private IEventLog eventLog;

	private int eventNumber;

	private int eventEntryIndex;

	public EventLogEntryReference(EventLogEntry eventLogEntry) {
		IEvent event = eventLogEntry.getEvent();
		eventLog = event.getEventLog();
		eventNumber = event.getEventNumber();
		eventEntryIndex = eventLogEntry.getIndex();
	}
	
	public EventLogEntry getEventLogEntry() {
		return eventLog.getEventForEventNumber(eventNumber).getEventLogEntry(eventEntryIndex);
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
