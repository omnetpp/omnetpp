/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.widgets;

import java.io.Serializable;
import java.util.Iterator;

import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
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
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.EventLogEntryReference;
import org.omnetpp.common.eventlog.EventLogFindTextDialog;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogSelection;
import org.omnetpp.common.eventlog.EventNumberRangeSet;
import org.omnetpp.common.eventlog.IEventLogChangeListener;
import org.omnetpp.common.eventlog.IEventLogProvider;
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
    implements IEventLogChangeListener, IEventLogProvider
{
    private static final boolean debug = false;

    public static final String STATE_PROPERTY = "EventLogTableState";

    private boolean paintHasBeenFinished = false;

    private RuntimeException internalError;
    private boolean internalErrorHappenedDuringPaint = false;

    private boolean followEnd = false; // when the eventlog changes should we follow it or not?

    private EventLogInput eventLogInput;

    private IEventLog eventLog;

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
    protected void paint(final GC gc) {
        paintHasBeenFinished = false;

        if (internalErrorHappenedDuringPaint)
            drawNotificationMessage(gc, "Internal error happened during painting. Try to reset zoom, position, filter, etc. and press refresh. Sorry for your inconvenience.");
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
        else {
            try {
                eventLogInput.runWithProgressMonitor(new Runnable() {
                    public void run() {
                        try {
                            EventLogTable.super.paint(gc);
                            paintHasBeenFinished = true;
                        }
                        catch (RuntimeException e) {
                            if (eventLogInput.isFileChangedException(e))
                                eventLogInput.synchronize(e);
                            else
                                throw e;
                        }
                    }
                });
            }
            catch (RuntimeException e) {
                EventLogTablePlugin.logError("Internal error happened during painting", e);
                internalError = e;
                internalErrorHappenedDuringPaint = true;
            }
        }
    }

    @Override
    public void refresh() {
        internalError = null;
        internalErrorHappenedDuringPaint = false;
        if (eventLogInput != null)
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
                int lastEventEntryIndex = eventLogSelection.getEventLog().getEventForEventNumber(eventNumberRange.getLast()).getNumEventLogEntries() - 1;
                selectionElements.addRange(new EventLogEntryReference(eventNumberRange.getFirst(), 0), new EventLogEntryReference(eventNumberRange.getLast(), lastEventEntryIndex));
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
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    eventLogChanged();
                }
                catch (RuntimeException x) {
                    if (eventLogInput.isFileChangedException(x))
                        eventLogInput.synchronize(x);
                    else
                        throw x;
                }
            }
        });
    }

    public void eventLogOverwritten() {
        Display.getCurrent().asyncExec(new Runnable() {
            public void run() {
                try {
                    eventLogChanged();
                }
                catch (RuntimeException x) {
                    if (eventLogInput.isFileChangedException(x))
                        eventLogInput.synchronize(x);
                    else
                        throw x;
                }
            }
        });
    }

    private void eventLogChanged() {
        if (eventLog.isEmpty())
            fixPointElement = null;
        else if (followEnd) {
            if (debug)
                Debug.println("Scrolling to follow eventlog change");
            scrollToEnd();
        }
        else if (fixPointElement == null || fixPointElement.getEvent(eventLog) == null)
            scrollToBegin();
        if (debug)
            Debug.println("EventLogTable got notification about eventlog change");
        configureVerticalScrollBar();
        updateVerticalBarPosition();
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

        // XXX avoid iteration over the elements of the selection
        Iterator<EventLogEntryReference> iterator = selectionElements.iterator();
        while (iterator.hasNext())
            if (eventLog.getEventForEventNumber(iterator.next().getEventNumber()) == null)
                iterator.remove();

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
        EventLogFindTextDialog findTextDialog = eventLogInput.getFindTextDialog();

        if (continueSearch || findTextDialog.open() == Window.OK) {
            String findText = findTextDialog.getValue();

            if (findText != null) {
                EventLogEntryReference eventLogEntryReference = getFocusElement();
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
