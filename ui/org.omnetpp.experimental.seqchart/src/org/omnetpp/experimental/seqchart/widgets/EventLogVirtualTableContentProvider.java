package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEventLog;

public class EventLogVirtualTableContentProvider implements IVirtualTableContentProvider {
	protected static boolean debug = false;

	protected IEventLog eventLog;
	
	protected EventLogTableFacade eventLogTableFacade;

	public Object getFirstElement() {
		if (debug)
			System.out.println("Virtual table content provider getFirstElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getFirstEntry();
	}

	public Object getLastElement() {
		if (debug)
			System.out.println("Virtual table content provider getLastElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getLastEntry();
	}

	public long getDistanceToElement(Object sourceElement, Object targetElement, long limit)
	{
		if (debug)
			System.out.println("Virtual table content provider getDistanceToElement sourceElement: " + sourceElement + " targetElement: " + targetElement + " limit: " + limit);

		if (sourceElement == null || targetElement == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToEntry((EventLogEntry)sourceElement, (EventLogEntry)targetElement, (int)limit);
	}

	public long getDistanceToFirstElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToFirstEntry((EventLogEntry)element, (int)limit);
	}

	public long getDistanceToLastElement(Object element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToLastEntry((EventLogEntry)element, (int)limit);
	}

	public Object getNeighbourElement(Object element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getNeighbourEntry((EventLogEntry)element, (int)distance);
	}

	public double getApproximatePercentageForElement(Object element) {
		if (debug)
			System.out.println("Virtual table content provider getApproximatePercentageForElement element: " + element);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getApproximatePercentageForEntry((EventLogEntry)element);
	}

	public Object getApproximateElementAt(double percentage) {
		if (debug)
			System.out.println("Virtual table content provider getApproximateElementAt percentage: " + percentage);

		if (percentage < 0 || percentage > 1)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getApproximateEventLogEntryTableAt(percentage);
	}

	public long getApproximateNumberOfElements() {
		if (debug)
			System.out.println("Virtual table content provider getApproximateNumberOfElements");

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getApproximateNumberOfEntries();
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		eventLog = (IEventLog)newInput;
		
		if (eventLog == null)
			eventLogTableFacade = null;
		else
			eventLogTableFacade = new EventLogTableFacade(eventLog);
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return eventLogTableFacade;
	}
}