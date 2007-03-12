package org.omnetpp.common.eventlog;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors/viewers.
 *  
 * @author andras
 */
public interface IEventLogSelection extends ISelection {
	/**
	 * The EventLog object we are dealing with. All selected Events
	 * belong to this EventLog. Never returns null.  
	 */
	public IEventLog getEventLog();
	
	/**
	 * The list of selected events. The returned list MUST NOT be modified
	 * by clients. Never returns null. 
	 */
	public List<IEvent> getEvents();

	/**
	 * Returns the first EventEntry in the selection, or null.
	 */
	public IEvent getFirstEvent();
}
