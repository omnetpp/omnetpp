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
				  Math.max(outer.y + outer.height - inner.y - inner.height, 0),
				  Math.max(outer.x + outer.width - inner.x - inner.width, 0));
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
		Transform transform = new Transform();
		transform.setRotation(Math.toRadians(rotation));
		Point p1 = transform.getTransformed(new Point(size.width / 2, size.height / 2));
		Point p2 = transform.getTransformed(new Point(size.width / 2, - size.height / 2));
		return new Dimension(
				Math.max(Math.abs(p1.x), Math.abs(p2.x)),
				Math.max(Math.abs(p1.y), Math.abs(p2.y)));
	}
}
