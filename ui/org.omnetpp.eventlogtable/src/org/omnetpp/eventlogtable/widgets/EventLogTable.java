package org.omnetpp.eventlogtable.widgets;

import org.eclipse.jface.action.MenuManager;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.TableColumn;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.virtualtable.VirtualTableViewer;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.EventLogTableFacade;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlogtable.editors.EventLogTableContributor;

public class EventLogTable extends VirtualTableViewer<EventLogEntry> {
	private MenuManager menuManager;

	public EventLogTable(Composite parent) {
		super(parent);

		setContentProvider(new EventLogTableContentProvider());
		setLineRenderer(new EventLogTableLineRenderer());

		TableColumn tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(60);
		tableColumn.setText("Event #");

		tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(140);
		tableColumn.setText("Time");

		tableColumn = virtualTable.createColumn();
		tableColumn.setWidth(2000);
		tableColumn.setText("Details");

		menuManager = new MenuManager();
		new EventLogTableContributor(this).contributeToPopupMenu(menuManager);
		virtualTable.setMenu(menuManager.createContextMenu(virtualTable));
	}

	@Override
	public void setInput(Object input) {
		super.setInput(input);

		if (input != null)
			((EventLogTableLineRenderer)getLineRenderer()).setInput((EventLogInput)input);
	}

	public IEventLog getEventLog() {
		return ((EventLogInput)getInput()).getEventLog();
	}

	public EventLogTableFacade getEventLogTableFacade() {
		return ((EventLogInput)getInput()).getEventLogTableFacade();
	}

	public EventLogTableContentProvider getEventLogTableContentProvider() {
		return (EventLogTableContentProvider)getContentProvider();
	}

	public int getFilterMode() {
		return getEventLogTableContentProvider().getFilterMode();
	}

	public void setFilterMode(int i) {
		getEventLogTableContentProvider().setFilterMode(i);
		virtualTable.stayNear();
	}
}
