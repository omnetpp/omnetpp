package org.omnetpp.experimental.seqchart.editors;


import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.seqchart.widgets.EventLogTable;


/**
 * View for displaying and navigating simulation events and associated log messages.
 */
//TODO add double-click support (factor out filtering to an Action?)
//TODO add context menu etc
//FIXME the <category name="OMNeT++/OMNEST" id="org.omnetpp"> stuff in plugin.xml doesn't seem to take effect
public class EventLogView extends ViewPart {

	public static final String PART_ID = "org.omnetpp.experimental.seqchart.editors.EventLogView"; 

	private EventLogTable eventLogTable;
	//private Action doubleClickAction;


	/**
	 * The constructor.
	 */
	public EventLogView() {
	}

	/**
	 * Create the viewer and initialize it.
	 */
	@Override
	public void createPartControl(Composite parent) {
		eventLogTable = new EventLogTable(parent, SWT.MULTI);
		
		// we want to provide selection for the sequence chart tool (an IEditPart)
		getSite().setSelectionProvider(eventLogTable);

		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part!=eventLogTable)
					eventLogTable.setSelection(selection);
			}
		});
		
		// bootstrap with current selection
		eventLogTable.setSelection(getSite().getSelectionProvider().getSelection());
}

	public EventLogTable getEventLogTable() {
		return eventLogTable;
	}
	
	/**
	 * Passing the focus request to the viewer's control.
	 */
	@Override
	public void setFocus() {
		eventLogTable.setFocus();
	}

//	private void hookDoubleClickAction() {
//		viewer.addDoubleClickListener(new IDoubleClickListener() {
//			public void doubleClick(DoubleClickEvent event) {
//				doubleClickAction.run();
//			}
//		});
//	}

}
