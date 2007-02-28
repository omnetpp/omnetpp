package org.omnetpp.common.virtualtable;

import org.eclipse.jface.viewers.ContentViewer;
import org.eclipse.jface.viewers.IBaseLabelProvider;
import org.eclipse.jface.viewers.IContentProvider;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

public class VirtualTableViewer<T> extends ContentViewer {
	protected VirtualTable<T> virtualTable;

	public VirtualTableViewer(Composite parent) {
		virtualTable = new VirtualTable<T>(parent, SWT.NONE);
	}

	public VirtualTable<T>  getVirtualTable() {
		return virtualTable;
	}

	@Override
	public Control getControl() {
		return virtualTable;
	}

	@Override
	public ISelection getSelection() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void refresh() {
		virtualTable.redraw();
	}

	@Override
	public void setSelection(ISelection selection, boolean reveal) {
		// TODO Auto-generated method stub	
	}
	
	@Override
	public void setInput(Object input) {
		super.setInput(input);
		virtualTable.setInput(input);
	}
	
	@Override
	@SuppressWarnings("unchecked")
	public void setContentProvider(IContentProvider contentProvider) {
		super.setContentProvider(contentProvider);
		virtualTable.setContentProvider((IVirtualTableContentProvider)contentProvider);
	}

	@Override
	public void setLabelProvider(IBaseLabelProvider labelProvider) {
		throw new RuntimeException("Not supported");
	}
	
	public IVirtualTableLineRenderer<T> getLineRenderer() {
		return virtualTable.getLineRenderer();
	}

	public void setLineRenderer(IVirtualTableLineRenderer<T> lineRenderer) {
		virtualTable.setLineRenderer(lineRenderer);
	}
}
