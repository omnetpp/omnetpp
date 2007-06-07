package org.omnetpp.common.eventlog;

import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.omnetpp.common.util.RecurringJob;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

/**
 * Input object for event log file editors and viewers.
 */
public class EventLogInput {
	private static final boolean debug = true;

	/**
	 * The event log file.
	 */
	protected IFile file;
	
	/**
	 * The C++ wrapper around the event log reader.
	 */
	protected IEventLog eventLog;
	
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
	protected ModuleTreeItem moduleTree;

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
	
	public EventLogInput() {
		eventLogWatcher = new RecurringJob(1000) {
			public void run() {
				checkEventLogForChanges();
			}
		};
	}

	public EventLogInput(IFile file, IEventLog eventLog) {
		this();
		this.file = file;
		this.eventLog = eventLog;
	}
	
	public IFile getFile() {
		return file;
	}

	public void setFile(IFile file) {
		this.file = file;
	}

	public IEventLog getEventLog() {
		return eventLog;
	}

	public void setEventLog(IEventLog eventLog) {
		this.eventLog = eventLog;
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
		if (moduleTree == null) {
			ModuleTreeBuilder treeBuilder = new ModuleTreeBuilder();
			for (int i = 1; i <= eventLog.getNumModuleCreatedEntries(); i++) {
				ModuleCreatedEntry entry = eventLog.getModuleCreatedEntry(i);
				
				if (entry != null)
					treeBuilder.addModule(entry.getParentModuleId(), entry.getModuleId(), entry.getModuleClassName(), entry.getFullName());
			}
	
			moduleTree = treeBuilder.getModuleTree();
		}
		
		return moduleTree;
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


	public void removeFilter() {
		// remove filter
		if (eventLog instanceof FilteredEventLog)
			eventLog = ((FilteredEventLog)eventLog).getEventLog();
		eventLog.own();

		/// store event log
		getEventLogTableFacade().setEventLog(eventLog);
		getSequenceChartFacade().setEventLog(eventLog);

		// TODO: move to SequenceChart
		// update coordinate system
		if (sequenceChartFacade.getTimelineCoordinateSystemOriginEventNumber() != -1)
			sequenceChartFacade.relocateTimelineCoordinateSystem(sequenceChartFacade.getTimelineCoordinateSystemOriginEvent());
	}
	
	public void addFilter(IntVector moduleIds) {
		// remove old filter
		if (eventLog instanceof FilteredEventLog)
			eventLog = ((FilteredEventLog)eventLog).getEventLog();
		eventLog.disown();

		// create new filter
		FilteredEventLog filteredEventLog = new FilteredEventLog(eventLog);
		filteredEventLog.setModuleIds(moduleIds);

		// store event log
		eventLog = filteredEventLog;
		getEventLogTableFacade().setEventLog(filteredEventLog);
		getSequenceChartFacade().setEventLog(filteredEventLog);
		
		eventLogFiltered();
	}

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

	public void eventLogAppended() {
		for (IEventLogChangeListener listener : eventLogChangeListeners)
			listener.eventLogAppended();
	}

	private void eventLogFiltered() {
		for (IEventLogChangeListener listener : eventLogChangeListeners)
			listener.eventLogFiltered();
	}

	public void checkEventLogForChanges() {
		if (eventLog.getFileReader().isChanged()) {
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

	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		eventLogChangeListeners.clear();
		eventLogWatcher.stop();
	}
}
