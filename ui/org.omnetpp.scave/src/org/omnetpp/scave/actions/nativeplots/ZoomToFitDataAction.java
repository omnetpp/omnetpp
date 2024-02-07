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
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.charting.LinePlot;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Sets the viewport of the chart of the active chart page in the active Scave editor,
 * such that it nicely encompasses all plotted data.
 *
 * @author attila
 */
public class ZoomToFitDataAction extends AbstractScaveAction {

    public ZoomToFitDataAction() {
        setText("Zoom to Fit Data");
        setToolTipText("Zoom to Fit Data");
        ImageDescriptor imageDescriptor = ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_ZOOMTOFIT);
        setImageDescriptor(imageDescriptor);
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Assert.isTrue(isEnabled());
        LinePlot plot = (LinePlot)scaveEditor.getActivePlot();
        if (plot != null)
            plot.zoomToFitData();
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        PlotBase plot = editor.getActivePlot();
        return plot instanceof LinePlot;
    }
}
