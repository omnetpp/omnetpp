package org.omnetpp.eventlogtable.widgets;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.EventLogTablePlugin;
import org.omnetpp.eventlogtable.editors.EventLogTableContributor;

public class EventLogTable
	extends VirtualTable<EventLogEntryReference>
	implements IEventLogChangeListener
{
	private static final boolean debug = true;

	public static final String STATE_PROPERTY = "EventLogTableState";
	
	private boolean normalPaintHasBeenRun = false;

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
	protected void paint(final GC gc)
	{
	    normalPaintHasBeenRun = false;
	    
		if (eventLogInput == null) {
			super.paint(gc);
			normalPaintHasBeenRun = true;
		}
		else if (eventLogInput.isCanceled())
			drawCancelMessage(gc);
		else if (eventLogInput.isLongRunningOperationInProgress())
			drawLongRunningOperationInProgressMessage(gc);
		else
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {
				    try {
    					EventLogTable.super.paint(gc);
    					normalPaintHasBeenRun = true;
				    }
                    catch (RuntimeException e) {
                        if (eventLogInput.isEventLogChangedException(e))
                            eventLogInput.checkEventLogForChanges();
                        else
                            throw e;
                    }
				}
			});
	}
	
	@Override
	public void refresh() {
		eventLogInput.resetCanceled();
		super.refresh();
	}

	protected void drawCancelMessage(GC gc) {
		gc.setForeground(ColorFactory.RED4);
		gc.setBackground(ColorFactory.WHITE);
		gc.setFont(JFaceResources.getDefaultFont());
		Point p = getSize();
		int x = p.x/ 2;
		int y = p.y / 2;
		String text = "Processing of a long running event log operation was cancelled, therefore the chart is incomplete and cannot be drawn.";
		p = gc.textExtent(text);
		gc.drawString(text, x - p.x / 2, y - p.y);
		text = "Either try changing some filter parameters or select refresh from the menu. Sorry for your inconvenience.";
		p = gc.textExtent(text);
		gc.drawString(text, x - p.x / 2, y);
	}

	protected void drawLongRunningOperationInProgressMessage(GC gc) {
		gc.setForeground(ColorFactory.RED4);
		gc.setBackground(ColorFactory.WHITE);
		gc.setFont(JFaceResources.getDefaultFont());
		Point p = getSize();
		int x = p.x/ 2;
		int y = p.y / 2;
		String text = "Processing a long running event log operation. Please wait.";
		p = gc.textExtent(text);
		gc.drawString(text, x - p.x / 2, y - p.y / 2);
	}

	@Override
	public ISelection getSelection() {
		if (eventLogInput == null)
			return null;
		else {
			List<EventLogEntryReference> selectionElements = getSelectionElements();
			ArrayList<IEvent> selectionEvents = new ArrayList<IEvent>();

			if (selectionElements != null)
    			for (EventLogEntryReference selectionElement : selectionElements)
    				selectionEvents.add(eventLog.getEventForEventNumber(selectionElement.getEventNumber()));
	
			return new EventLogSelection(eventLogInput, selectionEvents);
		}
	}

	/**
	 * Accepts only IEventLogSelections.
	 */
	@Override
	public void setSelection(ISelection selection) {
		IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
		List<EventLogEntryReference> eventLogEntries = new ArrayList<EventLogEntryReference>();

		for (IEvent event : eventLogSelection.getEvents())
			eventLogEntries.add(new EventLogEntryReference(event.getEventEntry()));

		super.setSelection(new VirtualTableSelection<EventLogEntryReference>(eventLogSelection.getEventLogInput(), eventLogEntries));
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
		eventLogTableFacade = eventLogInput == null ? null : eventLogInput.getEventLogTableFacade();
		
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
	 * GETTERS & SETTERS
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
        if (!eventLog.isEmpty() && fixPointElement == null)
            scrollToBegin();

		eventLogChanged();
	}

    public void eventLogOverwritten() {
        scrollToBegin();
        eventLogChanged();
    }

    private void eventLogChanged() {
        if (debug)
			System.out.println("EventLogTable got notification about event log change");

        configureVerticalScrollBar();
		updateVerticalBarPosition();

		if (followEnd)
		{
			if (debug)
				System.out.println("Scrolling to follow event log change");

			if (!eventLog.isEmpty())
			    scrollToEnd();
		}
		else
			redraw();
    }
	
	public void eventLogFiltered() {
		eventLog = eventLogInput.getEventLog();

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
	
	public void eventLogFilterRemoved() {
		eventLog = eventLogInput.getEventLog();
		redraw();
	}

	public void eventLogLongOperationStarted() {
	}

	public void eventLogLongOperationEnded() {
	    if (!normalPaintHasBeenRun)
	        canvas.redraw();
	}

	public void eventLogProgress() {
		if (eventLogInput.getEventLogProgressManager().isCanceled())
			canvas.redraw();
	}

	/*************************************************************************************
	 * PERSISTING STATE
	 */

	public boolean restoreState(IResource resource) {
		PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(EventLogTablePlugin.PLUGIN_ID, getClass().getClassLoader());

		try {
			if (manager.hasProperty(resource, STATE_PROPERTY)) {
				EventLogTableState eventLogTableState = (EventLogTableState)manager.getProperty(resource, STATE_PROPERTY);
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
			manager.removeProperty(resource, STATE_PROPERTY);

			throw new RuntimeException(e);
		}
	}

	public void storeState(IResource resource) {
		try {
			PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(EventLogTablePlugin.PLUGIN_ID, getClass().getClassLoader());
			EventLogEntryReference eventLogEntryReference = getTopVisibleElement();
	
			if (eventLogEntryReference == null)
				manager.removeProperty(resource, STATE_PROPERTY);
			else {
				EventLogTableState eventLogTableState = new EventLogTableState();
				eventLogTableState.topVisibleEventNumber = eventLogEntryReference.getEventLogEntry(eventLogInput).getEvent().getEventNumber();
				eventLogTableState.filterMode = getFilterMode();
				eventLogTableState.customFilter = getCustomFilter();
				eventLogTableState.displayMode = getDisplayMode();
				
				manager.setProperty(resource, STATE_PROPERTY, eventLogTableState);
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
