/*--------------------------------------------------------------*
  Copyright (C) 2006-2024 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.nativeplots;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Sets the viewport of the chart of the active chart page in the active Scave editor,
 * such that it nicely encompasses all plotted data and the origin.
 *
 * @author attila
 */
public class RestoreOriginalViewAction extends AbstractScaveAction {

    public RestoreOriginalViewAction() {
        setText("Restore Original View");
        setToolTipText("Restore Original View");
        ImageDescriptor imageDescriptor = PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_ETOOL_HOME_NAV);
        setImageDescriptor(imageDescriptor);
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Assert.isTrue(isEnabled());
        PlotBase plot = scaveEditor.getActivePlot();
        if (plot != null)
            plot.zoomToFit();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        PlotBase plot = editor.getActivePlot();
        return plot != null;
    }
}
