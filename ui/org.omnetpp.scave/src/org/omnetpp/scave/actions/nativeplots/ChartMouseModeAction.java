/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.nativeplots;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.canvas.ZoomableCanvasMouseSupport;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Switches mouse mode between zoom/pan for chart on the active chart page in the active Scave editor.
 *
 * @author andras
 */
public class ChartMouseModeAction extends AbstractScaveAction {
    private int destMode;

    public ChartMouseModeAction(int destMode) {
        super(IAction.AS_RADIO_BUTTON);
        this.destMode = destMode;
        if (destMode==ZoomableCanvasMouseSupport.PAN_MODE) {
            setText("Hand Tool");
            setDescription("Lets you move the chart using the mouse; hold down Ctrl for zooming.");
            setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_HAND));
        }
        else if (destMode==ZoomableCanvasMouseSupport.ZOOM_MODE) {
            setText("Zoom Tool");
            setDescription("Lets you zoom the chart using the mouse; use Shift for zooming out, or hold down Ctrl for moving the chart.");
            setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOM));
        }
        else {
            Assert.isTrue(false);
        }
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        PlotBase plot = scaveEditor.getActivePlot();
        if (isChecked() && plot != null) {
            plot.setMouseMode(destMode);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        PlotBase canvas = editor.getActivePlot();
        return canvas != null;
    }
}
