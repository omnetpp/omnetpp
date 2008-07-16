package org.omnetpp.common.eventlog;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.eventlog.engine.IEventLog;

/**
 * Selection that is published by event log editors/viewers.
 *  
 * @author andras
 */
public interface IEventLogSelection extends ISelection {
	/**
	 * The EventLogInput object we are dealing with. All selected Events
	 * belong to this EventLogInput. Never returns null.  
	 */
	public EventLogInput getEventLogInput();

	/**
	 * The EventLog object we are dealing with. All selected Events
	 * belong to this EventLog. Never returns null.  
	 */
	public IEventLog getEventLog();
	
	/**
	 * The list of selected event numbers. The returned list MUST NOT be modified
	 * by clients. Never returns null. 
	 */
	public List<Long> getEventNumbers();

	/**
	 * Returns the first event number in the selection, or null.
	 */
	public Long getFirstEventNumber();
}
