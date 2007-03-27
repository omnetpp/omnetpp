package org.omnetpp.sequencechart.widgets.axisrenderer;

import org.eclipse.draw2d.Graphics;

/**
 * An axis renderer draws the visual representation of a sequence chart axis.
 */
public interface IAxisRenderer {
	/**
	 * The height in pixels of the visual representation to be reserved for this axis.
	 */
	public int getHeight();

	/**
	 * Does the actual drawing lazily on the given graphics in the provided simulation time interval.
	 */
	public void drawAxis(Graphics graphics, double startSimulationTime, double endSimulationTime);
}