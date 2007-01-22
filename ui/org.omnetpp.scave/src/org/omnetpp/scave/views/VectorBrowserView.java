package org.omnetpp.scave.views;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.scave.editors.datatable.VectorResultContentProvider;
import org.omnetpp.scave.editors.datatable.VectorResultLabelProvider;
import org.omnetpp.scave.engine.IndexFile;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;

/**
 * View for vector data.
 *
 * @author tomi
 */
public class VectorBrowserView extends ViewPart {

	Composite panel;
	Table table;
	Composite messagePanel;
	Label message;
	VirtualTable<OutputVectorEntry> viewer;
	ISelectionListener selectionChangedListener;
	

	@Override
	public void createPartControl(Composite parent) {
		panel = new Composite(parent, SWT.NONE);
		GridLayout layout = new GridLayout(1, false);
		layout.marginWidth = layout.marginHeight = 0;
		panel.setLayout(layout);
		createMessage(panel);
		createTable(panel);
		createTableViewer(table);
		hookSelectionChangedListener();
	}
	
	private void createMessage(Composite parent) {
		messagePanel = new Composite(parent, SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.exclude = true;
		messagePanel.setLayoutData(gridData);
		messagePanel.setLayout(new FillLayout());
		messagePanel.setBackground(ColorFactory.asColor("white"));
		message = new Label(messagePanel, SWT.WRAP);
		message.setBackground(ColorFactory.asColor("white"));
	}

	private void createTable(Composite parent) {
		// create pages
		table = new Table(parent, SWT.VIRTUAL | SWT.MULTI | SWT.FULL_SELECTION);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		table.setHeaderVisible(true);
		table.setLinesVisible(true);
		TableColumn tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(60);
		tableColumn.setText("Item#");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Time");
		tableColumn = new TableColumn(table, SWT.NONE);
		tableColumn.setWidth(140);
		tableColumn.setText("Value");
	}

	private void createTableViewer(Table table) {
		viewer = new VirtualTable<OutputVectorEntry>(table);
		viewer.setContentProvider(new VectorResultContentProvider());
		viewer.setLabelProvider(new VectorResultLabelProvider());
		setViewerInput(getSite().getPage().getSelection());
	}
	
	
	@Override
	public void dispose() {
		unhookSelectionChangedListener();
		super.dispose();
	}
	
	private void hookSelectionChangedListener() {
		selectionChangedListener = new ISelectionListener() {
			public void selectionChanged(IWorkbenchPart part, ISelection selection) {
				setViewerInput(selection);
			}
		};
		getSite().getPage().addPostSelectionListener(selectionChangedListener);
	}
	
	private void unhookSelectionChangedListener() {
		if (selectionChangedListener != null)
			getSite().getPage().removePostSelectionListener(selectionChangedListener);
	}
	
	@Override
	public void setFocus() {
		table.setFocus();
	}

	public void setViewerInput(ISelection selection) {
		VectorResult selectedVector = null;
		
		if (selection instanceof IStructuredSelection) {
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			if (structuredSelection.size() == 1) {
				Object selectedObject = structuredSelection.getFirstElement();
				if (selectedObject instanceof VectorResult) {
					selectedVector = (VectorResult)selectedObject;
				}
				else if (selectedObject instanceof IAdaptable) {
					selectedVector = (VectorResult)((IAdaptable)selectedObject).getAdapter(VectorResult.class);
				}
			}
		}
		
		if (selectedVector != viewer.getInput()) {
			viewer.setInput(selectedVector);
			// show message instead of empty table, when no index file
			checkInput(selectedVector);
		}
	}
	
	/**
	 * 
	 */
	private void checkInput(VectorResult input) {
		if (input != null) {
			String file = input.getFileRun().getFile().getFileSystemFilePath();
			if (IndexFile.isIndexFileUpToDate(file)) {
				setVisible(messagePanel, false);
				setVisible(table, true);
			} else {
				message.setText("The vector content can not be browsed," +
						" because the index for \""+file+"\" is not up to date.");
				setVisible(messagePanel, true);
				setVisible(table, false);
				
			}
		}
	}
	
	private void setVisible(Control control, boolean visible) {
		GridData gridData = (GridData)control.getLayoutData();
		gridData.exclude = !visible;
		control.setVisible(visible);
		panel.layout(true, true);
	}
}
