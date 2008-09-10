package org.omnetpp.eventlogtable.widgets;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogFindTextDialog;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogEntry;
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
	
	private boolean paintHasBeenFinished = false;
	
	private boolean internalErrorHappenedDuringPaint = false;

	private boolean followEnd = false; // when the eventlog changes should we follow it or not?

	private EventLogInput eventLogInput;
	
	private IEventLog eventLog;

	private EventLogTableFacade eventLogTableFacade;
	
	private EventLogTableContributor eventLogTableContributor;

    public enum TypeMode {
        CPP,
        NED
    }

	public enum NameMode {
	    SMART_NAME,
	    FULL_NAME,
	    FULL_PATH
	}

	public enum DisplayMode {
	    DESCRIPTIVE,
	    RAW
	}
	
	private TypeMode typeMode = TypeMode.CPP;
	
	private NameMode nameMode = NameMode.SMART_NAME;
	
	private DisplayMode displayMode = DisplayMode.DESCRIPTIVE;

	/*************************************************************************************
	 * CONSTRUCTION
	 */

	public EventLogTable(Composite parent, int style) {
		super(parent, style);

		setContentProvider(new EventLogTableContentProvider());
		setRowRenderer(new EventLogTableRowRenderer(this));

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
	    this.eventLogTableContributor = eventLogTableContributor;
		MenuManager menuManager = new MenuManager();
		eventLogTableContributor.contributeToPopupMenu(menuManager);
		setMenu(menuManager.createContextMenu(this));
	}

	/*************************************************************************************
	 * OVERRIDING BEHAVIOR
	 */

    @Override
    protected void keyUpPressed(KeyEvent e) {
        if (e.stateMask == 0)
            super.keyUpPressed(e);
        else if (e.stateMask == SWT.MOD1)
            eventLogTableContributor.gotoEventCause();
        else if (e.stateMask == SWT.SHIFT)
            eventLogTableContributor.gotoPreviousModuleEvent();
        else if (e.stateMask == SWT.ALT)
            eventLogTableContributor.gotoPreviousEvent();
    }

    @Override
    protected void keyDownPressed(KeyEvent e) {
        if (e.stateMask == 0)
            super.keyDownPressed(e);
        else if (e.stateMask == SWT.MOD1)
            eventLogTableContributor.gotoMessageArrival();
        else if (e.stateMask == SWT.SHIFT)
            eventLogTableContributor.gotoNextModuleEvent();
        else if (e.stateMask == SWT.ALT)
            eventLogTableContributor.gotoNextEvent();
    }

	@Override
	protected void paint(final GC gc)
	{
	    paintHasBeenFinished = false;
	    
        if (internalErrorHappenedDuringPaint)
            drawNotificationMessage(gc, "Internal error happend during painting. Try to reset zoom, position, filter, etc. and press refresh. Sorry for your inconvenience.");
        else if (eventLogInput == null) {
			super.paint(gc);
			paintHasBeenFinished = true;
		}
		else if (eventLogInput.isCanceled())
		    drawNotificationMessage(gc,
		        "Processing of a long running eventlog operation was cancelled, therefore the chart is incomplete and cannot be drawn.\n" +
		        "Either try changing some filter parameters or select refresh from the menu. Sorry for your inconvenience.");
		else if (eventLogInput.isLongRunningOperationInProgress())
		    drawNotificationMessage(gc, "Processing a long running eventlog operation. Please wait.");
		else
			eventLogInput.runWithProgressMonitor(new Runnable() {
				public void run() {
				    try {
    					EventLogTable.super.paint(gc);
    					paintHasBeenFinished = true;
				    }
                    catch (RuntimeException e) {
                        if (eventLogInput.isEventLogChangedException(e))
                            eventLogInput.checkEventLogForChanges();
                        else {
                            internalErrorHappenedDuringPaint = true;
                            throw e;
                        }
                    }
				}
			});
	}
	
	@Override
	public void refresh() {
        internalErrorHappenedDuringPaint = false;
		eventLogInput.resetCanceled();
		super.refresh();
	}

	protected void drawNotificationMessage(GC gc, String text) {
        String[] lines = text.split("\n");
        gc.setForeground(ColorFactory.RED4);
        gc.setBackground(ColorFactory.WHITE);
        gc.setFont(JFaceResources.getDefaultFont());

        Point p = getSize();
        int x = p.x / 2;
        int y = p.y / 2;

        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];
            p = gc.textExtent(line);
            gc.drawString(line, x - p.x / 2, y - (lines.length / 2 - i) * p.y);
        }
    }

	@Override
	public ISelection getSelection() {
		if (eventLogInput == null)
			return null;
		else {
			List<EventLogEntryReference> selectionElements = getSelectionElements();
			ArrayList<Long> selectionEvents = new ArrayList<Long>();

			if (selectionElements != null)
    			for (EventLogEntryReference selectionElement : selectionElements)
    				selectionEvents.add(selectionElement.getEventNumber());
	
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

		IEventLog eventLog = eventLogSelection.getEventLogInput().getEventLog();
		for (Long eventNumber : eventLogSelection.getEventNumbers())
			eventLogEntries.add(new EventLogEntryReference(eventLog.getEventForEventNumber(eventNumber).getEventEntry()));

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

	public int getLineFilterMode() {
		return eventLogTableFacade.getFilterMode();
	}

	public void setLineFilterMode(int i) {
		eventLogTableFacade.setFilterMode(i);
		stayNear();
	}
	
    public String getCustomFilter() {
        return eventLogTableFacade.getCustomFilter();
    }

    public void setCustomFilter(String pattern) {
        eventLogTableFacade.setCustomFilter(pattern);
    }

    public TypeMode getTypeMode() {
        return typeMode;
    }

    public void setTypeMode(TypeMode typeMode) {
        if (typeMode == null)
            throw new IllegalArgumentException();
        this.typeMode = typeMode;
        redraw();
    }
    
    public NameMode getNameMode() {
        return nameMode;
    }

    public void setNameMode(NameMode nameMode) {
        if (nameMode == null)
            throw new IllegalArgumentException();
        this.nameMode = nameMode;
        redraw();
    }
    
    public DisplayMode getDisplayMode() {
        return displayMode;
    }

    public void setDisplayMode(DisplayMode displayMode) {
        if (displayMode == null)
            throw new IllegalArgumentException();
        this.displayMode = displayMode;
        redraw();
    }

	/*************************************************************************************
	 * EVENTLOG NOTIFICATIONS
	 */

	public void eventLogAppended() {
		eventLogChanged();
	}

    public void eventLogOverwritten() {
        eventLogChanged();
    }

    private void eventLogChanged() {
        if (eventLog.isEmpty())
            fixPointElement = null;
        else if (fixPointElement == null || fixPointElement.getEvent(eventLog) == null)
            scrollToBegin();

        if (debug)
			System.out.println("EventLogTable got notification about eventlog change");

        configureVerticalScrollBar();
		updateVerticalBarPosition();

		if (followEnd)
		{
			if (debug)
				System.out.println("Scrolling to follow eventlog change");

			if (!eventLog.isEmpty())
			    scrollToEnd();
		}
		else
			redraw();
    }
	
	public void eventLogFiltered() {
		eventLog = eventLogInput.getEventLog();

		if (eventLog.isEmpty())
		    fixPointElement = null;
		else if (fixPointElement != null) {
			FilteredEventLog filteredEventLog = (FilteredEventLog)eventLog;
			IEvent closestEvent = filteredEventLog.getMatchingEventInDirection(fixPointElement.getEventNumber(), false);
			
			if (closestEvent != null)
				relocateFixPoint(new EventLogEntryReference(closestEvent.getEventEntry()), 0);
			else {
				closestEvent = filteredEventLog.getMatchingEventInDirection(fixPointElement.getEventNumber(), true);
	
				if (closestEvent != null)
					relocateFixPoint(new EventLogEntryReference(closestEvent.getEventEntry()), 0);
				else
				    scrollToBegin();
			}
		}
		else
		    scrollToBegin();

        for (EventLogEntryReference eventLogEntryReference : new ArrayList<EventLogEntryReference>(selectionElements))
            if (eventLog.getEventForEventNumber(eventLogEntryReference.getEventNumber()) == null)
                selectionElements.remove(eventLogEntryReference);
		
        eventLogTableContributor.update();
		redraw();
	}
	
	public void eventLogFilterRemoved() {
		eventLog = eventLogInput.getEventLog();
        eventLogTableContributor.update();
		redraw();
	}

	public void eventLogLongOperationStarted() {
	    // void
	}

	public void eventLogLongOperationEnded() {
	    if (!paintHasBeenFinished)
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
					setLineFilterMode(eventLogTableState.lineFilterMode);
					setCustomFilter(eventLogTableState.customFilter);
                    setTypeMode(eventLogTableState.typeMode);
                    setNameMode(eventLogTableState.nameMode);
					setDisplayMode(eventLogTableState.displayMode);

					scrollToElement(new EventLogEntryReference(event.getEventEntry()));

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
				eventLogTableState.lineFilterMode = getLineFilterMode();
				eventLogTableState.customFilter = getCustomFilter();
                eventLogTableState.typeMode = getTypeMode();
                eventLogTableState.nameMode = getNameMode();
				eventLogTableState.displayMode = getDisplayMode();
				
				manager.setProperty(resource, STATE_PROPERTY, eventLogTableState);
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	public void findText(boolean continueSearch) {
	    EventLogFindTextDialog findTextDialog = eventLogInput.getFindTextDialog();

	    if (continueSearch || findTextDialog.open() == Window.OK) {
            String findText = findTextDialog.getValue();
            
            if (findText != null) {
                EventLogEntryReference eventLogEntryReference = getSelectionElement();
                EventLogEntry startEventLogEntry = (eventLogEntryReference == null ? getTopVisibleElement() : eventLogEntryReference).getEventLogEntry(eventLogInput);
                EventLogEntry foundEventLogEntry = eventLog.findEventLogEntry(startEventLogEntry, findText, !findTextDialog.isBackward(), !findTextDialog.isCaseInsensitive());
    
                if (foundEventLogEntry != null)
                    gotoClosestElement(new EventLogEntryReference(foundEventLogEntry));
                else
                    MessageDialog.openInformation(null, "Find raw text", "No more matches found for " + findText);
            }
        }
    }
}

/**
 * Used to persistently store the eventlog table settings.
 */
class EventLogTableState implements Serializable {
	private static final long serialVersionUID = 1L;
	public long topVisibleEventNumber;
	public int lineFilterMode;
	public String customFilter;
    public EventLogTable.TypeMode typeMode;
    public EventLogTable.NameMode nameMode;
	public EventLogTable.DisplayMode displayMode;
}
