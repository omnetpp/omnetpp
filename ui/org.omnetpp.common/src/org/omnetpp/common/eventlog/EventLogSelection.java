package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.virtualtable.IVirtualTableSelection;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors and viewers.
 *  
 * @author andras
 */
public class EventLogSelection implements IEventLogSelection, IVirtualTableSelection<IEvent>, IStructuredSelection, Cloneable {
	/**
	 * The input where this selection is.
	 */
	protected EventLogInput eventLogInput;

	/**
	 * The selected events.
	 */
	protected List<IEvent> events;

	public EventLogSelection(EventLogInput eventLogInput, List<IEvent> events) {
		Assert.isTrue(eventLogInput != null);
		Assert.isTrue(events != null);
		this.eventLogInput = eventLogInput;
		this.events = events;
	}

	public List<IEvent> getElements() {
		return events;
	}

	public Object getInput() {
		return eventLogInput;
	}
	
	public IEventLog getEventLog() {
		return eventLogInput.getEventLog();
	}

	public EventLogInput getEventLogInput() {
		return eventLogInput;
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
		return new EventLogSelection(this.eventLogInput, list);
	}
	
	@Override
	public boolean equals(Object o) {
		if (o == null || !(o instanceof EventLogSelection))
			return false;
		EventLogSelection sel = (EventLogSelection)o;
		if (sel.eventLogInput != eventLogInput)
			return false;
		if (sel.events.size() != events.size())
			return false;
		for (int i = 0; i<events.size(); i++)
			if (sel.events.get(i).getEventNumber() != events.get(i).getEventNumber())
				return false;
		return true;
	}

	public Object getFirstElement() {
		if (events.size() == 0)
			return null;
		else
			return events.get(0);
	}

	public Iterator iterator() {
		return events.iterator();
	}

	public int size() {
		return events.size();
	}

	public Object[] toArray() {
		return events.toArray();
	}

	public List toList() {
		return events;
	}
}
