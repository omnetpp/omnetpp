package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.gef.GraphicalEditPart;
import org.omnetpp.common.color.ColorFactory;

public class ConnectionEndpointEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy {

    @Override
    protected void addSelectionHandles() {
        super.addSelectionHandles();
        getConnectionFigure().setLineWidth(2);
        getConnectionFigure().setForegroundColor(ColorFactory.highlight);
    }

    protected PolylineConnection getConnectionFigure() {
        return (PolylineConnection) ((GraphicalEditPart) getHost()).getFigure();
    }

    @Override
    protected void removeSelectionHandles() {
        super.removeSelectionHandles();
        getConnectionFigure().setLineWidth(0);
        getConnectionFigure().setForegroundColor(ColorFactory.connection);
    }

}