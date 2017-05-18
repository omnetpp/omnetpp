/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

/**
 * A delegating label provider that can be used with a GenericTreeNode
 * tree. It accepts a label provider written for the tree node <i>payload</i>
 * objects, and delegates everything to it.
 *
 * @author Andras
 */
public class GenericTreeLabelProvider implements ILabelProvider {
    private ILabelProvider labelProvider;

    public GenericTreeLabelProvider() {
        this(new LabelProvider());
    }

    public GenericTreeLabelProvider(ILabelProvider labelProvider) {
        this.labelProvider = labelProvider;
    }

    public Image getImage(Object element) {
        return labelProvider.getImage(((GenericTreeNode)element).getPayload());
    }

    public String getText(Object element) {
        return labelProvider.getText(((GenericTreeNode)element).getPayload());
    }

    public void addListener(ILabelProviderListener listener) {
        labelProvider.addListener(listener);
    }

    public void dispose() {
        labelProvider.dispose();
    }

    public boolean isLabelProperty(Object element, String property) {
        return labelProvider.isLabelProperty(((GenericTreeNode)element).getPayload(), property);
    }

    public void removeListener(ILabelProviderListener listener) {
        labelProvider.removeListener(listener);
    }
}
