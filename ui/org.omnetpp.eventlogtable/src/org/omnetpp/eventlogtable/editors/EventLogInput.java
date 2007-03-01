package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IFile;
import org.omnetpp.eventlog.engine.EventLog;
import org.omnetpp.eventlog.engine.EventLogTableFacade;


public class EventLogInput {
	protected IFile file;
	
	protected EventLog eventLog;
	
	protected EventLogTableFacade eventLogTableFacade;

	public IFile getFile() {
		return file;
	}

	public void setFile(IFile file) {
		this.file = file;
	}

	public EventLog getEventLog() {
		return eventLog;
	}

	public void setEventLog(EventLog eventLog) {
		this.eventLog = eventLog;
	}
	
	public EventLogTableFacade getEventLogTableFacade() {
		if (eventLogTableFacade == null)
			eventLogTableFacade = new EventLogTableFacade(eventLog);

		return eventLogTableFacade;
	}
}
