package org.omnetpp.eventlogtable.widgets;

import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.virtualtable.IVirtualTableContentProvider;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;

public class EventLogTableContentProvider implements IVirtualTableContentProvider<EventLogEntry> {
	protected static boolean debug = false;
	
	protected EventLogTableFacade eventLogTableFacade;

	public EventLogEntry getFirstElement() {
		if (debug)
			System.out.println("Virtual table content provider getFirstElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getFirstEntry();
	}

	public EventLogEntry getLastElement() {
		if (debug)
			System.out.println("Virtual table content provider getLastElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getLastEntry();
	}

	public long getDistanceToElement(EventLogEntry sourceElement, EventLogEntry targetElement, long limit)
	{
		if (debug)
			System.out.println("Virtual table content provider getDistanceToElement sourceElement: " + sourceElement + " targetElement: " + targetElement + " limit: " + limit);

		if (sourceElement == null || targetElement == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToEntry(sourceElement, targetElement, (int)limit);
	}

	public long getDistanceToFirstElement(EventLogEntry element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToFirstEntry(element, (int)limit);
	}

	public long getDistanceToLastElement(EventLogEntry element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToLastEntry(element, (int)limit);
	}

	public EventLogEntry getNeighbourElement(EventLogEntry element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return null;
		else
			return eventLogTableFacade.getNeighbourEntry(element, (int)distance);
	}

	public double getApproximatePercentageForElement(EventLogEntry element) {
		if (debug)
			System.out.println("Virtual table content provider getApproximatePercentageForElement element: " + element);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getApproximatePercentageForEntry(element);
	}

	public EventLogEntry getApproximateElementAt(double percentage) {
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

	public EventLogEntry getClosestElement(EventLogEntry fixPointElement) {
		return eventLogTableFacade.getClosestEntry(fixPointElement);
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		EventLogInput eventLogInput = (EventLogInput)newInput;
		
		if (eventLogInput == null)
			eventLogTableFacade = null;
		else
			eventLogTableFacade = eventLogInput.getEventLogTableFacade();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return eventLogTableFacade;
	}
}