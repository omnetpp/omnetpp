package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.GraphicalEditPart;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.figures.ConnectionFigure;

public class ConnectionEndpointEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy {

    @Override
    protected void addSelectionHandles() {
        super.addSelectionHandles();

        getConnectionFigure().setLineWidth(getConnectionFigure().getLocalLineWidth() + 1);
        getConnectionFigure().setForegroundColor(ColorFactory.highlight);
    }

    protected ConnectionFigure getConnectionFigure() {
        return (ConnectionFigure) ((GraphicalEditPart) getHost()).getFigure();
    }

    @Override
    protected void removeSelectionHandles() {
        super.removeSelectionHandles();
        getConnectionFigure().setLineWidth(getConnectionFigure().getLocalLineWidth());
        getConnectionFigure().setLineStyle(getConnectionFigure().getLocalLineStyle());
        getConnectionFigure().setForegroundColor(getConnectionFigure().getLocalLineColor());
    }

}