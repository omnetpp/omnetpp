package org.omnetpp.eventlogtable.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IViewSite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.eventlogtable.widgets.EventLogTable;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
public class EventLogTableView extends ViewPart {
	protected EventLogTable eventLogTable;

	@Override
	public void createPartControl(Composite parent) {
		IViewSite viewSite = (IViewSite)getSite();
		eventLogTable = new EventLogTable(parent, SWT.NONE);

		// we want to provide selection for the sequence chart tool (an IEditPart)
		viewSite.setSelectionProvider(eventLogTable);
		
		// contribue to toolbar
		EventLogTableContributor eventLogTableContributor = new EventLogTableContributor(eventLogTable);
		eventLogTable.setEventLogTableContributor(eventLogTableContributor);
		eventLogTableContributor.contributeToToolBar(viewSite.getActionBars().getToolBarManager());

		// follow selection
		viewSite.getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != EventLogTableView.this)
					eventLogTable.setSelection(selection);
			}
		});
		
		// bootstrap with current selection
		eventLogTable.setSelection(getSite().getSelectionProvider().getSelection());
	}

	@Override
	public void setFocus() {
		eventLogTable.setFocus();
	}
}
