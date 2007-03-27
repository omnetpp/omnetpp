package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * Default axis representation for sequence charts is a solid 1 pixel thick line.
 */
public class AxisLineRenderer implements IAxisRenderer {
	protected static final Color AXIS_COLOR = ColorFactory.asColor("black");
	
	protected SequenceChart sequenceChart;
	
	public AxisLineRenderer(SequenceChart sequenceChart) {
		this.sequenceChart = sequenceChart;
	}

	public int getHeight() {
		return 1;
	}
	
	public void drawAxis(Graphics graphics, double startSimulationTime, double endSimulationTime)
	{
		Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setForegroundColor(AXIS_COLOR);
		graphics.drawLine(rect.x, 0, rect.right(), 0);
	}
}