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
import org.eclipse.jface.viewers.StructuredSelection;

/**
 * A default implementation of ISelectionProvider.
 *
 * @author tomi
 */
public class SelectionProvider implements ISelectionProvider
{
    protected ListenerList<ISelectionChangedListener> listeners = new ListenerList<ISelectionChangedListener>(ListenerList.IDENTITY);
    protected ISelection selection = new StructuredSelection();

    public SelectionProvider() {
    }

    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        listeners.add(listener);
    }

    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        listeners.remove(listener);
    }

    private void fireSelectionChanged(SelectionChangedEvent event) {
        for (ISelectionChangedListener listener : listeners)
            listener.selectionChanged(event);
    }

    public ISelection getSelection() {
        return selection;
    }

    /**
     * Note: we always notify (not only when the new selection is different
     * from the old one), in order to allow firing extra ("fake") notifications.
     */
    public void setSelection(ISelection selection) {
        this.selection = selection;
        fireSelectionChanged(new SelectionChangedEvent(this, selection));
    }
}
