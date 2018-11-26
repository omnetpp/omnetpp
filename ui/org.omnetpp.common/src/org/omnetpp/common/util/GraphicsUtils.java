package org.omnetpp.common.util;

import java.lang.reflect.Field;

import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.SWTGraphics;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gmf.runtime.draw2d.ui.render.awt.internal.svg.export.GraphicsSVG;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.omnetpp.common.IConstants;

@SuppressWarnings("restriction")
public class GraphicsUtils {

    /**
     * Measures the given string in pixels.
     */
    public static Point getTextExtent(Graphics graphics, String string) {
        if (graphics instanceof GraphicsSVG) {
            java.awt.Graphics g = ((GraphicsSVG)graphics).getSVGGraphics2D();
            java.awt.geom.Rectangle2D r = g.getFontMetrics().getStringBounds(string, g);
            return new Point((int)Math.ceil(r.getWidth()), (int)Math.ceil(r.getHeight()));
        }
        else if (graphics instanceof SWTGraphics) {
            try {
                SWTGraphics g = (SWTGraphics)graphics;
                Class<SWTGraphics> cls = SWTGraphics.class;
                Field field = cls.getDeclaredField("gc");
                field.setAccessible(true);
                GC gc = (GC)field.get(g);
                // KLUDGE: what a hack to send the font down to GC?!
                g.drawText("", 0, 0);
                return gc.textExtent(string);
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
        else {
            FontMetrics fontMetrics = graphics.getFontMetrics();
            return new Point(fontMetrics.getAverageCharWidth() * string.length(), fontMetrics.getHeight());
        }
    }

    public static Rectangle getClip(Graphics graphics) {
        return graphics.getClip(new Rectangle());
    }

    public static boolean isSVGGraphics(Graphics graphics) {
        return graphics instanceof GraphicsSVG;
    }
}
