package org.omnetpp.ned.editor.graph.parts.canvas;

import org.eclipse.draw2d.geometry.PrecisionDimension;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.figures.canvas.CanvasPlaceholderFigure;
import org.omnetpp.figures.misc.AnchoredRectangle;
import org.omnetpp.ned.editor.graph.misc.CanvasFigureUtils;
import org.omnetpp.ned.editor.graph.parts.NedEditPartFactory;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

/**
 * EditPart for @figure types that have no dedicated edit part: unknown/custom
 * types, and known types the editor cannot display (e.g. "panel"). Renders a placeholder
 * figure using pos and size property keys, defaulting to 80x40 at the origin.
 */
public class PlaceholderEditPart extends AbstractCanvasShapeEditPart {

    @Override
    protected boolean representsType(String type) {
        // must accept exactly the types for which NedEditPartFactory creates a placeholder,
        // otherwise getModel() returns null for them
        return !NedEditPartFactory.hasFigureEditPart(type);
    }

    @Override
    public boolean isResizable() {
        return true;
    }

    @Override
    public CanvasPlaceholderFigure getFigure() {
        return (CanvasPlaceholderFigure)super.getFigure();
    }

    @Override
    protected CanvasPlaceholderFigure createFigure() {
        return new CanvasPlaceholderFigure();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        CanvasPlaceholderFigure figure = getFigure();

        PrecisionPoint pos = parsePoint(PKEY_POS);
        PrecisionDimension size = parseDimension(PKEY_SIZE);
        AnchoredRectangle.Anchor anchor = parseAnchor(PKEY_ANCHOR);

        AnchoredRectangle rect = parseAnchoredRectangle(PKEY_BOUNDS, PKEY_POS, PKEY_SIZE, PKEY_ANCHOR);

        // override defaults: use 0,0 for pos and 80x40 for size when not specified
        if (pos == null && size == null && CanvasFigureUtils.parseRectangle(getModel(), PKEY_BOUNDS) == null) {
            rect = new AnchoredRectangle(0, 0, 80, 40, anchor != null ? anchor : AnchoredRectangle.Anchor.ANCHOR_NONE);
        }
        else if (size == null && CanvasFigureUtils.parseRectangle(getModel(), PKEY_BOUNDS) == null) {
            rect = new AnchoredRectangle(rect.getLocation().preciseX(), rect.getLocation().preciseY(), 80, 40, rect.getAnchor());
        }

        figure.setDefiningRectangle(rect);

        String type = getModel().getValue(PKEY_TYPE);
        figure.setType(type);
    }
}
