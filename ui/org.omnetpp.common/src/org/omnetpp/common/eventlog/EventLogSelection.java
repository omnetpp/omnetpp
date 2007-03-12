package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.virtualtable.IVirtualTableSelection;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors/viewers.
 *  
 * @author andras
 */
public class EventLogSelection implements IEventLogSelection, IVirtualTableSelection<IEvent>, Cloneable {

	// TODO: implement IStructuredSelection and IPropertySourceProvider
	private IEventLog eventLog;
	private List<IEvent> events;

	public EventLogSelection(IEventLog eventLog, List<IEvent> events) {
		Assert.isTrue(eventLog != null);
		Assert.isTrue(events != null);
		this.eventLog = eventLog;
		this.events = events;
	}

	public List<IEvent> getElements() {
		return events;
	}

	public Object getInput() {
		return eventLog;
	}
	
	public IEventLog getEventLog() {
		return eventLog;
	}

	public List<IEvent> getEvents() {
		return events;
	}

	public IEvent getFirstEvent() {
		return events.isEmpty() ? null : events.get(0);
	}

	public boolean isEmpty() {
		return events.isEmpty();
	}

	@Override
	public EventLogSelection clone() {
		ArrayList<IEvent> list = new ArrayList<IEvent>();
		for (IEvent e : events)
			list.add(e);
		return new EventLogSelection(this.eventLog, list);
	}
	
	@Override
	public boolean equals(Object o) {
		if (o == null || !(o instanceof EventLogSelection))
			return false;
		EventLogSelection sel = (EventLogSelection)o;
		if (sel.eventLog != eventLog)
			return false;
		if (sel.events.size() != events.size())
			return false;
		for (int i = 0; i<events.size(); i++)
			if (sel.events.get(i).getEventNumber() != events.get(i).getEventNumber())
				return false;
		return true;
	}
}
