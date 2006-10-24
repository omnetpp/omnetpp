package org.omnetpp.common.util;

import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.swt.graphics.Rectangle;

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
		return new Rectangle(
				rect.x - insets.left,
				rect.y - insets.top,
				rect.width + insets.getWidth(),
				rect.height + insets.getHeight());
	}
	
	public static Rectangle subtract(Rectangle rect, Insets insets) {
		return new Rectangle(
				rect.x + insets.left,
				rect.y + insets.top,
				rect.width - insets.getWidth(),
				rect.height - insets.getHeight());
	}
}
