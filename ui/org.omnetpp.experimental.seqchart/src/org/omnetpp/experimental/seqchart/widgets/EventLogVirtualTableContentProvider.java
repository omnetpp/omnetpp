package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogFacade;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogVirtualTableContentProvider implements IVirtualTableContentProvider {
	protected static boolean debug = true;

	protected IEventLog eventLog;
	
	protected EventLogFacade eventLogFacade;

	public Object getFirstElement() {
		if (debug)
			System.out.println("Virtual table content provider getFirstElement");

		if (eventLogFacade == null)
			return null;
		else
			return eventLogFacade.getFirstEventLogTableEntry();
	}

	public Object getLastElement() {
		if (debug)
			System.out.println("Virtual table content provider getLastElement");

		if (eventLogFacade == null)
			return null;
		else
			return eventLogFacade.getLastEventLogTableEntry();
	}

	public long getDistanceToElement(Object sourceElement, Object targetElement, long limit)
	{
		if (debug)
			System.out.println("Virtual table content provider getDistanceToElement: " + limit);

		if (sourceElement == null || targetElement == null)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return 0;
		else
			return eventLogFacade.getDistanceToEventLogTableEntry((EventLogEntry)sourceElement, (EventLogEntry)targetElement, (int)limit);
	}

	public long getDistanceToFirstElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return 0;
		else
			return eventLogFacade.getDistanceToFirstEventLogTableEntry((EventLogEntry)element, (int)limit);
	}

	public long getDistanceToLastElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return 0;
		else
			return eventLogFacade.getDistanceToLastEventLogTableEntry((EventLogEntry)element, (int)limit);
	}

	public Object getNeighbourElement(Object element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement: " + distance);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return null;
		else
			return eventLogFacade.getNeighbourEventLogTableEntry((EventLogEntry)element, (int)distance);
	}

	public double getApproximatePercentageForElement(Object element) {
		if (debug)
			System.out.println("Virtual table content provider getApproximatePercentageForElement");

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return 0;
		else
			return eventLogFacade.getApproximatePercentageForEventLogTableEntry((EventLogEntry)element);
	}

	public Object getApproximateElementAt(double percentage) {
		if (debug)
			System.out.println("Virtual table content provider getApproximateElementAt: " + percentage);

		if (percentage < 0 || percentage > 1)
			throw new IllegalArgumentException();

		if (eventLogFacade == null)
			return null;
		else
			return eventLogFacade.getApproximateEventLogEntryTableAt(percentage);
	}

	public long getApproximateNumberOfElements() {
		if (debug)
			System.out.println("Virtual table content provider getApproximateNumberOfElements");

		if (eventLogFacade == null)
			return 0;
		else
			return eventLogFacade.getApproximateNumberOfEventLogTableEntries();
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		eventLog = (IEventLog)newInput;
		
		if (eventLog == null)
			eventLogFacade = null;
		else
			eventLogFacade = new EventLogFacade(eventLog);
	}

	public EventLogFacade getEventLogFacade() {
		return eventLogFacade;
	}
}