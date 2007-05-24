package org.omnetpp.common.eventlog;

public interface IEventLogChangedListener {
	/**
	 * Called when the event log file gets updated by the simulation runtime environment.
	 */
	public void eventLogChanged();
}
