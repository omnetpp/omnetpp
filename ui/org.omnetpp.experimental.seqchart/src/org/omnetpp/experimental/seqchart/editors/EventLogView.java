package org.omnetpp.experimental.seqchart.editors;


import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.part.*;
import org.eclipse.jface.viewers.*;
import org.eclipse.swt.graphics.Image;
import org.eclipse.jface.action.*;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.*;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.SWT;
import org.omnetpp.experimental.seqchart.widgets.EventLogTable;


/**
 * View for displaying and navigating simulation events and associated log messages.
 */
//FIXME the <category name="OMNeT++/OMNEST" id="org.omnetpp"> stuff in plugin.xml doesn't seem to take effect
public class EventLogView extends ViewPart {

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
		SequenceChartToolEditor editor = getCurrentSequenceChartEditor();
		if (editor!=null)
			eventLogTable.setInput(editor.getFilteredEventLog());
	}

	/**
	 * Returns the currently active sequence chart editor, or null
	 * if the active editor is not a sequence chart editor.
	 */
	protected SequenceChartToolEditor getCurrentSequenceChartEditor() {
		IWorkbenchPage page = getSite().getWorkbenchWindow().getActivePage();
		if (page==null) return null;
		IEditorPart currentEditor = page.getActiveEditor();
		return currentEditor instanceof SequenceChartToolEditor ?
			(SequenceChartToolEditor)currentEditor : null;
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
//	private void showMessage(String message) {
//		MessageDialog.openInformation(
//			viewer.getControl().getShell(),
//			"Sample View",
//			message);
//	}

}
