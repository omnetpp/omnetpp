/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.panel;

import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.Viewer;

import org.omnetpp.scave.engine.IDList;

/**
 * This implementation of <code>IStructuredContentProvider</code> handles
 * the case where the viewer input is a org.omnetpp.scalars.engine.IntVector.
 */
public class IDListContentProvider implements IStructuredContentProvider {

    /**
     * Converts the input IntVector into Integer[]
     */
    public Object[] getElements(Object inputElement) {
        IDList vec = (IDList)inputElement;
        int sz = (int)vec.size();
        Object[] array = new Long[sz];
        for (int i=0; i<sz; i++)
            array[i] = Long.valueOf(vec.get(i));
        return array;
    }

    /**
     * This implementation does nothing.
     */
    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        // do nothing.
    }

    /**
     * This implementation does nothing.
     */
    public void dispose() {
        // do nothing.
    }
}
