package org.omnetpp.common.eventlog;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.RecurringJob;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.FileReader;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

/**
 * Input object for event log file editors and viewers.
 */
public class EventLogInput
	implements IEventLogProgressMonitor
{
	private static final boolean debug = true;

	public static final String STATE_PROPERTY = "EventLogInputState";

	/**
	 * The event log file.
	 */
	protected IFile file;
	
	/**
	 * The C++ wrapper around the event log reader.
	 */
	protected IEventLog eventLog;
	
	/**
	 * Manages long running operations for the event log.
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
	 * Watches the event log file for changes.
	 */
	protected RecurringJob eventLogWatcher;

	/**
	 * True indicates a long running operation was canceled by the user.
	 */
	private boolean canceled;
	
	/**
	 * True means a long running operation is in progress and the progress dialog is already shown.
	 */
	private boolean longRunningOperationInProgress;

	/*************************************************************************************
	 * CONSTRUCTION
	 */
	
	public EventLogInput(IFile file, IEventLog eventLog) {
		this.file = file;
		this.eventLogProgressManager = new EventLogProgressManager();
		this.eventLogWatcher = new RecurringJob(3000) {
			public void run() {
				checkEventLogForChanges();
			}
		};
		
		setEventLog(eventLog);
		restoreState();
	}

	private void checkEventLogForChanges() {
		if (eventLog.getFileReader().getFileChangedState() == FileReader.FileChangedState.APPENDED) {
			if (debug)
				System.out.println("Notifying listeners about new content being appended to the event log");

			eventLog.synchronize();
			getEventLogTableFacade().synchronize();
			getSequenceChartFacade().synchronize();

			eventLogAppended();

			if (debug)
				System.out.println("Event log append notification done");
		}
	}

	public void dispose() {
		if (eventLogWatcher != null)
			eventLogWatcher.stop();
		
		if (eventLog != null)
		    eventLog.getFileReader().ensureFileClosed();
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

	public IFile getFile() {
		return file;
	}
	
	private void setEventLog(IEventLog eventLog) {
		this.eventLog = eventLog;
	}

	public IEventLog getEventLog() {
		return eventLog;
	}
	
	public EventLogProgressManager getEventLogProgressManager() {
		return eventLogProgressManager;
	}

	public EventLogFilterParameters getFilterParameters() {
		if (eventLogFilterParameters == null) {
			eventLogFilterParameters = new EventLogFilterParameters(this);
		}

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

	public ModuleTreeItem getModuleTreeRoot() {
		if (moduleTreeRoot == null) {
			ModuleTreeBuilder treeBuilder = new ModuleTreeBuilder();
			for (int i = 1; i <= eventLog.getNumModuleCreatedEntries(); i++) {
				ModuleCreatedEntry entry = eventLog.getModuleCreatedEntry(i);
				
				if (entry != null)
					treeBuilder.addModule(entry.getParentModuleId(), entry.getModuleId(), entry.getModuleClassName(), entry.getFullName());
			}
	
			moduleTreeRoot = treeBuilder.getModuleTree();
		}
		
		return moduleTreeRoot;
	}

	public ArrayList<ModuleTreeItem> getAllModules() {
		final ArrayList<ModuleTreeItem> modules = new ArrayList<ModuleTreeItem>();

		getModuleTreeRoot().visitLeaves(new ModuleTreeItem.IModuleTreeItemVisitor() {
			public void visit(ModuleTreeItem treeItem) {
				if (treeItem != getModuleTreeRoot() && treeItem.getSubmodules().length == 0)
					modules.add(treeItem);
			}
		});

		return modules;
	}

	public ArrayList<ModuleTreeItem> getSelectedModules() {
		if (eventLog instanceof FilteredEventLog && eventLogFilterParameters.enableModuleFilter) {
			ArrayList<ModuleTreeItem> selectedModules = new ArrayList<ModuleTreeItem>();
			IntVector moduleIds = ((FilteredEventLog)eventLog).getSelectedModuleIds();

			for (int i = 0; i < moduleIds.size(); i++)
				selectedModules.add(moduleTreeRoot.findDescendantModule(moduleIds.get(i)));
			
			return selectedModules;
		}
		else
			return getAllModules();
	}

	/*************************************************************************************
	 * FILTERING
	 */

	public void removeFilter() {
		if (eventLog instanceof FilteredEventLog) {
			setEventLog(((FilteredEventLog)eventLog).getEventLog());
			eventLog.own();
	
			/// store event log
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
		eventLog.disown();

		// create new filter
		FilteredEventLog filteredEventLog = new FilteredEventLog(eventLog);

		// general filter
		filteredEventLog.setFirstEventNumber(eventLogFilterParameters.getFirstEventNumber());
		filteredEventLog.setLastEventNumber(eventLogFilterParameters.getLastEventNumber());

		filteredEventLog.setEnableModuleFilter(eventLogFilterParameters.enableModuleFilter);
		if (eventLogFilterParameters.enableModuleFilter) {
			filteredEventLog.setModuleExpression(eventLogFilterParameters.moduleFilterExpression);
			filteredEventLog.setModuleIds(eventLogFilterParameters.getModuleIds());
			filteredEventLog.setModuleClassNames(eventLogFilterParameters.getModuleClassNames());
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
			filteredEventLog.setMessageExpression(eventLogFilterParameters.messageFilterExpression);
			filteredEventLog.setMessageClassNames(eventLogFilterParameters.getMessageClassNames());
			filteredEventLog.setMessageNames(eventLogFilterParameters.getMessageNames());
			filteredEventLog.setMessageIds(eventLogFilterParameters.getMessageIds());
			filteredEventLog.setMessageTreeIds(eventLogFilterParameters.getMessageTreeIds());
			filteredEventLog.setMessageEncapsulationIds(eventLogFilterParameters.getMessageEncapsulationIds());
			filteredEventLog.setMessageEncapsulationTreeIds(eventLogFilterParameters.getMessageEcapsulationTreeIds());
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
			if (manager.hasProperty(file, STATE_PROPERTY)) {
				eventLogFilterParameters = (EventLogFilterParameters)manager.getProperty(file, STATE_PROPERTY);
				eventLogFilterParameters.setEventLogInput(this);
				// TODO: we should not filter unconditionally
				// TODO: what if filter is broken? (e.g. timeout) rather let the user decide
				// filter();
			}
		}
		catch (Exception e) {
			manager.removeProperty(file, STATE_PROPERTY);

			throw new RuntimeException(e);
		}
	}
	
	private void storeState() {
		try {
			PersistentResourcePropertyManager manager = new PersistentResourcePropertyManager(CommonPlugin.PLUGIN_ID);

			if (eventLogFilterParameters == null)
				manager.removeProperty(file, STATE_PROPERTY);
			else {
				manager.setProperty(file, STATE_PROPERTY, eventLogFilterParameters);
			}
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

	private void eventLogAppended() {
		for (IEventLogChangeListener listener : eventLogChangeListeners)
			listener.eventLogAppended();
	}

	private void eventLogFiltered() {
		for (IEventLogChangeListener listener : eventLogChangeListeners)
			listener.eventLogFiltered();
	}

	private void eventLogFilterRemoved() {
		for (IEventLogChangeListener listener : eventLogChangeListeners)
			listener.eventLogFilterRemoved();
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

	/*************************************************************************************
	 * PROGRESS MONITORING OF LONG RUNNING OPERATIONS
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
	            eventLog.setJavaProgressMonitor(null, this);
				eventLog.setProgressCallInterval(3);

	            eventLogProgressManager.runWithProgressMonitor(runnable);
    		}
    		catch (Throwable t) {
    			if (isLongRunningOperationCanceledException(t))
    				canceled = true;
    			else
    				throw new RuntimeException(t);
    		}
    		finally {
                eventLog.setJavaProgressMonitor(null, null);

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

	public void progress() {
		if (eventLogProgressManager.isInRunWithProgressMonitor()) {
			if (!longRunningOperationInProgress) {
				longRunningOperationInProgress = true;
				eventLogLongOperationStarted();
			}

			eventLogProgressManager.showProgressDialog();
			eventLog.setProgressCallInterval(0.5);

			while (Display.getCurrent().readAndDispatch());
	
			eventLogProgress();

			if (eventLogProgressManager.isCanceled())
				throw new LongRunningOperationCanceled("A long running operation from the progress monitor was cancelled by the user");
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
