package org.omnetpp.scave2.views;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.ISelectionListener;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.part.ViewPart;
import org.omnetpp.common.virtualtable.VirtualTable;
import org.omnetpp.scave.engine.OutputVectorEntry;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave2.editors.datatable.VectorResultContentProvider;
import org.omnetpp.scave2.editors.datatable.VectorResultLabelProvider;

/**
 * View for vector data.
 *
 * @author tomi
 */
public class VectorBrowserView extends ViewPart {

	Table table;
	VirtualTable<OutputVectorEntry> viewer;
	ISelectionListener selectionChangedListener;
	

	@Override
	public void createPartControl(Composite parent) {
		createTable(parent);
		createTableViewer(table);
		hookSelectionChangedListener();
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
		// TODO Auto-generated method stub

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
				else {
					selectedVector = (VectorResult)Platform.getAdapterManager().getAdapter(selectedObject, VectorResult.class);
				}
			}
		}
		
		if (selectedVector != viewer.getInput()) {
			viewer.setInput(selectedVector);
		}
	}
}
