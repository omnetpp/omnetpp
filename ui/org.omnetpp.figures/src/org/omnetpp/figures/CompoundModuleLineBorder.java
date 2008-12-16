/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.geometry.Insets;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleLineBorder extends LineBorder {
	/**
	 * @see org.eclipse.draw2d.Border#paint(IFigure, Graphics, Insets)
	 * copied from {@link LineBorder#paint(IFigure, Graphics, Insets)}
	 */
	public void paint(IFigure figure, Graphics graphics, Insets insets) {
		tempRect.setBounds(getPaintRectangle(figure, insets));
		if (getWidth() % 2 == 1) {
			tempRect.width--;
			tempRect.height--;
		}
		tempRect.shrink(getWidth() / 2, getWidth() / 2);
		graphics.setLineWidth(getWidth());
		if (getColor() != null)
			graphics.setForegroundColor(getColor());
		// fixing an issue if we are using 0 as width
		if(getWidth() > 0)
			graphics.drawRectangle(tempRect);
	}
}
