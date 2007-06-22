package org.omnetpp.common.eventlog;

import java.util.ArrayList;

import org.eclipse.core.resources.IFile;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.PersistentResourcePropertyManager;
import org.omnetpp.common.util.RecurringJob;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.FilteredEventLog;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.IntVector;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.PStringVector;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

/**
 * Input object for event log file editors and viewers.
 */
public class EventLogInput {
	private static final boolean debug = true;

	private static final String STATE_PROPERTY = "EventLogInputState";

	/**
	 * The event log file.
	 */
	protected IFile file;
	
	/**
	 * The C++ wrapper around the event log reader.
	 */
	protected IEventLog eventLog;
	
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
	
	public EventLogInput(IFile file, IEventLog eventLog) {
		this.eventLog = eventLog;
		this.file = file;
		this.eventLogWatcher = new RecurringJob(1000) {
			public void run() {
				checkEventLogForChanges();
			}
		};

		restoreState();
	}
	
	public IFile getFile() {
		return file;
	}

	public IEventLog getEventLog() {
		return eventLog;
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

	public ArrayList<ModuleTreeItem> getSelectedModules() {
		if (eventLog instanceof FilteredEventLog && eventLogFilterParameters.enableModuleFilter)
			return eventLogFilterParameters.getSelectedModules();
		else
			return getAllModules();
	}

	public void removeFilter() {
		if (eventLog instanceof FilteredEventLog) {
			eventLog = ((FilteredEventLog)eventLog).getEventLog();
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
			eventLog = ((FilteredEventLog)eventLog).getEventLog();
		eventLog.disown();

		// create new filter
		FilteredEventLog filteredEventLog = new FilteredEventLog(eventLog);

		filteredEventLog.setFirstEventNumber(eventLogFilterParameters.getFirstEventNumber());
		filteredEventLog.setLastEventNumber(eventLogFilterParameters.getLastEventNumber());

		if (eventLogFilterParameters.enableTraceFilter) {
			filteredEventLog.setTracedEventNumber(eventLogFilterParameters.tracedEventNumber);
			filteredEventLog.setTraceCauses(eventLogFilterParameters.traceCauses);
			filteredEventLog.setTraceConsequences(eventLogFilterParameters.traceConsequences);
		}

		if (eventLogFilterParameters.enableModuleFilter) {
			if (eventLogFilterParameters.moduleIds != null) {
				IntVector moduleIds = new IntVector();
				for (int id : eventLogFilterParameters.moduleIds)
					moduleIds.add(id);
				filteredEventLog.setModuleIds(moduleIds);
			}
	
			if (eventLogFilterParameters.moduleTypes != null) {
				PStringVector moduleTypes = new PStringVector();
				for (String moduleType : eventLogFilterParameters.moduleTypes)
					moduleTypes.add(moduleType);			
				filteredEventLog.setModuleTypes(moduleTypes);
			}
		}

		// store event log
		eventLog = filteredEventLog;
		getEventLogTableFacade().setEventLog(filteredEventLog);
		getSequenceChartFacade().setEventLog(filteredEventLog);
		
		eventLogFiltered();

		storeState();
	}
	
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
