/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Set view mode of the Charts page.
 *
 * @author andras
 */
public class ViewModeAction extends AbstractScaveAction {
    private IconGridViewer.ViewMode mode;

    public ViewModeAction(IconGridViewer.ViewMode mode, String text, String imagePath) {
        setText(text);
        setImageDescriptor(ScavePlugin.getImageDescriptor(imagePath));
        this.mode = mode;
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        scaveEditor.getChartsPage().getViewer().setViewMode(mode);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}
