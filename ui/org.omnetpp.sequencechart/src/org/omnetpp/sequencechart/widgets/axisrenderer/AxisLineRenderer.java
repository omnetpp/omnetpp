package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.eventlog.ModuleTreeItem;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * Default axis representation for sequence charts is a solid 1 pixel thick line.
 */
public class AxisLineRenderer implements IAxisRenderer {
	protected SequenceChart sequenceChart;

    protected ModuleTreeItem module;
	
	public AxisLineRenderer(SequenceChart sequenceChart, ModuleTreeItem module) {
		this.sequenceChart = sequenceChart;
		this.module = module;
	}

	public int getHeight() {
	    return module.isCompoundModule() ? 3 : 1;
	}
	
	public void drawAxis(Graphics graphics, long startEventPtr, long endEventPtr)
	{
		Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
		graphics.setLineStyle(SWT.LINE_SOLID);

        graphics.setForegroundColor(module.isCompoundModule() ? ColorFactory.DIM_GREY : ColorFactory.BLACK);
        graphics.drawLine(rect.x, 0, rect.right(), 0);
		
		if (module.isCompoundModule()) {
            graphics.drawLine(rect.x, 2, rect.right(), 2);
            graphics.setForegroundColor(ColorFactory.WHITE);
            graphics.drawLine(rect.x, 1, rect.right(), 1);
		}
	}
}
