package org.omnetpp.experimental.seqchart.editors;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.experimental.seqchart.widgets.EventLogVirtualTableItemProvider;
import org.omnetpp.experimental.seqchart.widgets.EventLogVirtualTableContentProvider;
import org.omnetpp.experimental.seqchart.widgets.LongVirtualTableItemProvider;
import org.omnetpp.experimental.seqchart.widgets.LongVirtualTableContentProvider;
import org.omnetpp.experimental.seqchart.widgets.VirtualTableViewer;

/**
 * View for displaying and navigating simulation events and associated log messages.
 */
//TODO add double-click support (factor out filtering to an Action?)
//TODO add context menu etc
//FIXME the <category name="OMNeT++/OMNEST" id="org.omnetpp"> stuff in plugin.xml doesn't seem to take effect
public class EventLogView extends ViewPart {

	public static final String PART_ID = "org.omnetpp.experimental.seqchart.editors.EventLogView"; 

	protected VirtualTableViewer virtualTableViewer;
	//protected Action doubleClickAction;


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
		Table table = new Table(parent, SWT.FULL_SELECTION | SWT.VIRTUAL);
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

		virtualTableViewer = createLongTableViewer(table);
		//virtualTableViewer = createEventLogTableViewer(table);
		
		// we want to provide selection for the sequence chart tool (an IEditPart)
		getSite().setSelectionProvider(virtualTableViewer);

		// follow selection
		getSite().getPage().addSelectionListener(new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				if (part != virtualTableViewer)
					virtualTableViewer.setSelection(selection);
			}
		});
		
		// bootstrap with current selection
		virtualTableViewer.setSelection(getSite().getSelectionProvider().getSelection());
	}

	/**
	 * For debug purposes.
	 */
	protected VirtualTableViewer createLongTableViewer(Table table) {
		VirtualTableViewer virtualTableViewer = new VirtualTableViewer(table);
		virtualTableViewer.setContentProvider(new LongVirtualTableContentProvider());
		virtualTableViewer.setInput(1000000L);
		virtualTableViewer.setLabelProvider(new LongVirtualTableItemProvider());
		
		return virtualTableViewer;
	}
	
	protected VirtualTableViewer createEventLogTableViewer(Table table) {
		VirtualTableViewer virtualTableViewer = new VirtualTableViewer(table);
		virtualTableViewer.setContentProvider(new EventLogVirtualTableContentProvider());
		virtualTableViewer.setInput(null);
		virtualTableViewer.setLabelProvider(new EventLogVirtualTableItemProvider());
		
		return virtualTableViewer;
	}
	
	/**
	 * Passing the focus request to the viewer's control.
	 */
	@Override
	public void setFocus() {
		virtualTableViewer.getControl().setFocus();
	}

//	private void hookDoubleClickAction() {
//		viewer.addDoubleClickListener(new IDoubleClickListener() {
//			public void doubleClick(DoubleClickEvent event) {
//				doubleClickAction.run();
//			}
//		});
//	}
}
