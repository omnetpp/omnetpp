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
	
	/**
	 * Called when the event log filter has been removed.
	 */
	public void eventLogFilterRemoved();

	/**
	 * Called when a long running event log operation starts just before the progress monitor is shown.
	 */
	public void eventLogLongOperationStarted();

	/**
	 * Called when a long running event log operation either normally or abnormally ends.
	 */
	public void eventLogLongOperationEnded();

	/**
	 * Called when a long running event log operation has done some progress.
	 */
	public void eventLogProgress();
}
