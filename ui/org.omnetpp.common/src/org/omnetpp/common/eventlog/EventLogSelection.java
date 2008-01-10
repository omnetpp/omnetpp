package org.omnetpp.common.eventlog;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.virtualtable.IVirtualTableSelection;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors and viewers.
 *  
 * @author andras
 */
public class EventLogSelection implements IEventLogSelection, IVirtualTableSelection<Integer>, IStructuredSelection, Cloneable {
	/**
	 * The input where this selection is.
	 */
	protected EventLogInput eventLogInput;

	/**
	 * The selected event numbers.
	 */
	protected List<Integer> eventNumbers;

	public EventLogSelection(EventLogInput eventLogInput, List<Integer> eventNumbers) {
		Assert.isTrue(eventLogInput != null);
		Assert.isTrue(eventNumbers != null);
		this.eventLogInput = eventLogInput;
		this.eventNumbers = eventNumbers;
	}

	public List<Integer> getElements() {
		return eventNumbers;
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

	public List<Integer> getEventNumbers() {
		return eventNumbers;
	}

	public Integer getFirstEventNumber() {
		return eventNumbers.isEmpty() ? null : eventNumbers.get(0);
	}

	public boolean isEmpty() {
		return eventNumbers.isEmpty();
	}

	@Override
	public EventLogSelection clone() {
		ArrayList<Integer> list = new ArrayList<Integer>();
		for (Integer e : eventNumbers)
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
		if (sel.eventNumbers.size() != eventNumbers.size())
			return false;
		for (int i = 0; i<eventNumbers.size(); i++)
			if (sel.eventNumbers.get(i) != eventNumbers.get(i))
				return false;
		return true;
	}

	public Object getFirstElement() {
		if (eventNumbers.size() == 0)
			return null;
		else
			return eventNumbers.get(0);
	}

	public Iterator<Integer> iterator() {
		return eventNumbers.iterator();
	}

	public int size() {
		return eventNumbers.size();
	}

	public Object[] toArray() {
		return eventNumbers.toArray();
	}

	public List<Integer> toList() {
		return eventNumbers;
	}
}
