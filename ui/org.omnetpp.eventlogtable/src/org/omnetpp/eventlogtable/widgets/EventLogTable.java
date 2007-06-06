package org.omnetpp.eventlogtable.widgets;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.QualifiedName;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.editors.EventLogTableContributor;

public class EventLogTable
	extends VirtualTable<EventLogEntryReference>
	implements IEventLogChangeListener
{
	private static final boolean debug = true;

	private boolean followEnd = false; // when the event log changes should we follow it or not?

	public EventLogTable(Composite parent, int style) {
		super(parent, style);

		setContentProvider(new EventLogTableContentProvider());
		setRowRenderer(new EventLogTableRowRenderer());

		TableColumn tableColumn = createColumn();
		tableColumn.setWidth(60);
		tableColumn.setText("Event #");

		tableColumn = createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Time");

		tableColumn = createColumn();
		tableColumn.setWidth(2000);
		tableColumn.setText("Details");
		
		addDisposeListener(new DisposeListener() {
			public void widgetDisposed(DisposeEvent e) {
				EventLogInput eventLogInput = getEventLogInput();

				if (eventLogInput != null) {
					storeState(eventLogInput.getFile());
					getEventLogInput().removeEventLogChangedListener(EventLogTable.this);
				}
			}
		});
	}

	public void setEventLogTableContributor(EventLogTableContributor eventLogTableContributor) {
		MenuManager menuManager = new MenuManager();
		eventLogTableContributor.contributeToPopupMenu(menuManager);
		setMenu(menuManager.createContextMenu(this));
	}

	@Override
	public ISelection getSelection() {
		List<EventLogEntryReference> selectionElements = getSelectionElements();
		
		if (selectionElements == null)
			return null;
		else {
			ArrayList<IEvent> selectionEvents = new ArrayList<IEvent>();
			IEventLog eventLog = getEventLog();
			
			for (EventLogEntryReference selectionElement : selectionElements)
				selectionEvents.add(eventLog.getEventForEventNumber(selectionElement.getEventNumber()));
	
			return new EventLogSelection(getEventLogInput(), selectionEvents);
		}
	}

	@Override
	public void setSelection(ISelection selection) {
		if (selection instanceof EventLogSelection) {
			EventLogSelection eventLogSelection = (EventLogSelection)selection;
			List<EventLogEntryReference> eventLogEntries = new ArrayList<EventLogEntryReference>();

			for (IEvent event : eventLogSelection.getEvents())
				eventLogEntries.add(new EventLogEntryReference(event.getEventEntry()));
			
			super.setSelection(new VirtualTableSelection<EventLogEntryReference>(eventLogSelection.getEventLogInput(), eventLogEntries));
		}
	}
	
	@Override
	public void setInput(Object input) {
		EventLogInput eventLogInput = getEventLogInput();

		// store current position
		if (eventLogInput != null) {
			eventLogInput.removeEventLogChangedListener(this);
			storeState(eventLogInput.getFile());
		}

		super.setInput(input);

		// restore last known position
		if (input != null) {
			eventLogInput = (EventLogInput)input;
			eventLogInput.addEventLogChangedListener(this);
			restoreState(eventLogInput.getFile());
		}
	}
	
	@Override
	protected void relocateFixPoint(EventLogEntryReference element, int distance) {
		Assert.isTrue(getEventLog().getEventForEventNumber(element.getEventNumber()) != null);
		super.relocateFixPoint(element, distance);
	}
	
	@Override
	public void scroll(int numberOfElements) {
		super.scroll(numberOfElements);
		followEnd = false;
	}
	
	@Override
	public void scrollToElement(EventLogEntryReference element) {
		super.scrollToElement(element);
		followEnd = false;
	}
	
	@Override
	public void scrollToEnd() {
		super.scrollToEnd();
		followEnd = true;
	}

	public EventLogInput getEventLogInput() {
		return (EventLogInput)getInput();		
	}

	public IEventLog getEventLog() {
		EventLogInput eventLogInput = getEventLogInput();
		
		if (eventLogInput == null)
			return null;
		else
			return eventLogInput.getEventLog();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return ((EventLogInput)getInput()).getEventLogTableFacade();
	}

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getDisplayMode() {
		return getEventLogTableFacade().getDisplayMode();
	}

	public void setDisplayMode(int i) {
		getEventLogTableFacade().setDisplayMode(i);
	}

	public int getFilterMode() {
		return getEventLogTableFacade().getFilterMode();
	}

	public void setFilterMode(int i) {
		getEventLogTableFacade().setFilterMode(i);
		stayNear();
	}
	
	public String getCustomFilter() {
		return getEventLogTableFacade().getCustomFilter();
	}

	public void setCustomFilter(String pattern) {
		getEventLogTableFacade().setCustomFilter(pattern);
	}

	public void eventLogAppended() {
		if (debug)
			System.out.println("EventLogTable got notification about event log change");

		configureVerticalScrollBar();
		updateVerticalBarPosition();

		if (followEnd)
		{
			if (debug)
				System.out.println("Scrolling to follow event log change");
			
			scrollToEnd();
		}
		else
			redraw();
	}
	
	public void eventLogFiltered() {
		if (fixPointElement != null) {
			FilteredEventLog filteredEventLog = (FilteredEventLog)getEventLog();
			IEvent closestEvent = filteredEventLog.getMatchingEventInDirection(fixPointElement.getEventNumber(), false);
			
			if (closestEvent != null)
				relocateFixPoint(new EventLogEntryReference(closestEvent.getEventEntry()), 0);
			else {
				closestEvent = filteredEventLog.getMatchingEventInDirection(fixPointElement.getEventNumber(), true);
	
				if (closestEvent != null)
					relocateFixPoint(new EventLogEntryReference(closestEvent.getEventEntry()), 0);
			}
		}

		redraw();
	}

	protected final QualifiedName EVENT_NUMBER_PROPERTY = new QualifiedName("EventLogTable", "EventNumber");
	
	protected final QualifiedName FILTER_MODE_PROPERTY = new QualifiedName("EventLogTable", "FilterMode");
	
	protected final QualifiedName CUSTOM_FILTER_PROPERTY = new QualifiedName("EventLogTable", "CustomFilter");
	
	protected final QualifiedName DISPLAY_MODE_PROPERTY = new QualifiedName("EventLogTable", "DisplayMode");
	
	public void restoreState(IResource resource) {
		try {
			String filterMode = resource.getPersistentProperty(FILTER_MODE_PROPERTY);
			if (filterMode != null)
				setFilterMode(Integer.parseInt(filterMode));

			String customFilter = resource.getPersistentProperty(CUSTOM_FILTER_PROPERTY);
			if (customFilter != null)
				setCustomFilter(customFilter);
			
			String displayMode = resource.getPersistentProperty(DISPLAY_MODE_PROPERTY);
			if (displayMode != null)
				setDisplayMode(Integer.parseInt(displayMode));

			String eventNumber = resource.getPersistentProperty(EVENT_NUMBER_PROPERTY);
			if (eventNumber != null) {
				IEvent event = getEventLog().getEventForEventNumber(Integer.parseInt(eventNumber));
				
				if (event != null)
					gotoElement(new EventLogEntryReference(event.getEventEntry()));
			}
		}
		catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}

	public void storeState(IResource resource) {
		try {
			EventLogEntryReference eventLogEntryReference = getTopVisibleElement();
	
			if (eventLogEntryReference != null) {
				String eventNumber = String.valueOf(eventLogEntryReference.getEventLogEntry(getEventLogInput()).getEvent().getEventNumber());	
				resource.setPersistentProperty(EVENT_NUMBER_PROPERTY, eventNumber);
			}
			
			resource.setPersistentProperty(FILTER_MODE_PROPERTY, String.valueOf(getFilterMode()));
			resource.setPersistentProperty(CUSTOM_FILTER_PROPERTY, getCustomFilter());
			resource.setPersistentProperty(DISPLAY_MODE_PROPERTY, String.valueOf(getDisplayMode()));
		}
		catch (CoreException e) {
			throw new RuntimeException(e);
		}
	}
}
