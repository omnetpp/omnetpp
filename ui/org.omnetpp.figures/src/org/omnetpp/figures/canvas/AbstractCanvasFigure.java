package org.omnetpp.figures.canvas;

import java.util.List;

import org.eclipse.draw2d.Figure;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.TreeSearch;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.draw2d.geometry.PrecisionRectangle;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.Debug;
import org.omnetpp.figures.misc.FigureUtils;
import org.omnetpp.figures.misc.IComparableFigure;
import org.omnetpp.figures.misc.IFigureSortingParent;
import org.omnetpp.figures.misc.Transform;

/**
 * The root superclass of every CanvasFigure. Handles correct z-order drawing,
 * selection hit testing, and transformation.
 *
 * @author attila
 */
public abstract class AbstractCanvasFigure extends Figure implements IComparableFigure, IFigureSortingParent {
    private double scale = 1; // for zooming
    private double zIndex = 0;
    private int ordinal = 0; // ordinal position of this @figure among sibling figures

    // the Figure's isVisible() and setVisible() can't be used here, because that also disables selection
    private boolean isVisible = true;
    protected Transform transform = new Transform();

    @Override
    @SuppressWarnings("unchecked")
    public void add(IFigure figure, java.lang.Object constraint, int index) {
        super.add(figure, constraint, index);
        // getChildren() says the returned list is not modifiable. Well, it is.
        // An alternative would be to remove every child and add them in the
        // right order, but that would be a bit more tedious.
        FigureUtils.sortFigures(getChildren()); // HACK
    }

    public boolean isCanvasFigureVisible() {
        return isVisible;
    }

    public void setCanvasFigureVisible(boolean isVisible) {
        if (this.isVisible != isVisible) {
            this.isVisible = isVisible;

            if (isVisible) {
                repaint();
            } else {
                erase();
            }
        }
    }

    public double getScale() {
        return scale;
    }

    public void setScale(double scale) {
        if (this.scale != scale) {
            erase();
            this.scale = scale;
            repaint();
        }
    }

    public Transform getTransform() {
        return transform;
    }

    public void setTransform(Transform t) {
        if ((t != null) && !(transform.equals(t))) {
            erase();
            transform = t.clone();
            repaint();
        }
    }

    // source: http://math.stackexchange.com/questions/78137/decomposition-of-a-nonsquare-affine-matrix
    protected static void applyTransformToGraphics(Transform t, Graphics g) {

        double[] elements = t.getElements();


        double a = elements[0];
        double b = elements[2];
        double c = elements[4];
        double d = elements[1];
        double e = elements[3];
        double f = elements[5];


        double phi = Math.atan2(b, a);
        double q = (a*d + b*e) / (a*e - b*d);
        double p = Math.sqrt(a*a + b*b);
        double r = (a*e - b*d) / Math.sqrt(a*a + b*b);


        float deg = (float)(-phi * 180.0 / Math.PI); // negated to have the correct direction

        float shearx = 0;
        float sheary = (float)q;

        float sx = (float)p;
        float sy = (float)r;

        float tx = (float)c;
        float ty = (float)f;


        // Only call the transform methods that actually do something. Apart from being
        // wasteful, the float variants of translate() and scale(), as well as shear() and
        // rotate() are "advanced" Graphics methods that are not implemented by every
        // Graphics implementation (e.g. GraphicsSVG throws an exception for all of them),
        // and the overwhelming majority of the figures have no transform at all.
        if (tx != 0 || ty != 0) {
            if (tx == Math.rint(tx) && ty == Math.rint(ty))
                g.translate((int)tx, (int)ty); // the int variant is implemented by everyone
            else
                g.translate(tx, ty);
        }

        if (sx != 1 || sy != 1)
            g.scale(sx, sy);

        if (shearx != 0 || sheary != 0)
            g.shear(shearx, sheary);

        if (deg != 0)
            g.rotate(deg);

        if (Debug.isDebugging()) {
            // TESTING:

            Transform test = new Transform();

            test.rotate(Math.toRadians(deg));

            test.skewx(shearx);
            test.skewy(sheary);

            test.scale(sx, sy);
            test.translate(tx, ty);

            double[] testElements = test.getElements();

            boolean mismatch = false;
            for (int i = 0; i < 6; ++i) {
                if (Math.abs(testElements[i] - elements[i]) > 0.01) {
                    mismatch = true;
                    break;
                }
            }

            if (mismatch) {
                Debug.println("transform decomposition failed:");
                Debug.println("deg: " + deg + " shear: " + shearx + " " + sheary + " scale: " + sx + " " + sy + " translate: " + tx + " " + ty);
                for (int i = 0; i < 6; ++i) {
                    Debug.println("difference in element " + i + ": " + (testElements[i] - elements[i]));
                }

                Debug.println("original: " + t.toString());
                Debug.println("recovered: " + test.toString());
                Debug.println("-----------");
            }
        }
    }

