package org.omnetpp.experimental.seqchart.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLog;

/**
 * Selection that is published by the sequence chart tool
 * (SequenceChartToolEditor) and its associated log view.
 *  
 * @author andras
 */
public class EventLogSelection implements IEventLogSelection, Cloneable {

	private EventLog eventLog;
	private List<EventEntry> events;

	public EventLogSelection(EventLog eventLog, List<EventEntry> events) {
		Assert.isTrue(events!=null);
		this.eventLog = eventLog;
		this.events = events;
	}
	
	public EventLog getEventLog() {
		return eventLog;
	}

	public List<EventEntry> getEvents() {
		return events;
	}

	public EventEntry getFirstEvent() {
		return events.isEmpty()? null : events.get(0);
	}

	public boolean isEmpty() {
		return events.isEmpty();
	}

	@Override
	public EventLogSelection clone() {
		ArrayList<EventEntry> list = new ArrayList<EventEntry>();
		for (EventEntry e : events)
			list.add(e);
		return new EventLogSelection(this.eventLog, list);
	}
	
	@Override
	public boolean equals(Object o) {
		if (o==null || !(o instanceof EventLogSelection))
			return false;
		EventLogSelection sel = (EventLogSelection)o;
		if (sel.eventLog != eventLog)
			return false;
		if (sel.events.size() != events.size())
			return false;
		for (int i=0; i<events.size(); i++)
			if (sel.events.get(i).getEventNumber() != events.get(i).getEventNumber())
				return false;
		return true;
	}
}
