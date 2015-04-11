/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.draw2d.geometry.Transform;

/**
 * Various 2D geometry routines.
 *
 * @author tomi
 */
public class GeomUtils {

    public static Insets subtract(Rectangle outer, Rectangle inner) {
        return new Insets(Math.max(inner.y - outer.y, 0),
                  Math.max(inner.x - outer.x, 0),
                  Math.max(outer.bottom() - inner.bottom(), 0),
                  Math.max(outer.right() - inner.right(), 0));
    }

    public static Rectangle add(Rectangle rect, Insets insets) {
        return rect.getCopy().expand(insets);
    }

    public static Rectangle subtract(Rectangle rect, Insets insets) {
        return rect.getCopy().crop(insets);
    }

    /**
     * Calculates bounding box of a rotated rectangle. Rotation is in *degrees*.
     */
    public static Dimension rotatedSize(Dimension size, double rotation) {
        if (rotation == 0 || rotation == 180)  // avoid rounding errors in spec cases
            return size.getCopy();
        else if (rotation == 90 || rotation == 270)
            return new Dimension(size.height, size.width);
        else {
            Transform transform = new Transform();
            transform.setRotation(Math.toRadians(rotation));
            Point p1 = transform.getTransformed(new Point((size.width+1), (size.height+1)));
            Point p2 = transform.getTransformed(new Point((size.width+1), - (size.height+1)));
            return new Dimension(
                    Math.max(Math.abs(p1.x), Math.abs(p2.x)),
                    Math.max(Math.abs(p1.y), Math.abs(p2.y)));
        }
    }
}
