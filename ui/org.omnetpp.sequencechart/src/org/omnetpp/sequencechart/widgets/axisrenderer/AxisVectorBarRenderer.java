package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.sequencechart.widgets.SequenceChart;

/**
 * A special axis representation for sequence charts which displays a horizontal colored bar
 * with the names representing the individual values of a data vector.
 */
public class AxisVectorBarRenderer implements IAxisRenderer {
	protected static final Color AXIS_COLOR = ColorFactory.asColor("black");

	protected static final Font VALUE_NAME_FONT = new Font(null, "Courier New", 8, 0);

	protected static final Color VALUE_NAME_COLOR = ColorFactory.asColor("black");
	
	protected static final Color NO_VALUE_COLOR = ColorFactory.asColor("white");
	
	protected SequenceChart sequenceChart;

	protected XYArray data;

	// TODO: extract meta data
	private String[] names = new String[] {
			"IDLE",
	        "DEFER",
	        "WAITDIFS",
	        "BACKOFF",
	        "WAITACK",
	        "WAITBROADCAST",
	        "WAITCTS",
	        "WAITSIFS",
	        "RECEIVE",
	};

	public AxisVectorBarRenderer(SequenceChart sequenceChart, XYArray data) {
		this.sequenceChart = sequenceChart;
		this.data = data;
	}

	public int getHeight() {
		return 13;
	}

	/**
	 * Draws a colored tick bar based on the values in the data vector in the given range.
	 */
	public void drawAxis(Graphics graphics, double startSimulationTime, double endSimulationTime)
	{
		Rectangle rect = graphics.getClip(Rectangle.SINGLETON);
		int size = getDataLength();

		int startIndex = getIndex(startSimulationTime, true);
		if (startIndex == -1)
			startIndex = 0;

		int endIndex = getIndex(endSimulationTime, false);
		if (endIndex == -1)
			endIndex = size;

		// draw default color where no value is available
		graphics.setLineStyle(SWT.LINE_SOLID);
		graphics.setBackgroundColor(NO_VALUE_COLOR);
		graphics.fillRectangle(rect.x, 0, rect.right() - rect.x, getHeight());

		// draw axis as a colored thick line with labels representing value names
		// two phases: first for background, then value names
		for (int phase = 0; phase < 2; phase++)
			for (int i = startIndex; i < endIndex; i++) {
				double simulationTime = getSimulationTime(i);
				double nextSimulationTime = (i == size - 1) ? endSimulationTime : getSimulationTime(i + 1);

				int colorIndex = getValueIndex(i);
				graphics.setBackgroundColor(ColorFactory.getGoodColor(colorIndex));

				int x1 = sequenceChart.getViewportCoordinateForSimulationTime(simulationTime);
				int x2 = sequenceChart.getViewportCoordinateForSimulationTime(nextSimulationTime);

				if (phase == 0) {
					graphics.fillRectangle(x1, 0, x2 - x1, getHeight());
					graphics.setForegroundColor(AXIS_COLOR);
					graphics.drawLine(x1, 0, x1, getHeight());
				}

				// draw labels starting at each value change and repeat labels based on canvas width
				if (phase == 1) {
					String name = getValueName(i);
					int labelWidth = graphics.getFontMetrics().getAverageCharWidth() * name.length();
					if (x2 - x1 > labelWidth) {
						graphics.setForegroundColor(VALUE_NAME_COLOR);
						graphics.setFont(VALUE_NAME_FONT);

						int x = x1 + 3;
						while (x < rect.right() && x < x2 - labelWidth) {
							graphics.drawText(name, x, 0);
							x += sequenceChart.getClientArea().width;
						}
					}
				}
			}

		graphics.setForegroundColor(AXIS_COLOR);
		graphics.drawLine(rect.x, 0, rect.right(), 0);
		graphics.drawLine(rect.x, getHeight(), rect.right(), getHeight());
	}

	/**
	 * Returns the index having less or greater simulation time in the data array depending on the given flag.
	 */
	public int getIndex(double simulationTime, boolean before)
	{
		int index = -1;
		int left = 0;
		int right = getDataLength();

		while (left <= right) {
	        int mid = (int)Math.floor((right + left) / 2);

	        if (getSimulationTime(mid) == simulationTime) {
	        	do {
	        		if (before)
	        			mid--;
	        		else
	        			mid++;
	        	}
	        	while (mid >= 0 && mid < getDataLength() && getSimulationTime(mid) == simulationTime);

	        	index = mid;
	        	break;
	        }
            else if (simulationTime < getSimulationTime(mid))
	            right = mid - 1;
	        else
	            left = mid + 1;
		}

		if (left > right)
			if (before)
				if (simulationTime < getSimulationTime(left))
					index = left - 1;
				else
					index = left;
			else
				if (simulationTime > getSimulationTime(right))
					index = right + 1;
				else
					index = right;

		if (index < 0 || index >= getDataLength())
			return -1;
		else {
			Assert.isTrue((before && getSimulationTime(index) < simulationTime) ||
						  (!before && getSimulationTime(index) > simulationTime));
			return index;
		}
	}

	public int getDataLength()
	{
		return data.length();
	}

	public double getSimulationTime(int index)
	{
		return data.getX(index);
	}

	public double getValue(int index)
	{
		return data.getY(index);
	}

	private int getValueIndex(int index)
	{
		return (int)Math.floor(getValue(index));
	}

	private String getValueName(int index)
	{
		return names[getValueIndex(index)];
	}
}