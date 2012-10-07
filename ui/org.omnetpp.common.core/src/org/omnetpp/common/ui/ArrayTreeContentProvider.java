package org.omnetpp.common.ui;

import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;

/**
 * Makes it possible to put a flat list or array into a TreeViewer, if that's
 * what you happen to need.
 *
 * @author Andras
 */
public class ArrayTreeContentProvider extends ArrayContentProvider implements ITreeContentProvider {

    public Object[] getChildren(Object parentElement) {
        return new Object[0];
    }

    public Object getParent(Object element) {
        return null;
    }

    public boolean hasChildren(Object element) {
        return false;
    }

}
