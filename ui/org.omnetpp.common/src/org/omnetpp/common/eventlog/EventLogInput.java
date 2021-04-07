/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.RecurringJob;
import org.omnetpp.eventlog.EventLogTableFacade;
import org.omnetpp.eventlog.FilteredEventLog;
import org.omnetpp.eventlog.IEventLog;
import org.omnetpp.eventlog.SequenceChartFacade;
import org.omnetpp.eventlog.TimelineMode;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;

/**
 * Input object for event log file editors and viewers.
 */
public class EventLogInput extends FileEditorInput
    implements IEventLogProgressMonitor
{
    private static final boolean debug = false;

    public static final String STATE_PROPERTY = "EventLogInputState";

    /**
     * The C++ wrapper around the event log reader.
     */
    protected IEventLog eventLog;

    /**
     * Manages long-running operations for the event log.
     */
    protected EventLogProgressManager eventLogProgressManager;

    /**
     * The filter parameters applied to the event log last time.
     */
    protected EventLogFilterParameters eventLogFilterParameters;

    /**
     * A C++ wrapper around a helpful facade.
     */
    protected EventLogTableFacade eventLogTableFacade;

    /**
     * A C++ wrapper around a helpful facade.
     */
    protected SequenceChartFacade sequenceChartFacade;

    /**
     * Root of the module tree present in the event log file.
     */
    protected ModuleTreeItem moduleTreeRoot;

    /**
     * A list of listeners to be notified when the contents of the event log changes.
     * The standard eclipse IResourceChangeListener interface is not sufficient since
     * it relies on the last modification date which is not updated when the file is
     * being written without actually closing it.
     */
    protected ArrayList<IEventLogChangeListener> eventLogChangeListeners = new ArrayList<IEventLogChangeListener>();

    /**
     * Find text dialog along with the last parameters.
     */
    protected EventLogFindTextDialog findTextDialog = new EventLogFindTextDialog(null);

    /**
     * Watches the event log file for changes.
     */
    protected RecurringJob eventLogWatcher;

    /**
     * True indicates a long-running operation was canceled by the user.
     */
    private boolean canceled;

    /**
     * True means a long-running operation is in progress and the progress dialog is already shown.
     */
    private boolean longRunningOperationInProgress;

    /**
     * True means eventlog synchronization (due to file changes) is in progress.
     */
    private boolean synchronizing;

    /*************************************************************************************
     * CONSTRUCTION
     */

    public EventLogInput(IFile file, IEventLog eventLog) {
        super(file);
        this.eventLogProgressManager = new EventLogProgressManager();
        this.eventLogWatcher = new RecurringJob(3000) {
            public void run() {
                Display.getDefault().asyncExec(new Runnable() {
                    // synchronize may destructively modify the underlying structure of the event log
                    // and thus it must be called from the UI thread to prevent concurrent paints
                    public void run() {
                        try {
                            FileReader fileReader = getEventLog().getFileReader();
                            if (fileReader.isFileOpen())
                                synchronize(fileReader.getFileChange());
                        }
                        catch (RuntimeException e) {
                            if (isFileChangedException(e))
                                eventLogSynchronizationFailed();
                            else
                                throw e;
                        }
                    }
                });
            }
        };

        setEventLog(eventLog);
        restoreState();
    }

    public void synchronize(Exception e) {
        // TODO: XXX: FIXME: get change and call synchronize with that
        if (e.getMessage().contains("appended"))
            synchronize(FileReader.FileChange.APPENDED);
        else
            synchronize(FileReader.FileChange.OVERWRITTEN);
    }

    public void synchronize(int change) {
        Assert.isTrue(Display.getCurrent() != null);
        if (change != FileReader.FileChange.UNCHANGED) {
            if (debug)
                Debug.println("Synchronizing event log file content: " + getFile().getName() + " with change: " + change);
            doSynchronize(change);
            eventLogChanged(change);
        }
    }

    private void doSynchronize(int change) {
        try {
            // TODO: this was Assert.isTrue(Display.getCurrent() != null);
            // TODO: but it is called from a job during dispose()
            synchronizing = true;
            eventLog.synchronize(change);
            getEventLogTableFacade().synchronize(change);
            getSequenceChartFacade().synchronize(change);
        }
        catch (RuntimeException e) {
            // ignore file changed errors during synchronize
            // because the poller will kick in anyway
            if (isFileChangedException(e))
                eventLogSynchronizationFailed();
            else
                throw e;
        }
        finally {
            synchronizing = false;
        }
    }

    public void dispose() {
        if (eventLogWatcher != null)
            eventLogWatcher.stop();
        if (eventLog != null) {
            // NOTE: explicitly release memory (we might have allocated a lot)
            // we don't rely on when the garbage collector will kick in
            // TODO: at least we could do this in a background thread to avoid unresponsive ui
            // TODO: except that we must be calling the eventlog thread safe
//            new Job("Releasing eventlog") {
//                @Override
//                protected IStatus run(IProgressMonitor monitor) {
                    doSynchronize(FileReader.FileChange.OVERWRITTEN);
                    eventLog.getFileReader().ensureFileClosed();
//                    return new Status(IStatus.OK, CommonPlugin.PLUGIN_ID, null);
//                }
//            }.schedule();
        }
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();

        eventLogChangeListeners.clear();

        if (eventLogWatcher != null)
            eventLogWatcher.stop();
    }

    /*************************************************************************************
     * GETTERS
     */

    private void setEventLog(IEventLog eventLog) {
        this.eventLog = eventLog;
    }

    public IEventLog getEventLog() {
        return eventLog;
    }

    public TimelineMode getTimelineMode() {
        return sequenceChartFacade.getTimelineMode();
    }

    public void setTimelineMode(TimelineMode timelineMode) {
        sequenceChartFacade.setTimelineMode(timelineMode);
    }

    public EventLogFindTextDialog getFindTextDialog() {
        return findTextDialog;
    }

    public EventLogProgressManager getEventLogProgressManager() {
        return eventLogProgressManager;
    }

    public EventLogFilterParameters getFilterParameters() {
        if (eventLogFilterParameters == null)
            eventLogFilterParameters = new EventLogFilterParameters(this);

        return eventLogFilterParameters;
    }

    public EventLogTableFacade getEventLogTableFacade() {
        if (eventLogTableFacade == null)
            eventLogTableFacade = new EventLogTableFacade(eventLog);
        return eventLogTableFacade;
    }

    public SequenceChartFacade getSequenceChartFacade() {
        if (sequenceChartFacade == null)
            sequenceChartFacade = new SequenceChartFacade(eventLog);
        return sequenceChartFacade;
    }

    /**
     * Returns the module tree which at least contains all modules created during initialization.
     * Dynamically created modules might be added later as they get discovered.
     */
    public ModuleTreeItem getModuleTreeRoot() {
        if (moduleTreeRoot == null)
            synchronizeModuleTree();

        return moduleTreeRoot;
    }

    /**
     * Updates the module tree according to the current state of the eventlog.
     * The C++ implementation lazily discovers dynamic modules as the corresponding
     * module creation entries are read from the file.
     */
    public void synchronizeModuleTree() {
        ArrayList<ModuleDescriptionEntry> moduleDescriptionEntryList = eventLog.getEventLogEntryCache().getModuleDescriptionEntries();

        for (int i = 0; i < moduleDescriptionEntryList.size(); i++) {
            ModuleDescriptionEntry entry = moduleDescriptionEntryList.get(i);

            if (entry != null) {
                if (entry.getParentModuleId() == -1) {
                    if (moduleTreeRoot == null)
                        moduleTreeRoot = new ModuleTreeItem(entry.getModuleId(), entry.getFullName(), entry.getNedTypeName(), entry.getModuleClassName(), null, entry.getCompoundModule());
                }
                else {
                    ModuleTreeItem moduleTreeItem = moduleTreeRoot.findDescendantModule(entry.getModuleId());

                    if (moduleTreeItem != null && moduleTreeItem.getModuleName().equals("<unknown>")) {
                        // FIXME: what about references to this tree item? are there any?
                        moduleTreeItem.remove();
                        moduleTreeItem = null;
                    }

                    if (moduleTreeItem == null)
                        moduleTreeRoot.addDescendantModule(entry.getParentModuleId(), entry.getModuleId(), entry.getNedTypeName(), entry.getModuleClassName(), entry.getFullName(), entry.getCompoundModule());
                }
            }
        }
    }

    /*************************************************************************************
     * FILTERING
     */

    public int openFilterDialog() {
        FilterEventLogDialog filterEventLogDialog = new FilterEventLogDialog(Display.getCurrent().getActiveShell(), this, getFilterParameters());
        return filterEventLogDialog.open();
    }

    public void removeFilter() {
        if (eventLog instanceof FilteredEventLog) {
            setEventLog(((FilteredEventLog)eventLog).getEventLog());
            getEventLogTableFacade().setEventLog(eventLog);
            getSequenceChartFacade().setEventLog(eventLog);
            eventLogFilterRemoved();
            storeState();
        }
    }

    public void filter() {
        // remove old filter
        if (eventLog instanceof FilteredEventLog)
            setEventLog(((FilteredEventLog)eventLog).getEventLog());

        // create new filter
        FilteredEventLog filteredEventLog = new FilteredEventLog(eventLog);

        // collection limits
        if (eventLogFilterParameters.enableCollectionLimits) {
            filteredEventLog.setCollectMessageReuses(eventLogFilterParameters.collectMessageReuses);
            filteredEventLog.setMaximumCauseDepth(eventLogFilterParameters.maximumDepthOfMessageDependencies);
            filteredEventLog.setMaximumConsequenceDepth(eventLogFilterParameters.maximumDepthOfMessageDependencies);
            filteredEventLog.setMaximumNumberOfCauses(eventLogFilterParameters.maximumNumberOfMessageDependencies);
            filteredEventLog.setMaximumNumberOfConsequences(eventLogFilterParameters.maximumNumberOfMessageDependencies);
            filteredEventLog.setMaximumCauseCollectionTime(eventLogFilterParameters.maximumMessageDependencyCollectionTime);
            filteredEventLog.setMaximumConsequenceCollectionTime(eventLogFilterParameters.maximumMessageDependencyCollectionTime);
        }

        // enable is handled in filter parameters
        filteredEventLog.setFirstConsideredEventNumber(eventLogFilterParameters.getFirstEventNumber());
        filteredEventLog.setLastConsideredEventNumber(eventLogFilterParameters.getLastEventNumber());
//        filteredEventLog.setExcludedEventNumbers(eventLogFilterParameters.getExcludedEventNumbers());

        filteredEventLog.setEnableModuleFilter(eventLogFilterParameters.enableModuleFilter);
        if (eventLogFilterParameters.enableModuleFilter) {
            if (eventLogFilterParameters.enableModuleExpressionFilter)
                filteredEventLog.setModuleExpression(eventLogFilterParameters.moduleFilterExpression);

//            if (eventLogFilterParameters.enableModuleIdFilter || eventLogFilterParameters.enableModuleNameFilter)
//                filteredEventLog.setModuleIds(eventLogFilterParameters.getModuleIds());
//
//            if (eventLogFilterParameters.enableModuleNEDTypeNameFilter)
//                filteredEventLog.setModuleNedTypeNames(eventLogFilterParameters.getModuleNEDTypeNames());
        }

        if (eventLogFilterParameters.enableTraceFilter) {
            filteredEventLog.setTracedEventNumber(eventLogFilterParameters.tracedEventNumber);
            filteredEventLog.setTraceCauses(eventLogFilterParameters.traceCauses);
            filteredEventLog.setTraceConsequences(eventLogFilterParameters.traceConsequences);
            filteredEventLog.setTraceMessageReuses(eventLogFilterParameters.traceMessageReuses);
            filteredEventLog.setTraceSelfMessages(eventLogFilterParameters.traceSelfMessages);
        }
        else
            filteredEventLog.setTracedEventNumber(-1);

        filteredEventLog.setEnableMessageFilter(eventLogFilterParameters.enableMessageFilter);
        if (eventLogFilterParameters.enableMessageFilter) {
            if (eventLogFilterParameters.enableMessageExpressionFilter)
                filteredEventLog.setMessageExpression(eventLogFilterParameters.messageFilterExpression);
//
//            if (eventLogFilterParameters.enableMessageClassNameFilter)
//                filteredEventLog.setMessageClassNames(eventLogFilterParameters.getMessageClassNames());
//
//            if (eventLogFilterParameters.enableMessageNameFilter)
//                filteredEventLog.setMessageNames(eventLogFilterParameters.getMessageNames());
//
//            if (eventLogFilterParameters.enableMessageIdFilter)
//                filteredEventLog.setMessageIds(eventLogFilterParameters.getSelectedMessageIds());
//
//            if (eventLogFilterParameters.enableMessageTreeIdFilter)
//                filteredEventLog.setMessageTreeIds(eventLogFilterParameters.getSelectedMessageTreeIds());
//
//            if (eventLogFilterParameters.enableMessageEncapsulationIdFilter)
//                filteredEventLog.setMessageEncapsulationIds(eventLogFilterParameters.getSelectedMessageEncapsulationIds());
//
//            if (eventLogFilterParameters.enableMessageEncapsulationTreeIdFilter)
//                filteredEventLog.setMessageEncapsulationTreeIds(eventLogFilterParameters.getSelectedMessageEcapsulationTreeIds());
        }

        // store event log
        setEventLog(filteredEventLog);
        getEventLogTableFacade().setEventLog(filteredEventLog);
        getSequenceChartFacade().setEventLog(filteredEventLog);
        eventLogFiltered();
        storeState();
    }

    /*************************************************************************************
     * PERSISTENT STATE
     */

    private void restoreState() {
        PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(CommonPlugin.PLUGIN_ID, getClass().getClassLoader());

        try {
            if (manager.hasProperty(getFile(), STATE_PROPERTY)) {
                eventLogFilterParameters = (EventLogFilterParameters)manager.getProperty(getFile(), STATE_PROPERTY);
                eventLogFilterParameters.setEventLogInput(this);
            }
        }
        catch (Exception e) {
            manager.removeProperty(getFile(), STATE_PROPERTY);

            throw new RuntimeException(e);
        }
    }

    private void storeState() {
        try {
            PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(CommonPlugin.PLUGIN_ID);

            if (eventLogFilterParameters == null)
                manager.removeProperty(getFile(), STATE_PROPERTY);
            else
                manager.setProperty(getFile(), STATE_PROPERTY, eventLogFilterParameters);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    /*************************************************************************************
     * LISTENERS
     */

    public void addEventLogChangedListener(IEventLogChangeListener listener) {
        if (eventLogChangeListeners.size() == 0)
            eventLogWatcher.start();

        if (!eventLogChangeListeners.contains(listener))
            eventLogChangeListeners.add(listener);
    }

    public void removeEventLogChangedListener(IEventLogChangeListener listener) {
        eventLogChangeListeners.remove(listener);

        if (eventLogChangeListeners.size() == 0)
            eventLogWatcher.stop();
    }

    private void eventLogChanged(int change) {
        switch (change) {
            case FileReader.FileChange.APPENDED:
                eventLogAppended();
                break;
            case FileReader.FileChange.OVERWRITTEN:
                eventLogOverwritten();
                break;
        }
    }

    private void eventLogAppended() {
        if (debug)
            Debug.println("Notifying listeners about new content being appended to the event log");

        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogAppended();

        if (debug)
            Debug.println("Event log append notification done");
    }

    private void eventLogOverwritten() {
        if (debug)
            Debug.println("Notifying listeners about the content being overwritten in the event log");

        moduleTreeRoot = null;

        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogOverwritten();

        if (debug)
            Debug.println("Event log overwritten notification done");
    }

    private void eventLogFiltered() {
        if (debug)
            Debug.println("Notifying listeners about new filter applied to the event log");

        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogFiltered();

        if (debug)
            Debug.println("Event log filter notification done");
    }

    private void eventLogFilterRemoved() {
        if (debug)
            Debug.println("Notifying listeners about removing filter from the event log");

        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogFilterRemoved();

        if (debug)
            Debug.println("Event log filter removing notification done");
    }

    private void eventLogLongOperationStarted() {
        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogLongOperationStarted();
    }

    private void eventLogLongOperationEnded() {
        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogLongOperationEnded();
    }

    private void eventLogProgress() {
        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogProgress();
    }

    private void eventLogSynchronizationFailed() {
        for (IEventLogChangeListener listener : eventLogChangeListeners)
            listener.eventLogSynchronizationFailed();
    }

    /*************************************************************************************
     * ERROR HANDLING FOR EVENTLOG RELATED OPERATIONS
     */

    public void runWithErrorHandling(Runnable runnable) {
        try {
            runnable.run();
        }
        catch (RuntimeException e) {
            handleRuntimeException(e);
        }
    }

    public void handleRuntimeException(RuntimeException e) {
        if (isFileChangedException(e))
            eventLogSynchronizationFailed();
        else
            throw e;
    }

    /*************************************************************************************
     * PROGRESS MONITORING OF LONG-RUNNING OPERATIONS
     */

    public boolean isCanceled() {
        return canceled;
    }

    public void resetCanceled() {
        canceled = false;
    }

    public void runWithProgressMonitor(Runnable runnable) {
        // canceled flag should be cleared upon explicit request by the user
        if (canceled)
            return;
        // avoid reentrant calls; the progress monitor is already set up and running
        else if (eventLogProgressManager.isInRunWithProgressMonitor())
            runnable.run();
        else {
            try {
                eventLog.setProgressCallInterval(3);
                eventLogProgressManager.runWithProgressMonitor(new IRunnableWithProgress() {
                    @Override
                    public void run(IProgressMonitor progressMonitor) throws InvocationTargetException, InterruptedException {
                        try {
                            eventLog.setProgressMonitor(eventLogProgressManager.getProgressMonitor());
                            runnable.run();
                        }
                        finally {
                            eventLog.setProgressMonitor(null);
                        }
                    }
                });
            }
            catch (Throwable t) {
                if (isLongRunningOperationCanceledException(t))
                    canceled = true;
                else if (t instanceof RuntimeException)
                    throw (RuntimeException)t;
                else
                    throw new RuntimeException(t);
            }
            finally {
                longRunningOperationInProgress = false;
                eventLogLongOperationEnded();
            }
        }
    }

    private boolean isLongRunningOperationCanceledException(Throwable t) {
        if (t instanceof LongRunningOperationCanceled)
            return true;
        else if (t instanceof InvocationTargetException)
            return isLongRunningOperationCanceledException(((InvocationTargetException)t).getTargetException());
        else if (t.getCause() != null)
            return isLongRunningOperationCanceledException(t.getCause());
        else
            return t.getMessage() != null && t.getMessage().contains("LongRunningOperationCanceled");
    }

    // TODO: check for specific C++ exception class
    public boolean isFileChangedException(Throwable t) {
        if (t instanceof InvocationTargetException)
            return isFileChangedException(((InvocationTargetException)t).getTargetException());
        else if (t.getCause() != null)
            return isFileChangedException(t.getCause());
        else
            return t.getMessage() != null && t.getMessage().contains("File changed: ");
    }

    public void progress() {
        if (!synchronizing && eventLogProgressManager.isInRunWithProgressMonitor()) {
            eventLogProgressManager.getProgressMonitor().internalWorked(0);
            if (!longRunningOperationInProgress) {
                longRunningOperationInProgress = true;
                eventLogLongOperationStarted();
            }

            eventLog.setProgressCallInterval(0.5);

            while (Display.getCurrent().readAndDispatch());

            eventLogProgress();

            if (eventLogProgressManager.isCanceled())
                throw new LongRunningOperationCanceled("A long-running operation was cancelled by the user from the progress monitor");
        }
    }

    public boolean isLongRunningOperationInProgress() {
        return longRunningOperationInProgress;
    }

    private static class LongRunningOperationCanceled extends RuntimeException
    {
        private static final long serialVersionUID = 1L;

        public LongRunningOperationCanceled(String string) {
            super(string);
        }
    }
}
