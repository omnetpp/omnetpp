/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.eventlog.IEvent;

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

    public IAxisRenderer getRenderer(int index) {
        return axisRenderers[index];
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

    public void drawAxis(Graphics graphics, IEvent startEvent, IEvent endEvent) {
        axisRenderers[selectedRendererIndex].drawAxis(graphics, startEvent, endEvent);
    }
}
