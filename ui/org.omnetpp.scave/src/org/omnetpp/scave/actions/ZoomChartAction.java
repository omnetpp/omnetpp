/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Zooms in/out the chart of the active chart page in the active Scave editor.
 *
 * @author tomi
 */
public class ZoomChartAction extends AbstractScaveAction {
    private double zoomFactor;
    private boolean horizontally;
    private boolean vertically;

    public ZoomChartAction(boolean horizontally, boolean vertically, double zoomFactor) {
        this.horizontally = horizontally;
        this.vertically = vertically;
        this.zoomFactor = zoomFactor;

        boolean both = horizontally && vertically;
        String inout = (zoomFactor == 0.0 ? "To Fit" :
                        zoomFactor > 1.0 ? "In" : "Out");
        //String dir = both ? "" : (horizontally ? " X" : " Y");
        String dir2 = both ? "" : (horizontally ? " Horizontally" : " Vertically");
        setText("Zoom " +  inout + dir2);
        setDescription("Zoom " + inout.toLowerCase() + " Chart " + dir2.toLowerCase());

        String imageId =
            zoomFactor == 0.0 ? ScaveImages.IMG_ETOOL16_ZOOMTOFIT :
            zoomFactor > 1.0 ? (both ? ScaveImages.IMG_ETOOL16_ZOOMIN : horizontally ? ScaveImages.IMG_ETOOL16_HZOOMIN : ScaveImages.IMG_ETOOL16_VZOOMIN) :
                (both ? ScaveImages.IMG_ETOOL16_ZOOMOUT : horizontally ? ScaveImages.IMG_ETOOL16_HZOOMOUT : ScaveImages.IMG_ETOOL16_VZOOMOUT);
        setImageDescriptor(ScavePlugin.getImageDescriptor(imageId));
    }


    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        Assert.isTrue(isEnabled());
        PlotBase plot = scaveEditor.getActivePlot();
        if (plot != null) {
            if (horizontally) {
                if (zoomFactor == 0.0)
                    plot.zoomToFitX();
                else
                    plot.zoomXBy(zoomFactor);
            }
            if (vertically) {
                if (zoomFactor == 0.0)
                    plot.zoomToFitY();
                else
                    plot.zoomYBy(zoomFactor);
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        PlotBase plot = editor.getActivePlot();
        if (plot != null) {
            boolean res = zoomFactor > 1.0 && horizontally && plot.getZoomX() < plot.getMaxZoomX() ||
                   zoomFactor > 1.0 && vertically && plot.getZoomY() < plot.getMaxZoomY() ||
                   zoomFactor < 1.0 && horizontally && plot.getMinZoomX() < plot.getZoomX() ||
                   zoomFactor < 1.0 && vertically && plot.getMinZoomY() < plot.getZoomY();
           return res;
        }
        return false;
    }
}
