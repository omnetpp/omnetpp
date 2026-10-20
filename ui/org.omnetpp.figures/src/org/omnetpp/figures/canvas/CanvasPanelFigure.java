package org.omnetpp.figures.canvas;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.omnetpp.figures.misc.Transform;

/**
 * Figure for "panel" type @figures (cPanelFigure). Like a group, it has no visual
 * appearance of its own, but it sets up an axis-aligned, unscaled coordinate system
 * for its children: it cancels the transforms inherited from its ancestors, and
 * also the zoom. The anchor point (in the panel's own coordinate system) is mapped
 * to the position (in the parent's coordinate system). The panel's own transform
 * applies to its children (and the anchor point) as usual.
 *
 * This mirrors cPanelFigure::updateParentTransform() in the simulation library.
 */
public class CanvasPanelFigure extends CanvasGroupFigure {
    private Point position = new PrecisionPoint();
    private Point anchorPoint = new PrecisionPoint();

    public Point getPosition() {
        return position;
    }

    public void setPosition(Point position) {
        if (!this.position.equals(position)) {
            erase();
            this.position = position.getCopy();
            repaint();
        }
    }

    public Point getAnchorPoint() {
        return anchorPoint;
    }

    public void setAnchorPoint(Point anchorPoint) {
        if (!this.anchorPoint.equals(anchorPoint)) {
            erase();
            this.anchorPoint = anchorPoint.getCopy();
            repaint();
        }
    }

    @Override
    protected Transform getCascadedTransformRec() {
        // the parent's coordinate system on the screen, i.e. including the zoom
        IFigure parent = getParent();
        Transform parentTransform;
        if (parent instanceof AbstractCanvasFigure)
            parentTransform = ((AbstractCanvasFigure)parent).getCascadedTransform();
        else
            parentTransform = new Transform().scale(getScale(), getScale());

        // replace it with an axis-aligned, unscaled coordinate system, with the anchor point at the
        // position, then apply our own transform in the normal way
        PrecisionPoint origin = parentTransform.applyTo(position);
        PrecisionPoint anchor = transform.applyTo(anchorPoint);
        Transform result = new Transform().translate(origin.preciseX() - anchor.preciseX(), origin.preciseY() - anchor.preciseY());
        result.multiply(transform);

        // getCascadedTransform() applies the zoom on top of this, which must not affect the panel
        result.leftMultiply(new Transform().scale(1 / getScale(), 1 / getScale()));
        return result;
    }
}
