/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.TreeViewer;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;

/**
 * TODO
 *
 * @author andras
 */
public class CollapseTreeAction extends Action {
    private TreeViewer tree;

    public CollapseTreeAction(TreeViewer tree) {
        setText("Collapse All");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_COLLAPSE));
        this.tree = tree;
    }

    @Override
    public void run() {
        tree.collapseAll();
    }
}
