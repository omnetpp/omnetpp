package org.omnetpp.ned.editor.graph.figures.properties;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

public interface DisplayBackgroundSupport {

	enum ImageArrangement {Scretch, Center, Tile }
	
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
}
