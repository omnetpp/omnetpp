/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.core.commands.common.EventManager;
import org.eclipse.jface.util.SafeRunnable;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProviderChangedEvent;
import org.eclipse.swt.graphics.Image;

/**
 * Same as org.eclipse.jface.viewers.LabelProvider, but for tables.
 *
 * @author Tomi
 */
public class TableLabelProvider extends EventManager implements ITableLabelProvider {

    /**
     * Creates a new label provider.
     */
    public TableLabelProvider() {
    }

    /* (non-Javadoc)
     * Method declared on IBaseLabelProvider.
     */
    public void addListener(ILabelProviderListener listener) {
        addListenerObject(listener);
    }

    /**
     * The <code>TableLabelProvider</code> implementation of this
     * <code>IBaseLabelProvider</code> method does nothing. Subclasses may extend.
     */
    public void dispose() {
    }

    /**
     * Fires a label provider changed event to all registered listeners
     * Only listeners registered at the time this method is called are notified.
     *
     * @param event a label provider changed event
     *
     * @see ILabelProviderListener#labelProviderChanged
     */
    protected void fireLabelProviderChanged(final LabelProviderChangedEvent event) {
        Object[] listeners = getListeners();
        for (int i = 0; i < listeners.length; ++i) {
            final ILabelProviderListener l = (ILabelProviderListener) listeners[i];
            SafeRunnable.run(new SafeRunnable() {
                public void run() {
                    l.labelProviderChanged(event);
                }
            });
        }
    }

    /**
     * The <code>TableLabelProvider</code> implementation of this
     * <code>ITableLabelProvider</code> method returns <code>null</code>. Subclasses may
     * override.
     */
    public Image getColumnImage(Object element, int columnIndex) {
        // TODO Auto-generated method stub
        return null;
    }

    /**
     * The <code>TableLabelProvider</code> implementation of this
     * <code>ITableLabelProvider</code> method returns the element's <code>toString</code>
     * string. Subclasses may override.
     */
    public String getColumnText(Object element, int columnIndex) {
        return element == null ? "" : element.toString();//$NON-NLS-1$
    }

    /**
     * The <code>TableLabelProvider</code> implementation of this
     * <code>IBaseLabelProvider</code> method returns <code>true</code>. Subclasses may
     * override.
     */
    public boolean isLabelProperty(Object element, String property) {
        return true;
    }

    /* (non-Javadoc)
     * Method declared on IBaseLabelProvider.
     */
    public void removeListener(ILabelProviderListener listener) {
        removeListenerObject(listener);
    }
}
