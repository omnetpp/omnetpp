package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;

public class AxisGraph {
	protected static final Color AXIS_COLOR = ColorFactory.asColor("black");
	
	protected SequenceChart sequenceChart;
	
	public AxisGraph(SequenceChart sequenceChart) {
		this.sequenceChart = sequenceChart;
	}

	public int getHeight() {
		return 1;
	}
	
	public void paintAxis(Graphics graphics, double startSimulationTime, double endSimulationTime)
	{
		Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setForegroundColor(AXIS_COLOR);
		graphics.drawLine(rect.x, 0, rect.right(), 0);
	}
}