package org.omnetpp.ned.editor.graph.parts.canvas;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionPoint;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.omnetpp.figures.canvas.AbstractCanvasFigure;
import org.omnetpp.figures.canvas.CanvasPanelFigure;
import org.omnetpp.figures.misc.Transform;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

public class PanelEditPart extends AbstractCanvasFigureEditPart {
    @Override
    protected boolean representsType(String type) {
        return type.equals(FTYPE_PANEL);
    }

    @Override
    public CanvasPanelFigure getFigure() {
        return (CanvasPanelFigure)super.getFigure();
    }

    @Override
    protected CanvasPanelFigure createFigure() {
        return new CanvasPanelFigure();
    }

    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();

        CanvasPanelFigure figure = getFigure();
        figure.setPosition(getLocation());
        PrecisionPoint anchorPoint = parsePoint(PKEY_ANCHORPOINT);
        figure.setAnchorPoint(anchorPoint == null ? new PrecisionPoint() : anchorPoint);
    }

    @Override
    public Point getLocation() {
        PrecisionPoint position = parsePoint(PKEY_POS);
        return position == null ? new PrecisionPoint() : position;
    }

    @Override
    public void setLocation(Point loc) {
        setPoint(PKEY_POS, loc);
    }

    @Override
    protected Command getTransformedMoveCommand(Request request, Transform t) {
        // the position is in the parent's coordinate system, unlike the panel's contents
        IFigure parent = getFigure().getParent();
        Transform transf;
        if (parent instanceof AbstractCanvasFigure) {
            transf = ((AbstractCanvasFigure)parent).getCascadedTransform();
        } else {
            transf = new Transform();
            transf.scale((float)getScale(), (float)getScale());
        }

        return super.getTransformedMoveCommand(request, transf);
    }
}
