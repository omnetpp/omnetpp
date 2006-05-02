package org.omnetpp.ned.editor.graph.figures.properties;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

public interface DisplayBackgroundSupport {

	enum ImageArrangement {FIXED, SCRETCH, TILED }
	
	/**
	 * Adjusts compound module background parameteres
	 * @param img Background image
	 * @param arrange 
	 * @param backgroundColor
	 * @param borderColor
	 * @param borderWidth
	 */
	public void setBackgorund(Image img, ImageArrangement arrange, 
			Color backgroundColor, Color borderColor, int borderWidth);
	
	/**
	 * @param tickDistance Maximum distance between two ticks measured in pixels
	 * @param noOfTics Number of minor ticks between two major one
	 * @param gridColor Grid color
	 */
	public void setGrid(int tickDistance, int noOfTics, Color gridColor);
}
