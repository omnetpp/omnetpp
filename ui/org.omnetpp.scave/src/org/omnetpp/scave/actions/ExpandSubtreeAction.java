/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.TreeViewer;
import org.omnetpp.scave.ScavePlugin;

/**
 * TODO
 * 
 * @author andras
 */
public class ExpandSubtreeAction extends Action {
    private TreeViewer tree;

    public ExpandSubtreeAction(TreeViewer tree) {
        setText("Expand");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/expand.png"));
        this.tree = tree;
    }

    @Override
    public void run() {
        Object[] elements = tree.getStructuredSelection().toArray();
        for (Object element : elements)
            tree.expandToLevel(element, TreeViewer.ALL_LEVELS);
    }
}
