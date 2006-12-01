package org.omnetpp.eventlogtable.editors;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.virtualtable.VirtualTableSelection;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.selection.EventLogSelection;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
public class EventLogTableView extends ViewPart {
	protected EventLogTable eventLogTable;

	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent);
		
//		IViewSite viewSite = (IViewSite)getSite();
//		viewSite.getActionBars().getToolBarManager().add(new Action("Hello", null) {
//			// TODO:
//		});

		// we want to provide selection for the sequence chart tool (an IEditPart)
		getSite().setSelectionProvider(eventLogTable);

		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != eventLogTable && selection instanceof EventLogSelection) {
					EventLogSelection eventLogSelection = (EventLogSelection)selection;
					List<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>();
					for (IEvent event : eventLogSelection.getEvents())
						eventLogEntries.add(event.getEventEntry());
					eventLogTable.setSelection(new VirtualTableSelection<EventLogEntry>(eventLogSelection.getInput(), eventLogEntries));
				}
			}
		});
		
		// bootstrap with current selection
		eventLogTable.setSelection(getSite().getSelectionProvider().getSelection());
	}
	
	@Override
	public void setFocus() {
		eventLogTable.getControl().setFocus();
	}
}
