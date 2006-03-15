package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.gef.GraphicalEditPart;
import org.omnetpp.ned.editor.graph.misc.ColorFactory;

public class WireEndpointEditPolicy extends org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy {

    protected void addSelectionHandles() {
        super.addSelectionHandles();
        getConnectionFigure().setLineWidth(2);
        getConnectionFigure().setForegroundColor(ColorFactory.highlight);
    }

    protected PolylineConnection getConnectionFigure() {
        return (PolylineConnection) ((GraphicalEditPart) getHost()).getFigure();
    }

    protected void removeSelectionHandles() {
        super.removeSelectionHandles();
        getConnectionFigure().setLineWidth(0);
        getConnectionFigure().setForegroundColor(ColorFactory.connection);
    }

}