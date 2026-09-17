/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.awt.Dimension;
import java.awt.geom.AffineTransform;
import java.awt.geom.NoninvertibleTransformException;
import java.awt.geom.Rectangle2D;
import java.util.Stack;

import org.apache.batik.anim.dom.SVGDOMImplementation;
import org.apache.batik.svggen.SVGGraphics2D;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gmf.runtime.draw2d.ui.render.awt.internal.graphics.GraphicsToGraphics2DAdaptor;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;

/**
 * A draw2d Graphics that renders into an SVG document, with support for affine
 * transformations.
 *
 * It is used instead of GMF's GraphicsSVG, which does not support transformations:
 * translate(float,float), scale(float,float), shear() and rotate() are not implemented
 * there at all (they throw an exception), and scale(double), although implemented, has
 * no effect on the produced SVG (it only affects an internal SWTGraphics that serves as
 * the source of font metrics). translate(int,int) does work, but it is implemented as an
 * integer offset that is added to the coordinates of every primitive, i.e. it always acts
 * as the innermost transformation, so it could not be combined with rotation or scaling
 * in the right order anyway.
 *
 * This class routes every transformation into the affine transform of the underlying AWT
 * Graphics2D, which composes them in the correct order and can represent all of them, and
 * saves/restores that transform in pushState()/popState()/restoreState().
 *
 * GraphicsSVG cannot be subclassed for this purpose, because its constructors are private.
 *
 * @author andras
 */
@SuppressWarnings("restriction")
public class SVGGraphics extends GraphicsToGraphics2DAdaptor {
    private static final String SVG_NAMESPACE_URI = "http://www.w3.org/2000/svg";
    private static final int COORDINATE_PRECISION = 3; // digits, as in GraphicsSVG

    private SVGGraphics2D svgGraphics2D;

    // the transform of the Graphics2D, saved/restored by pushState()/popState()/restoreState();
    // the inherited state stack only holds the integer translation, which we do not use
    private Stack<AffineTransform> transformStack = new Stack<AffineTransform>();

    public static SVGGraphics getInstance(Rectangle viewPort) {
        DOMImplementation domImplementation = SVGDOMImplementation.getDOMImplementation();
        Document document = domImplementation.createDocument(SVG_NAMESPACE_URI, "svg", null);
        SVGGraphics2D svgGraphics2D = new SVGGraphics2D(document);
        svgGraphics2D.getGeneratorContext().setPrecision(COORDINATE_PRECISION);
        svgGraphics2D.setSVGCanvasSize(new Dimension(viewPort.width, viewPort.height));
        return new SVGGraphics(svgGraphics2D, viewPort);
    }

    private SVGGraphics(SVGGraphics2D svgGraphics2D, Rectangle viewPort) {
        super(svgGraphics2D, viewPort);
        this.svgGraphics2D = svgGraphics2D;
    }

    public SVGGraphics2D getSVGGraphics2D() {
        return svgGraphics2D;
    }

    @Override
    public void translate(int dx, int dy) {
        translate((float)dx, (float)dy);
    }

    @Override
    public void translate(float dx, float dy) {
        getGraphics2D().translate(dx, dy);
        updateClipAfterTransform(AffineTransform.getTranslateInstance(dx, dy));
    }

    @Override
    public void scale(double amount) {
        scale((float)amount, (float)amount);
    }

    @Override
    public void scale(float sx, float sy) {
        getGraphics2D().scale(sx, sy);
        updateClipAfterTransform(AffineTransform.getScaleInstance(sx, sy));
    }

    @Override
    public void shear(float sx, float sy) {
        getGraphics2D().shear(sx, sy);
        updateClipAfterTransform(AffineTransform.getShearInstance(sx, sy));
    }

    @Override
    public void rotate(float degrees) {
        double radians = Math.toRadians(degrees);
        getGraphics2D().rotate(radians);
        updateClipAfterTransform(AffineTransform.getRotateInstance(radians));
    }

    @Override
    public void pushState() {
        super.pushState();
        transformStack.push(getGraphics2D().getTransform());
    }

    @Override
    public void popState() {
        super.popState();
        getGraphics2D().setTransform(transformStack.pop());
    }

    @Override
    public void restoreState() {
        super.restoreState();
        getGraphics2D().setTransform(transformStack.peek());
    }

    /**
     * Re-expresses the clipping rectangle in the new coordinate system, after the given
     * transformation has been applied to the Graphics2D.
     *
     * This is necessary because the inherited implementation maintains the clip in the
     * current coordinate system: getClip() is used by draw2d to decide which children
     * need to be painted at all, and the clip is applied to the Graphics2D lazily, before
     * the next drawing operation, i.e. under whatever transform is in effect by then.
     *
     * Clipping remains an approximation in rotated or sheared coordinate systems, because
     * the clip is stored as an upright rectangle. We round outwards, so the result errs on
     * the side of clipping (and culling) too little rather than too much.
     */
    private void updateClipAfterTransform(AffineTransform transform) {
        try {
            Rectangle clip = getClip(new Rectangle());
            Rectangle2D transformedClip = transform.createInverse()
                    .createTransformedShape(new Rectangle2D.Double(clip.x, clip.y, clip.width, clip.height))
                    .getBounds2D();

            long x = (long)Math.floor(transformedClip.getMinX());
            long y = (long)Math.floor(transformedClip.getMinY());
            long right = (long)Math.ceil(transformedClip.getMaxX());
            long bottom = (long)Math.ceil(transformedClip.getMaxY());

            setClip(new Rectangle(toInt(x), toInt(y), toInt(right - x), toInt(bottom - y)));
        }
        catch (NoninvertibleTransformException e) {
            // degenerate transform: nothing can be painted through it anyway, leave the clip alone
        }
    }

    private static int toInt(long value) {
        return (int)Math.max(Integer.MIN_VALUE / 2, Math.min(Integer.MAX_VALUE / 2, value));
    }
}
