package org.omnetpp.eventlogtable.widgets;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
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
import org.omnetpp.common.util.Base64Serializer;
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

	private final QualifiedName EVENT_LOG_TABLE_STATE_PROPERTY = new QualifiedName("EventLogTable", "State");

	private boolean followEnd = false; // when the event log changes should we follow it or not?

	private EventLogInput eventLogInput;
	
	private IEventLog eventLog;

	private EventLogTableFacade eventLogTableFacade;

	/*************************************************************************************
	 * CONSTRUCTION
	 */

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
				if (eventLogInput != null) {
					storeState(eventLogInput.getFile());
					eventLogInput.removeEventLogChangedListener(EventLogTable.this);
				}
			}
		});
	}

	public void setEventLogTableContributor(EventLogTableContributor eventLogTableContributor) {
		MenuManager menuManager = new MenuManager();
		eventLogTableContributor.contributeToPopupMenu(menuManager);
		setMenu(menuManager.createContextMenu(this));
	}

	/*************************************************************************************
	 * OVERRIDING BEHAVIOR
	 */

	@Override
	public ISelection getSelection() {
		List<EventLogEntryReference> selectionElements = getSelectionElements();
		
		if (selectionElements == null)
			return null;
		else {
			ArrayList<IEvent> selectionEvents = new ArrayList<IEvent>();
			
			for (EventLogEntryReference selectionElement : selectionElements)
				selectionEvents.add(eventLog.getEventForEventNumber(selectionElement.getEventNumber()));
	
			return new EventLogSelection(eventLogInput, selectionEvents);
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
		// store current settings
		if (eventLogInput != null) {
			eventLogInput.removeEventLogChangedListener(this);
			storeState(eventLogInput.getFile());
		}

		// remember input
		eventLogInput = (EventLogInput)input;
		eventLog = eventLogInput == null ? null : eventLogInput.getEventLog();
		eventLogTableFacade = eventLogInput.getEventLogTableFacade();
		
		// clear state
		followEnd = false;

		super.setInput(input);

		// restore last known settings
		if (eventLogInput != null) {
			eventLogInput.addEventLogChangedListener(this);

			if (!restoreState(eventLogInput.getFile()))
				scrollToBegin();
		}
	}
	
	@Override
	protected void relocateFixPoint(EventLogEntryReference element, int distance) {
		Assert.isTrue(element == null || eventLog.getEventForEventNumber(element.getEventNumber()) != null);
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

	/*************************************************************************************
	 * MISC
	 */

	public EventLogInput getEventLogInput() {
		return eventLogInput;		
	}

	public IEventLog getEventLog() {
		return eventLog;
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return eventLogTableFacade;
	}

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getDisplayMode() {
		return eventLogTableFacade.getDisplayMode();
	}

	public void setDisplayMode(int i) {
		eventLogTableFacade.setDisplayMode(i);
	}

	public int getFilterMode() {
		return eventLogTableFacade.getFilterMode();
	}

	public void setFilterMode(int i) {
		eventLogTableFacade.setFilterMode(i);
		stayNear();
	}
	
	public String getCustomFilter() {
		return eventLogTableFacade.getCustomFilter();
	}

	public void setCustomFilter(String pattern) {
		eventLogTableFacade.setCustomFilter(pattern);
	}

	/*************************************************************************************
	 * EVENT LOG NOTIFICATIONS
	 */

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
			FilteredEventLog filteredEventLog = (FilteredEventLog)eventLog;
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

	/*************************************************************************************
	 * PERSISTING STATE
	 */

	public boolean restoreState(IResource resource) {
		try {
			String propertyValue = resource.getPersistentProperty(EVENT_LOG_TABLE_STATE_PROPERTY);
			EventLogTableState eventLogTableState = (EventLogTableState)Base64Serializer.deserialize(propertyValue, getClass().getClassLoader());

			if (eventLogTableState != null) {
				IEvent event = eventLog.getEventForEventNumber(eventLogTableState.topVisibleEventNumber);

				if (event != null) {
					setFilterMode(eventLogTableState.filterMode);
					setCustomFilter(eventLogTableState.customFilter);
					setDisplayMode(eventLogTableState.displayMode);

					gotoElement(new EventLogEntryReference(event.getEventEntry()));

					return true;
				}
			}

			return false;
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	public void storeState(IResource resource) {
		try {
			EventLogEntryReference eventLogEntryReference = getTopVisibleElement();
	
			if (eventLogEntryReference != null) {
				EventLogTableState eventLogTableState = new EventLogTableState();
				eventLogTableState.topVisibleEventNumber = eventLogEntryReference.getEventLogEntry(eventLogInput).getEvent().getEventNumber();
				eventLogTableState.filterMode = getFilterMode();
				eventLogTableState.customFilter = getCustomFilter();
				eventLogTableState.displayMode = getDisplayMode();
				
				resource.setPersistentProperty(EVENT_LOG_TABLE_STATE_PROPERTY, Base64Serializer.serialize(eventLogTableState));
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}
}

class EventLogTableState implements Serializable {
	private static final long serialVersionUID = 1L;
	public int topVisibleEventNumber;
	public int filterMode;
	public String customFilter;
	public int displayMode;
}
