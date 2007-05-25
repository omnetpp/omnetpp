package org.omnetpp.common.eventlog;

public interface IEventLogChangeListener {
	/**
	 * Called when new content has been appended to the event log file by the simulation runtime environment.
	 */
	public void eventLogAppended();
	
	/**
	 * Called when the event log has been filtered.
	 */
	public void eventLogFiltered();
}
