/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;

public class AxisMultiRenderer implements IAxisRenderer {
    private IAxisRenderer[] axisRenderers;
    private int selectedRendererIndex = 0;

    public AxisMultiRenderer(IAxisRenderer[] axisRenderers, int selectedRendererIndex) {
        this.axisRenderers = axisRenderers;
        this.selectedRendererIndex = selectedRendererIndex;
    }

    public int getRendererCount() {
        return axisRenderers.length;
    }

    public int getSelectedRendererIndex() {
        return selectedRendererIndex;
    }

    public void setSelectedRendererIndex(int selectedRendererIndex) {
        this.selectedRendererIndex = selectedRendererIndex;
    }

    public int getHeight() {
        return axisRenderers[selectedRendererIndex].getHeight();
    }

    public void drawAxis(Graphics graphics, long startEventPtr, long endEventPtr) {
        axisRenderers[selectedRendererIndex].drawAxis(graphics, startEventPtr, endEventPtr);
    }
}
