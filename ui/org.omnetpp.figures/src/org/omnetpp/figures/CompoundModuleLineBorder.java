/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LineBorder;
import org.eclipse.draw2d.geometry.Insets;

/**
 * A special border for compound modules
 *
 * @author rhornig
 */
public class CompoundModuleLineBorder extends LineBorder {
    @Override
    public void setWidth(int width) {
        // hack: LineBorder.setWidth() does not allow for zero width, although it is perfectly working
        // (see paint()) and we also need it. Use Integer.MAX_VALUE to mean zero.
        if (width <= 0)
            width = Integer.MAX_VALUE;
        super.setWidth(width);
    }

    @Override
    public int getWidth() {
        // other half of the setWidth() hack: return zero for Integer.MAX_VALUE
        int width = super.getWidth();
        return width==Integer.MAX_VALUE ? 0 : width;
    }

    /**
     * @see org.eclipse.draw2d.Border#paint(IFigure, Graphics, Insets)
     * copied from {@link LineBorder#paint(IFigure, Graphics, Insets)}
     */
    public void paint(IFigure figure, Graphics graphics, Insets insets) {
        if (getColor() != null)
            graphics.setForegroundColor(getColor());

        tempRect.setBounds(getPaintRectangle(figure, insets));
        if (getWidth() % 2 == 1) {
            tempRect.width--;
            tempRect.height--;
        }
        tempRect.shrink(getWidth() / 2, getWidth() / 2);
        graphics.setLineWidthFloat(getWidth()*(float)graphics.getAbsoluteScale());
        // fixing an issue if we are using 0 as width
        if(getWidth() > 0)
            graphics.drawRectangle(tempRect);
    }
}
