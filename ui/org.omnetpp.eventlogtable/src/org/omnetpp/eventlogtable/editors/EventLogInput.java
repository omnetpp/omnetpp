package org.omnetpp.eventlogtable.editors;

import org.eclipse.core.resources.IFile;
import org.omnetpp.eventlog.engine.EventLog;


public class EventLogInput {
	protected IFile file;
	
	protected EventLog eventLog;

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
}
