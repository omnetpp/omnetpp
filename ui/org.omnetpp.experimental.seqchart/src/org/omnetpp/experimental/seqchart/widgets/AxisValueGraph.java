package org.omnetpp.experimental.seqchart.widgets;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;

public class AxisValueGraph extends AxisGraph {
	private static final Font VALUE_NAME_FONT = new Font(null, "Courier New", 8, 0);
	private static final Color VALUE_NAME_COLOR = ColorFactory.asColor("black");
	private static final Color NO_VALUE_COLOR = ColorFactory.asColor("white");
	private static final Color TOO_MANY_VALUES_COLOR = ColorFactory.asColor("black");

	// TODO: add more colors
	private static final Color[] colors = {
			ColorFactory.asColor("green"),
			ColorFactory.asColor("red"),
			ColorFactory.asColor("yellow"),
			ColorFactory.asColor("purple"),
			ColorFactory.asColor("cyan"),
			ColorFactory.asColor("pink"),
			ColorFactory.asColor("brown"),
			ColorFactory.asColor("gray"),
	};

	private XYArray data;

	// TODO: extract meta data
	private String[] names = new String[] {
			"IDLE",
	        "DEFER",
	        "WAITDIFS",
	        "BACKOFF",
	        "TRANSMITTING",
	        "RECEIVING",
	        "WAITSIFS"
	};
	
	public AxisValueGraph(SequenceChart sequenceChart, XYArray data) {
		super(sequenceChart);
		this.data = data;
	}

	public int getHeight() {
		return 13;
	}
	
	/**
	 * Paints a colored tick line based on the values in the data vector in the given range. 
	 */
	public void paintAxis(Graphics graphics, double startSimulationTime, double endSimulationTime)
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
				Color color = (colorIndex >= colors.length) ? TOO_MANY_VALUES_COLOR : colors[colorIndex];
				graphics.setBackgroundColor(color);
				
				int x1 = sequenceChart.simulationTimeToPixel(simulationTime);
				int x2 = sequenceChart.simulationTimeToPixel(nextSimulationTime);
				
				if (phase == 0) {
					graphics.fillRectangle(x1, 0, x2 - x1, getHeight());
					graphics.setForegroundColor(AXIS_COLOR);
					graphics.drawLine(x1, 0, x1, getHeight());
				}
	
				// draw labels starting at each value change
				// TODO: measure name and compare length to that
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
	 * Returns the index having the same simulation time in the data array.
	 * If there's no such element, then return the one before or after depending on the flag.
	 */
	private int getIndex(double simulationTime, boolean before)
	{
		int index;
		int left = 0;
		int right = getDataLength();

		while (left <= right) {
	        int mid = (int)Math.floor((right-left)/2) + left;

	        if (getSimulationTime(mid) == simulationTime)
	            return mid;
            else if (simulationTime < getSimulationTime(mid))
	            right = mid - 1;
	        else
	            left = mid + 1;
		}

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
		else
			return index;
	}
	
	private int getDataLength()
	{
		return data.length();
	}
	
	private double getSimulationTime(int index)
	{
		return data.getX(index);
	}
	
	private double getValue(int index)
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