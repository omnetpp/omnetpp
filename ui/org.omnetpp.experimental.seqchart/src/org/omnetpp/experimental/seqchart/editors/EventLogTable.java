package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.draw2d.ColorConstants;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;
import org.omnetpp.scave.engine.MessageEntries;
import org.omnetpp.scave.engine.PStringVector;

/**
 * Table to hold the event log. Has to be prepared for very large amounts of 
 * data (several hundred thousand lines).
 * 
 * @author andras
 */
//FIXME this should actually be turned into a View!
//FIXME actually implement expand/collapse! [+] and [-] images to be displayed are there in VirtualTableTreeBase; see also eventLog.collapseEvent/expandEvent 
//FIXME also: remove getTable and provide better encapsulation instead
public class EventLogTable extends VirtualTableTreeBase {
	
	private EventLog eventLog;
	private Font boldFont = JFaceResources.getFontRegistry().getBold(JFaceResources.DEFAULT_FONT);
	private Color blueColor = ColorConstants.blue;

	public EventLogTable(Composite parent, int style) {
		super(parent, style);
		
		table.setHeaderVisible(true);
		table.setLinesVisible(false);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Event#");
		TableColumn tableColumn1 = new TableColumn(table, SWT.NONE);
		tableColumn1.setWidth(140);
		tableColumn1.setText("Time");
		TableColumn tableColumn4 = new TableColumn(table, SWT.NONE);
		tableColumn4.setWidth(800);
		tableColumn4.setText("Details / log message");
	}

	public void setInput(EventLog eventLog) {
		this.eventLog = eventLog;

		EventEntry oldSelectionEvent = null;
		if (eventLog!=null)
			oldSelectionEvent = eventForTableIndex(table.getSelectionIndex());
		
		// make log messages visible
		eventLog.expandAllEvents();
		
		// calculate number of rows and set table row count
		EventEntry lastEvent = eventLog.getLastEvent();
		
		if (lastEvent != null)
		{
			int lastEventRows = 1 + (lastEvent.getIsExpandedInTree() ? lastEvent.getNumLogMessages() : 0);
			table.setItemCount(lastEvent==null ? 0 : lastEvent.getTableRowIndex()+lastEventRows);
		}
		else
			table.setItemCount(0);			

		if (oldSelectionEvent!=null)
			gotoEvent(oldSelectionEvent);
		
		// refresh
		table.clearAll();
	}

	/**
	 * Position the table selection to the given event, or if it's not in the current 
	 * filtered event log, to an event near it.
	 */
	public void gotoEvent(EventEntry event) {
		if (event!=null) {
			if (!eventLog.containsEvent(event))
				event = eventLog.getLastEventBefore(event.getSimulationTime());
			if (event!=null) {
				int index = tableIndexForEvent(event);
				table.setSelection(index);
				table.setTopIndex(index);
			}
		}
	}

	protected void configureTableItem(TableItem item, int lineNumber) {
		EventEntry event = eventLog.getEventByTableRowIndex(lineNumber);
		if (event==null) {
			item.setText(0, "null");
			return;
		}

		if (lineNumber == event.getTableRowIndex()) {
			//item.setFont(boldFont);
			item.setForeground(blueColor);
			//item.setImage(0, ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_SIMPLE)); // plus/minus sign
			item.setText(0, "#"+event.getEventNumber());
			item.setText(1, ""+event.getSimulationTime()+"s");
			item.setText(2, "module ("+event.getCause().getModule().getModuleClassName()
							+") "+event.getCause().getModule().getModuleFullPath()
							+", on arrival of message ("+event.getCause().getMessageClassName()
							+") "+event.getCause().getMessageName());
		}
		else {
			int logMessageIndex = lineNumber - event.getTableRowIndex()-1;
			//Assert.isTrue(logMessageIndex>=0);
			String logMessage = getLogMessage(event, logMessageIndex);
			if (logMessage==null)
				logMessage = "bad log message index "+logMessageIndex; //XXX debug code
			item.setText(0, "");
			item.setText(1, "");
			item.setText(2, logMessage);
		}
	}

	/**
	 * Utility function to get the ith log message line for an event.
	 * They are stored with the causes[] messages.
	 */
	private String getLogMessage(EventEntry event, int logMessageIndex) {
		MessageEntries causes = event.getCauses();
		for (int i=0; i<causes.size(); i++) {
			PStringVector msgs = causes.get(i).getLogMessages();
			if (logMessageIndex < msgs.size())
				return msgs.get(logMessageIndex);
			else
				logMessageIndex -= msgs.size();
		}
		return null; // bad luck
	}
	
	public EventEntry eventForTableIndex(int index) {
		return eventLog.getEventByTableRowIndex(index);
	}

	public int tableIndexForEvent(EventEntry event) {
		return event.getTableRowIndex();
	}
}