    protected Transform getCascadedTransformRec() {
        Transform cascaded = new Transform();

        IFigure parent = getParent();
        if (parent instanceof AbstractCanvasFigure) {
            cascaded.multiply(((AbstractCanvasFigure) parent).getCascadedTransformRec());
        }

        cascaded.multiply(getTransform());
        return cascaded;
    }

    public Transform getCascadedTransform() {
        Transform cascaded = new Transform();
        cascaded.scale((float)scale, (float)scale); // zoom
        cascaded.multiply(getCascadedTransformRec());
        return cascaded;
    }

    /**
     * Prevents drawing if this figure is not visible.
     */
    @Override
    @SuppressWarnings("unchecked")
    public void paint(Graphics graphics) {
        // A non-invertible transform (e.g. a zero scale factor, which is allowed in the simulation)
        // collapses the figure and its whole subtree into a line or a point: there is nothing to draw,
        // like in Qtenv. (Also, the transform could not be inverted below.)
        if (isVisible && getCascadedTransform().isInvertible()) {
            graphics.pushState();
            try {
                applyTransformToGraphics(getCascadedTransform(), graphics);
                paintFigure(graphics);

                // To avoid the "clipping modified in a way that cannot be saved or restored" bug in SWTGraphics
                // also, Graphics expects the clip rect in the transformed coordinate system, so we transform
                graphics.setClip(getCascadedTransform().getInverse().applyTo(getBounds()));

                graphics.restoreState();
                for (IFigure child : (List<IFigure>)getChildren()) {
                    child.paint(graphics);
                }
            } finally {
                graphics.popState();
            }
        }
    }

    /**
     * Should return true if the figure can be selected (is opaque) at the given
     * coordinates, in the parent's coordinate system.
     */
    protected abstract boolean hitTest(Point point);

    /**
     * Adds visibility check and hit testing to the superclass's method.
     */
    @Override
    public IFigure findFigureAt(int x, int y, TreeSearch search) {
        if (!isVisible || !getCascadedTransform().isInvertible()) {
            return null; // not drawn, see paint()
        }

        IFigure result = super.findFigureAt(x, y, search);

        Point point = getCascadedTransform().getInverse().applyTo(new PrecisionPoint(x, y));

        return (result == this) ? (hitTest(point) ? this : null) : result;
    }

    @Override
    public void setZIndex(double zIndex) {
        if (this.zIndex != zIndex) {
            this.zIndex = zIndex;
            if (getParent() instanceof IFigureSortingParent) {
                ((IFigureSortingParent)getParent()).childChanged(this);
            }
        }
    }

    @Override
    public double getZIndex() {
        return zIndex;
    }

    public void setOrdinal(int ordinal) {
        if (this.ordinal != ordinal) {
            this.ordinal = ordinal;
            if (getParent() instanceof IFigureSortingParent) {
                ((IFigureSortingParent)getParent()).childChanged(this);
            }
        }
    }

    @Override
    public int getOrdinal() {
        return ordinal;
    }

    @Override
    @SuppressWarnings("unchecked")
    public void childChanged(IComparableFigure child) {
        FigureUtils.sortFigures(getChildren());
        repaint();
    }

    /**
     * Subclasses can specify their graphical clipping region with this method.
     * The returned Rectangle is never modified, and it should be in the
     * parent's coordinate system.
     */
    protected abstract Rectangle getGraphicalBounds();

    @Override
    public void validate() {
        erase();
        recomputeBounds();
        repaint();
        super.validate();
    }

    // the real bounds if this very figure, without the children
    private Rectangle getOwnBounds() {
        Rectangle graphical = getGraphicalBounds();
        PrecisionRectangle bounds = new PrecisionRectangle(graphical);

        // the original parameters
        double width = bounds.preciseWidth();
        double height = bounds.preciseHeight();

        double x = bounds.preciseX();
        double y = bounds.preciseY();

        // what gets rounded down on the top and the left, should be added on the right and the bottom
        width += (x - Math.floor(x));
        height += (y - Math.floor(y));

        // then rounding "outwards" to make sure nothing gets cropped

        bounds.setPreciseWidth(Math.ceil(width) + 1); // there is an off-by-one error in the
        bounds.setPreciseHeight(Math.ceil(height) + 1); // drawing of the selection "border" rectangle

        bounds.setPreciseX(Math.floor(x));
        bounds.setPreciseY(Math.floor(y));

        return bounds;
    }

    // includes the children's bounding boxes into this figure's own bounds
    @SuppressWarnings("unchecked")
    public void recomputeBounds() {
        Rectangle bounds = getCascadedTransform().applyTo(getOwnBounds());

        for (Figure child : (List<Figure>)getChildren()) {
            ((AbstractCanvasFigure)child).recomputeBounds();
            Rectangle childBounds = child.getBounds();
            bounds.union(childBounds);
        }

        this.bounds = bounds;
    }

}