/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.runtime.ListenerList;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;

/**
 * A selection provider that delegates to another selection provider. One can switch
 * selection providers. Useful e.g. with multi-page editors.
 *
 * @author andras
 */
public class DelegatingSelectionProvider implements ISelectionProvider {
    private ListenerList<ISelectionChangedListener> listeners = new ListenerList<>();
    private ISelectionProvider currentSelectionProvider = null;

    private ISelectionChangedListener listener = new ISelectionChangedListener() {
        public void selectionChanged(SelectionChangedEvent event) {
            fireSelectionChanged(event);
        }
    };

    public DelegatingSelectionProvider() {
    }

    public void setSelectionProvider(ISelectionProvider selectionProvider) {
        if (currentSelectionProvider != null)
            currentSelectionProvider.removeSelectionChangedListener(listener);
        currentSelectionProvider = selectionProvider;
        currentSelectionProvider.addSelectionChangedListener(listener);

        SelectionChangedEvent e = new SelectionChangedEvent(selectionProvider, selectionProvider.getSelection());
        fireSelectionChanged(e);
    }

    private void fireSelectionChanged(SelectionChangedEvent event) {
        for (ISelectionChangedListener listener : listeners)
            listener.selectionChanged(event); //TODO SafeRunner, etc
    }

    public void setSelection(ISelection selection) {
        currentSelectionProvider.setSelection(selection);
    }

    public ISelection getSelection() {
        return currentSelectionProvider.getSelection();
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        listeners.add(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        listeners.remove(listener);
    }
}
