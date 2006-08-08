package org.omnetpp.experimental.seqchart.editors;

import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLog;

/**
 * Selection that is published by the sequence chart tool
 * (SequenceChartToolEditor) and its associated log view.
 *  
 * @author andras
 */
public interface IEventLogSelection extends ISelection {
	/**
	 * The EventLog object we are dealing with. All selected EventEntries
	 * belong to this EventLog. Never returns null.  
	 */
	public EventLog getEventLog();
	
	/**
	 * The list of selected events. The returned list MUST NOT be modified
	 * by clients. Never returns null. 
	 */
	public List<EventEntry> getEvents();

	/**
	 * Returns the first EventEntry in the selection, or null.
	 */
	public EventEntry getFirstEvent();
}
