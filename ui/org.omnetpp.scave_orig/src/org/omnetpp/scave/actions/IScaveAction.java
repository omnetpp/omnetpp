/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.ui.ISelectionListener;

/**
 * Interface for actions in Scave.
 * @author andras
 */
public interface IScaveAction extends IAction {
    /**
     * To automatically enable/disable the action based on the selection,
     * this method needs to be called from a JFace {@link ISelectionChangedListener}
     * or the selection service's {@link ISelectionListener}, and
     * this method should be implemented to enable/disable the action accordingly.
     */
    public void selectionChanged(ISelection selection);

    /**
     * When set, the action will operate on the selection of the viewer,
     * and not on the selection service's selection.
     */
    public void setViewer(Viewer viewer);
}
