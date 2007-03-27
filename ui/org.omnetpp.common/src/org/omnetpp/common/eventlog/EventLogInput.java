package org.omnetpp.common.eventlog;

import org.eclipse.core.resources.IFile;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.SequenceChartFacade;

/**
 * Input object for event log file editors and viewers.
 */
public class EventLogInput {
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
}
