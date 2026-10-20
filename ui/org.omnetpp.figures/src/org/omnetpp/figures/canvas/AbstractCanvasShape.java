package org.omnetpp.figures.canvas;


import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionRectangle;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.LineAttributes;
import org.eclipse.swt.graphics.Path;
import org.eclipse.swt.widgets.Display;

/**
 * Adds some functionality to {@link AbstractCanvasFigure}, very similarly to
 * what {@link org.eclipse.draw2d.Shape} does to
 * {@link org.eclipse.draw2d.Figure}.
 *
 * @author attila
 */
public abstract class AbstractCanvasShape extends AbstractCanvasFigure {

    private static final float MIN_LINE_HIT_TOLERANCE = 5;

    // only used for hit testing, not for real drawing
    private static final GC dummyGC = new GC(Display.getDefault());

    protected Path path;

    public enum LineStyle { LINE_SOLID, LINE_DASHED, LINE_DOTTED }
    public enum JoinStyle { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND }
    public enum FillRule { FILL_EVENODD, FILL_NONZERO}

    protected boolean outline = true;
    protected boolean fill = false;
    protected float lineWidth = 1;
    protected boolean zoomLineWidth = false;
    protected double lineOpacity = 1.0;
    protected double fillOpacity = 1.0;
    protected boolean clipChildren = false;

    LineStyle style = LineStyle.LINE_SOLID;
    JoinStyle join = JoinStyle.JOIN_BEVEL;

    @Override
    protected boolean hitTest(Point point) {
        LineAttributes attributes = new LineAttributes(Math.max(getLineWidth(), MIN_LINE_HIT_TOLERANCE));
        dummyGC.setLineAttributes(attributes);

        boolean onFill = path.contains((float)point.preciseX(), (float)point.preciseY(), dummyGC, false);
        boolean onOutline = path.contains((float)point.preciseX(), (float)point.preciseY(), dummyGC, true);

        return (getFill() && (getLocalBackgroundColor() != null) && onFill)
                || (getOutline() && (getLocalForegroundColor() != null) && onOutline);
    }

    protected void fillShape(Graphics graphics) {
        graphics.fillPath(path);
    }

    protected void outlineShape(Graphics graphics) {
        graphics.drawPath(path);
    }

    @Override
    public void paintFigure(Graphics graphics) {
        if (lineWidth > 0) { // 0 is invalid for dash pattern
            switch (style) {
                case LINE_DASHED:
                    graphics.setLineStyle(SWT.LINE_CUSTOM);
                    graphics.setLineDash(new float[] {3 * getZoomedLineWidth(), 3 * getZoomedLineWidth()});
                    graphics.setLineDashOffset(0);
                    break;
                case LINE_DOTTED:
                    graphics.setLineStyle(SWT.LINE_CUSTOM);
                    graphics.setLineDash(new float[] {1 * getZoomedLineWidth(), 2 * getZoomedLineWidth()});
                    graphics.setLineDashOffset(0);
                    break;
                default:
                    graphics.setLineStyle(SWT.LINE_SOLID);
            }

            graphics.setLineWidthFloat(getZoomedLineWidth());
        }

        switch (join) {
        case JOIN_BEVEL:
            graphics.setLineJoin(SWT.JOIN_BEVEL);
            break;
        case JOIN_MITER:
            graphics.setLineJoin(SWT.JOIN_MITER);
            break;
        case JOIN_ROUND:
            graphics.setLineJoin(SWT.JOIN_ROUND);
            break;
        }


        // these are a bit redundant, but don't cause problems, so better be safe.
        // (also, LineDecoration fill color always stays black without these.)
        if (getLocalForegroundColor() != null) {
            graphics.setForegroundColor(getLocalForegroundColor());
        }

        if (getLocalBackgroundColor() != null) {
            graphics.setBackgroundColor(getLocalBackgroundColor());
        }

        if (fill) {
            graphics.setAlpha((int)Math.round(fillOpacity * 255.0));
            fillShape(graphics);
        }

        if (outline && (lineWidth > 0)) {
            graphics.setAlpha((int)Math.round(lineOpacity * 255.0));
            outlineShape(graphics); // Graphics seems to draw a fine line even with 0 width
        }
    }

    /**
     * The clip region is the interior of the shape (without the outline);
     * see the superclass method for why it is approximated with a rectangle.
     */
    @Override
    protected Rectangle getChildClipBounds() {
        if (!clipChildren)
            return null;
        float[] b = new float[4];
        path.getBounds(b);
        PrecisionRectangle transformed = getCascadedTransform().applyTo(new PrecisionRectangle(b[0], b[1], b[2], b[3]));
        // round outwards, to make sure that nothing inside the region gets cut off
        int left = (int)Math.floor(transformed.preciseX());
        int top = (int)Math.floor(transformed.preciseY());
        int right = (int)Math.ceil(transformed.preciseX() + transformed.preciseWidth());
        int bottom = (int)Math.ceil(transformed.preciseY() + transformed.preciseHeight());
        return new Rectangle(left, top, right - left, bottom - top);
    }

    @Override
    public Rectangle getGraphicalBounds() {
        float[] bounds = new float[4];
        path.getBounds(bounds);
        PrecisionRectangle boundingRectangle = new PrecisionRectangle(bounds[0], bounds[1], bounds[2], bounds[3]);
        return boundingRectangle.expand(getZoomedLineWidth(), getZoomedLineWidth());
    }

    public boolean getOutline() {
        return outline;
    }

    public void setOutline(boolean outline) {
        if (this.outline != outline) {
            this.outline = outline;
            repaint();
        }
    }

    public boolean getFill() {
        return fill;
    }

    public void setFill(boolean fill) {
        if (this.fill != fill) {
            this.fill = fill;
            repaint();
        }
    }

    public double getFilOpacity() {
        return fillOpacity;
    }

    public void setFillOpacity(double opacity) {
        opacity = Math.min(Math.max(opacity, 0), 1); // clamping
        if (fillOpacity != opacity) {
            fillOpacity = opacity;
            repaint();
        }
    }

    public double getLineOpacity() {
        return lineOpacity;
    }

    public void setLineOpacity(double opacity) {
        opacity = Math.min(Math.max(opacity, 0), 1); // clamping
        if (lineOpacity != opacity) {
            lineOpacity = opacity;
            repaint();
        }
    }

    public float getZoomedLineWidth() {
        return zoomLineWidth ? getLineWidth() : getLineWidth() / (float)getScale();
    }

    public float getLineWidth() {
        return lineWidth;
    }

    public void setLineWidth(float w) {
        if (w != lineWidth) {
            erase();

            lineWidth = w;

            recomputeBounds();
            repaint();
            revalidate();
        }
    }

    public boolean getZoomLineWidth() {
        return zoomLineWidth;
    }

    public void setZoomLineWidth(boolean z) {
        if (z != zoomLineWidth) {
            erase();

            zoomLineWidth = z;

            recomputeBounds();
            repaint();
            revalidate();
        }
    }

    public LineStyle getLineStyle() {
        return style;
    }

    public void setLineStyle(LineStyle s) {
        if (style != s) {
            style = s;
            repaint();
        }
    }

    public JoinStyle getJoinStyle() {
        return join;
    }

    public boolean getClipChildren() {
        return clipChildren;
    }

    public void setClipChildren(boolean clipChildren) {
        if (this.clipChildren != clipChildren) {
            erase();

            this.clipChildren = clipChildren;

            recomputeBounds();
            repaint();
            revalidate();
        }
    }

    public void setJoinStyle(JoinStyle j) {
        if (join != j) {
            join = j;
            repaint();
        }
    }
}