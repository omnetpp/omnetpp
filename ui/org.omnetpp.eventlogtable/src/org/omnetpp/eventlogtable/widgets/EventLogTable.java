/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.widgets;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;

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
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.Debug;
import org.omnetpp.common.collections.IEnumerator;
import org.omnetpp.common.collections.IRangeSet;
import org.omnetpp.common.collections.Range;
import org.omnetpp.common.collections.RangeSet;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogFindTextDialog;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.EventNumberRangeSet;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogProvider;
import org.omnetpp.common.eventlog.IEventLogSelection;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.EventLogEntry;
import org.omnetpp.eventlog.EventLogTableFacade;
import org.omnetpp.eventlog.EventLogTableFilterMode;
import org.omnetpp.eventlog.FilteredEventLog;
import org.omnetpp.eventlog.IEvent;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlogtable.EventLogTablePlugin;
import org.omnetpp.eventlogtable.editors.EventLogTableContributor;

public class EventLogTable
    extends VirtualTable<EventLogEntryReference>
    implements IEventLogChangeListener, IEventLogProvider
{
    private static final boolean debug = false;

    public static final String STATE_PROPERTY = "EventLogTableState";

    private boolean isOutOfSync = false; // the underlying eventlog has been changed during the last operation
    private boolean isPaintComplete = false; // the last paint was successfully completed or not
    private boolean followEnd = false; // when the eventlog changes should we follow it or not?
    private RuntimeException internalError;

    private EventLogInput eventLogInput; // the Java input object
    private IEventLog eventLog; // the C++ wrapper for the data to be displayed
    private EventLogTableFacade eventLogTableFacade;
    private EventLogTableContributor eventLogTableContributor;

    private static class EventLogEntryReferenceEnumerator implements IEnumerator<EventLogEntryReference> {
        private EventLogTableContentProvider contentProvider;

        public EventLogEntryReferenceEnumerator(EventLogTableContentProvider contentProvider) {
            this.contentProvider = contentProvider;
        }

        @Override
        public int compare(EventLogEntryReference element1, EventLogEntryReference element2) {
            return contentProvider.compare(element1, element2);
        }

        @Override
        public EventLogEntryReference getPrevious(EventLogEntryReference element) {
            return contentProvider.getNeighbourElement(element, -1);
        }

        @Override
        public EventLogEntryReference getNext(EventLogEntryReference element) {
            return contentProvider.getNeighbourElement(element, 1);
        }

        @Override
        public int getApproximateDelta(EventLogEntryReference element1, EventLogEntryReference element2) {
            if (compare(element1, element2) <= 0) {
                long distance =  contentProvider.getDistanceToElement(element1, element2, 100);
                if (distance < 100)
                    return (int)distance;
            }
            else {
                long distance =  contentProvider.getDistanceToElement(element1, element2, -100);
                if (distance > -100)
                    return (int)distance;
            }
            long numOfElements = contentProvider.getApproximateNumberOfElements();
            double percentage1 = contentProvider.getApproximatePercentageForElement(element1);
            double percentage2 = contentProvider.getApproximatePercentageForElement(element2);
            return (int)(numOfElements * (percentage2 - percentage1));
        }

        public boolean isExact() { return false; }
    }

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

    private IWorkbenchPart workbenchPart;

    /*************************************************************************************
     * CONSTRUCTION
     */

    public EventLogTable(Composite parent, int style) {
        super(parent, style);

        EventLogTableContentProvider contentProvider = new EventLogTableContentProvider();
        setContentProvider(contentProvider);
        setRowRenderer(new EventLogTableRowRenderer(this));

        rowEnumerator = new EventLogEntryReferenceEnumerator(contentProvider);

        TableColumn tableColumn = createColumn();
        tableColumn.setWidth(120);
        tableColumn.setText("Event #");

        tableColumn = createColumn();
        tableColumn.setWidth(200);
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

    public EventLogTableContributor getEventLogTableContributor() {
        return eventLogTableContributor;
    }

    public void setEventLogTableContributor(EventLogTableContributor eventLogTableContributor) {
        this.eventLogTableContributor = eventLogTableContributor;
        MenuManager menuManager = new MenuManager();
        eventLogTableContributor.contributeToPopupMenu(menuManager);
        setMenu(menuManager.createContextMenu(this));
    }

    public IWorkbenchPart getWorkbenchPart() {
        return workbenchPart;
    }

    public void setWorkbenchPart(IWorkbenchPart workbenchPart) {
        this.workbenchPart = workbenchPart;
    }

    /*************************************************************************************
     * OVERRIDING BEHAVIOR
     */

    @Override
    public void handleRuntimeException(RuntimeException e) {
        if (eventLogInput != null)
            eventLogInput.handleRuntimeException(e);
        else
            throw e;
    }

    @Override
    protected void paint(final GC gc) {
        isPaintComplete = false;
        if (internalError != null)
            drawNotificationMessage(gc, "Internal error - please tweak the settings and refresh",
                "Try changing the zoom factor, scroll position, display modes, eventlog filter parameters, etc.\nto prevent the error from happening again, then press Refresh.\nWe are sorry for the inconvenience.");
        else if (eventLogInput == null) {
            super.paint(gc);
            isPaintComplete = true;
        }
        else if (isOutOfSync)
            drawNotificationMessage(gc, "The eventlog is continually changing - please wait",
                "This is most often caused by a simulation running in the background.\nThe component will automatically refresh in a few seconds.");
        else if (eventLogInput.isCanceled())
            drawNotificationMessage(gc, "Operation cancelled - please tweak the settings and refresh",
                "A long-running eventlog operation has been cancelled.\nTo continue, try changing the zoom factor, scroll position, display modes, eventlog filter parameters, etc.\nto speed up the operation, then press Refresh.");
        else if (eventLogInput.isLongRunningOperationInProgress())
            drawNotificationMessage(gc, "Processing a long-running eventlog operation - please wait",
                "The operation is taking long because of using the eventlog filter, showing too much content, or some other reasons.\nThe component will automatically refresh when the operation completes.");
        else {
            try {
                isPaintComplete = false;
                if (eventLogInput == null) {
                    super.paint(gc);
                    isPaintComplete = true;
                }
                else {
                    eventLogInput.runWithProgressMonitor(new Runnable() {
                        public void run() {
                            try {
                                EventLogTable.super.paint(gc);
                                isPaintComplete = true;
                            }
                            catch (RuntimeException e) {
                                if (eventLogInput.isFileChangedException(e))
                                    eventLogInput.handleRuntimeException(e);
                                else
                                    throw e;
                            }
                        }
                    });
                }
            }
            catch (RuntimeException e) {
                EventLogTablePlugin.logError("Internal error happened during painting", e);
                internalError = e;
            }
        }
    }

    @Override
    public void refresh() {
        internalError = null;
        if (eventLogInput != null)
            eventLogInput.resetCanceled();
        super.refresh();
    }

    /**
     * Draws a notification message to the center of the viewport.
     */
    protected void drawNotificationMessage(GC gc, String title, String text) {
        Point p = getSize();
        int x = p.x / 2;
        int y = p.y / 2;
        String[] lines = text.split("\n");
        gc.setForeground(DisplayUtils.getForegroundColor());
        gc.setFont(JFaceResources.getHeaderFont());
        p = gc.textExtent(title);
        gc.drawText(title, x - p.x / 2, y - (lines.length / 2 + 2) * p.y);
        gc.setFont(JFaceResources.getDefaultFont());
        for (int i = 0; i < lines.length; i++) {
            String line = lines[i];
            p = gc.textExtent(line);
            gc.drawText(line, x - p.x / 2, y - (lines.length / 2 - i) * p.y);
        }
    }

    @Override
    public ISelection getSelection() {
        if (eventLogInput == null)
            return null;
        else {
            IRangeSet<EventLogEntryReference> selectionElements = getSelectionElements();
            EventNumberRangeSet selectionEvents = new EventNumberRangeSet();
            if (selectionElements != null) {
                Iterator<Range<EventLogEntryReference>> rangeIterator = selectionElements.rangeIterator();
                while (rangeIterator.hasNext()) {
                    Range<EventLogEntryReference> range = rangeIterator.next();
                    selectionEvents.addRange(range.getFirst().getEventNumber(), range.getLast().getEventNumber());
                }
            }
            return new EventLogSelection(eventLogInput, selectionEvents, null);
        }
    }

    /**
     * Accepts only IEventLogSelections.
     */
    @Override
    public void setSelection(ISelection selection) {
        if (selection instanceof IEventLogSelection) {
            IEventLogSelection eventLogSelection = (IEventLogSelection)selection;
            EventLogTableContentProvider selectionContentProvider = new EventLogTableContentProvider();
            selectionContentProvider.inputChanged(null, null, eventLogSelection.getEventLogInput());
            RangeSet<EventLogEntryReference> selectionElements = new RangeSet<EventLogEntryReference>(new EventLogEntryReferenceEnumerator(selectionContentProvider));
            EventNumberRangeSet selectedEventNumbers = eventLogSelection.getEventNumbers();
            Iterator<Range<Long>> iterator = selectedEventNumbers.rangeIterator();
            while (iterator.hasNext()) {
                Range<Long> eventNumberRange = iterator.next();
                IEvent event = eventLogSelection.getEventLog().getEventForEventNumber(eventNumberRange.getLast());
                if (event != null) {
                    int lastEventEntryIndex = event.getNumEventLogEntries() - 1;
                    selectionElements.addRange(new EventLogEntryReference(eventNumberRange.getFirst(), 0), new EventLogEntryReference(eventNumberRange.getLast(), lastEventEntryIndex));
                }
            }
            super.setSelection(new VirtualTableSelection<EventLogEntryReference>(eventLogSelection.getEventLogInput(), selectionElements));
        }
    }

    @Override
    public void setInput(Object input) {
        if (input != eventLogInput) {
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
    public void reveal(EventLogEntryReference element) {
        super.reveal(element);
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

    public boolean hasInput() {
        return eventLogInput != null;   // input may be null, e.g. when editor is not a Sequence Chart
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

    public EventLogTableFilterMode getLineFilterMode() {
        return eventLogTableFacade.getFilterMode();
    }

    public void setLineFilterMode(EventLogTableFilterMode i) {
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

    @Override
    public void eventLogAppended() {
        eventLogChanged();
    }

    @Override
    public void eventLogOverwritten() {
        eventLogChanged();
    }

    private void eventLogChanged() {
        if (fixPointElement != null && fixPointElement.getEvent(eventLog) == null)
            fixPointElement = null;
        if (selectionElements != null)
            for (EventLogEntryReference selectionElement : new ArrayList<EventLogEntryReference>(selectionElements))
                if (eventLog.getEventForEventNumber(selectionElement.getEventNumber()) == null)
                    selectionElements.remove(selectionElement);
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    handleEventLogChanged();
                }
                catch (RuntimeException e) {
                    handleRuntimeException(e);
                }
            }
        });
    }

    private void handleEventLogChanged() {
        if (debug)
            Debug.println("EventLogTable got notification about eventlog change");
        if (eventLog.isEmpty())
            fixPointElement = null;
        else if (followEnd) {
            if (debug)
                Debug.println("Scrolling to follow eventlog change");
            scrollToEnd();
        }
        else if (fixPointElement == null || fixPointElement.getEvent(eventLog) == null)
            scrollToBegin();
        configureVerticalScrollBar();
        updateVerticalBarPosition();
        redraw();
        isOutOfSync = false;
    }

    @Override
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
        eventLogTableContributor.update();
        redraw();
    }

    @Override
    public void eventLogFilterRemoved() {
        eventLog = eventLogInput.getEventLog();
        eventLogTableContributor.update();
        redraw();
    }

    @Override
    public void eventLogLongOperationStarted() {
        // void
    }

    @Override
    public void eventLogLongOperationEnded() {
        if (!isPaintComplete)
            canvas.redraw();
    }

    @Override
    public void eventLogProgress() {
        if (eventLogInput.getEventLogProgressManager().isCanceled())
            canvas.redraw();
    }

    @Override
    public void eventLogSynchronizationFailed() {
        isOutOfSync = true;
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

                    reveal(new EventLogEntryReference(event.getEventEntry()));

                    return true;
                }
            }

            return false;
        }
        catch (Exception e) {
            manager.removeProperty(resource, STATE_PROPERTY);

            EventLogTablePlugin.logError(e);
            MessageDialog.openError(getShell(), "Error", "Could not restore saved event log table state, ignoring.");
            return false;
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
        final EventLogFindTextDialog findTextDialog = eventLogInput.getFindTextDialog();
        if (continueSearch || findTextDialog.open() == Window.OK) {
            final String findText = findTextDialog.getValue();
            if (findText != null) {
                EventLogEntryReference eventLogEntryReference = getFocusElement();
                final EventLogEntry startEventLogEntry = (eventLogEntryReference == null ? getTopVisibleElement() : eventLogEntryReference).getEventLogEntry(eventLogInput);
                final EventLogEntry[] foundEventLogEntries = new EventLogEntry[1];
                final boolean[] completed = new boolean[1];
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        foundEventLogEntries[0] = eventLog.findEventLogEntry(startEventLogEntry, findText, !findTextDialog.isBackward(), !findTextDialog.isCaseInsensitive());
                        completed[0] = true;
                    }
                });
                if (completed[0]) {
                    if (foundEventLogEntries[0] != null)
                        gotoClosestElement(new EventLogEntryReference(foundEventLogEntries[0]));
                    else
                        MessageDialog.openInformation(null, "Find raw text", "No more matches found for " + findText);
                }
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
    public EventLogTableFilterMode lineFilterMode;
    public String customFilter;
    public EventLogTable.TypeMode typeMode;
    public EventLogTable.NameMode nameMode;
    public EventLogTable.DisplayMode displayMode;
}
