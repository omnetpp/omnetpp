package org.omnetpp.figures.layout;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;


public class FreeformDesktopLayout extends DesktopLayout {

    /**
     * Returns the point (0,0) as the origin. This is required for correct freeform handling.
     * @see XYLayout#getOrigin(IFigure)
     */
    @Override
    public Point getOrigin(IFigure figure) {
        return new Point();
    }

}
