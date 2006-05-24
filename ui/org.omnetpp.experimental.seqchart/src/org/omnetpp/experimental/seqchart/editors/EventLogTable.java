package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.engine.EventEntry;
import org.omnetpp.scave.engine.EventLog;

/**
 * Table to hold the event log. Has to be prepared for very large amounts of 
 * data (several hundred thousand lines).
 * 
 * @author andras
 */
//FIXME this should actually be turned into a View!
public class EventLogTable extends VirtualTableTreeBase {
	
	private EventLog eventLog;

	public EventLogTable(Composite parent, int style) {
		super(parent, SWT.NONE);
		
		table.setHeaderVisible(true);
		table.setLinesVisible(false);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Event#");
		TableColumn tableColumn1 = new TableColumn(table, SWT.NONE);
		tableColumn1.setWidth(80);
		tableColumn1.setText("Time");
		TableColumn tableColumn2 = new TableColumn(table, SWT.NONE);
		tableColumn2.setWidth(80);
		tableColumn2.setText("Module class");
		TableColumn tableColumn3 = new TableColumn(table, SWT.NONE);
		tableColumn3.setWidth(80);
		tableColumn3.setText("Module");
		TableColumn tableColumn4 = new TableColumn(table, SWT.NONE);
		tableColumn4.setWidth(800);
		tableColumn4.setText("Details / log message");
	}

	public void setInput(EventLog eventLog) {
		this.eventLog = eventLog;
		table.setItemCount(eventLog.getNumEvents()); //XXX count log msgs too!
		table.clearAll();
	}

	protected void configureTableItem(TableItem item, int lineNumber) {
		EventEntry event = eventLog.getEvent(lineNumber);
		item.setImage(0, ImageFactory.getImage(ImageFactory.TOOLBAR_IMAGE_SIMPLE)); // plus/minus sign
		item.setText(0, "#"+event.getEventNumber());
		item.setText(1, ""+event.getSimulationTime()+"s");
		item.setText(2, ""+event.getModule().getModuleClassName());
		item.setText(3, ""+event.getModule().getModuleFullPath());
		item.setText(4, "handleMessage ("+event.getCause().getMessageClassName()+") "+event.getCause().getMessageName());
	}
}
