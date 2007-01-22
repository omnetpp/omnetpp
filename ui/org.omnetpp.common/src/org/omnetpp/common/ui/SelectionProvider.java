package org.omnetpp.common.ui;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;

public class SelectionProvider implements ISelectionProvider
{
	ListenerList listeners = new ListenerList(ListenerList.IDENTITY);
	ISelection selection;
	
	public SelectionProvider() {
	}
	
	public void addSelectionChangedListener(ISelectionChangedListener listener) {
		listeners.add(listener);
	}

	public void removeSelectionChangedListener(ISelectionChangedListener listener) {
		listeners.remove(listener);
	}
	
	private void fireSelectionChanged(SelectionChangedEvent event) {
		for (Object listener : listeners.getListeners())
			((ISelectionChangedListener)listener).selectionChanged(event);
	}

	public ISelection getSelection() {
		return selection;
	}

	public void setSelection(ISelection selection) {
		this.selection = selection;
		fireSelectionChanged(new SelectionChangedEvent(this, selection));
	}
}
