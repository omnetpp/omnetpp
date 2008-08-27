package org.omnetpp.eventlogtable.widgets;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.Viewer;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.virtualtable.IVirtualTableContentProvider;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;

/**
 * This class provides the content for the EventLogTable. The individual entries are wrapped with a reference
 * object so that the C library can manage memory (delete events, entries on demand) and we will still be
 * able to find what we are looking for.
 */
public class EventLogTableContentProvider implements IVirtualTableContentProvider<EventLogEntryReference> {
	protected static boolean debug = false;
	
	protected EventLogTableFacade eventLogTableFacade;

	protected EventLogInput eventLogInput;

	public EventLogEntryReference getFirstElement() {
		if (debug)
			System.out.println("Virtual table content provider getFirstElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return toEventLogEntryReference(eventLogTableFacade.getFirstEntry());
	}

	public EventLogEntryReference getLastElement() {
		if (debug)
			System.out.println("Virtual table content provider getLastElement");

		if (eventLogTableFacade == null)
			return null;
		else
			return toEventLogEntryReference(eventLogTableFacade.getLastEntry());
	}

	public long getDistanceToElement(EventLogEntryReference sourceElement, EventLogEntryReference targetElement, long limit)
	{
		if (debug)
			System.out.println("Virtual table content provider getDistanceToElement sourceElement: " + sourceElement + " targetElement: " + targetElement + " limit: " + limit);

		if (sourceElement == null || targetElement == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToEntry(sourceElement.getEventLogEntry(eventLogInput), targetElement.getEventLogEntry(eventLogInput), (int)limit);
	}

	public long getDistanceToFirstElement(EventLogEntryReference element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToFirstElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToFirstEntry(element.getEventLogEntry(eventLogInput), (int)limit);
	}

	public long getDistanceToLastElement(EventLogEntryReference element, long limit) {
		if (debug)
			System.out.println("Virtual table content provider getDistanceToLastElement element: " + element + " limit: " + limit);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getDistanceToLastEntry(element.getEventLogEntry(eventLogInput), (int)limit);
	}

	public EventLogEntryReference getNeighbourElement(EventLogEntryReference element, long distance) {
		if (debug)
			System.out.println("Virtual table content provider getNeighbourElement element: " + element + " distance: " + distance);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return null;
		else
			return toEventLogEntryReference(eventLogTableFacade.getNeighbourEntry(element.getEventLogEntry(eventLogInput), (int)distance));
	}

	public double getApproximatePercentageForElement(EventLogEntryReference element) {
		if (debug)
			System.out.println("Virtual table content provider getApproximatePercentageForElement element: " + element);

		if (element == null)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getApproximatePercentageForEntry(element.getEventLogEntry(eventLogInput));
	}

	public EventLogEntryReference getApproximateElementAt(double percentage) {
		if (debug)
			System.out.println("Virtual table content provider getApproximateElementAt percentage: " + percentage);

		if (percentage < 0 || percentage > 1)
			throw new IllegalArgumentException();

		if (eventLogTableFacade == null)
			return null;
		else
			return toEventLogEntryReference(eventLogTableFacade.getApproximateEventLogEntryAt(percentage));
	}

	public long getApproximateNumberOfElements() {
		if (debug)
			System.out.println("Virtual table content provider getApproximateNumberOfElements");

		if (eventLogTableFacade == null)
			return 0;
		else
			return eventLogTableFacade.getApproximateNumberOfEntries();
	}

	public EventLogEntryReference getClosestElement(EventLogEntryReference element) {
		if (element == null)
			throw new IllegalArgumentException();
		
		return toEventLogEntryReference(eventLogTableFacade.getClosestEntryInEvent(element.getEventLogEntry(eventLogInput)));
	}

	public void dispose() {
	}

	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		eventLogInput = (EventLogInput)newInput;
		
		if (eventLogInput == null)
			eventLogTableFacade = null;
		else
			eventLogTableFacade = eventLogInput.getEventLogTableFacade();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return eventLogTableFacade;
	}

	private EventLogEntryReference toEventLogEntryReference(EventLogEntry eventLogEntry) {
		if (eventLogEntry == null)
			return null;
		else {
			Assert.isTrue(eventLogInput.getEventLog().getEventForEventNumber(eventLogEntry.getEvent().getEventNumber()) != null);
			return new EventLogEntryReference(eventLogEntry);
		}
	}
}
