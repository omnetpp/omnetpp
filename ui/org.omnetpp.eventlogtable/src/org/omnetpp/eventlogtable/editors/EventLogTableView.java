package org.omnetpp.eventlogtable.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
public class EventLogTableView extends ViewWithMessagePart {
	protected EventLogTable eventLogTable;

	@Override
	public void createPartControl(Composite parent) {
		super.createPartControl(parent);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		IViewSite viewSite = (IViewSite)getSite();
		viewSite.setSelectionProvider(eventLogTable);
		
		// contribute to toolbar
		EventLogTableContributor eventLogTableContributor = new EventLogTableContributor(eventLogTable);
		eventLogTable.setEventLogTableContributor(eventLogTableContributor);
		eventLogTableContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager());

		// follow selection
		ISelectionListener listener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != EventLogTableView.this)
					eventLogTable.setSelection(selection);

				if (eventLogTable.getInput() == null)
					displayMessage("No event log available");
				else
					hideMessage();
			}
		};
		viewSite.getPage().addSelectionListener(listener);

		// bootstrap with current selection
		listener.selectionChanged(null, getActiveEditorSelection());
	}

	@Override
	public void setFocus() {
		eventLogTable.setFocus();
	}

	@Override
	protected Control createViewControl(Composite parent) {
		eventLogTable = new EventLogTable(parent, SWT.NONE);

		return eventLogTable;
	}
}
