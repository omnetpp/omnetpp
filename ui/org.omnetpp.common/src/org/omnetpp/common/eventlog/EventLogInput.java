package org.omnetpp.common.eventlog;

import org.eclipse.core.resources.IFile;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEventLog;


public class EventLogInput {
	protected IFile file;
	
	protected IEventLog eventLog;
	
	protected EventLogTableFacade eventLogTableFacade;

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
}
