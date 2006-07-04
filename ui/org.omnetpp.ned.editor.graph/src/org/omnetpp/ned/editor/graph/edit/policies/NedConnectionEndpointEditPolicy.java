package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.figures.ConnectionFigure;

public class NedConnectionEndpointEditPolicy extends ConnectionEndpointEditPolicy {

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.SelectionHandlesEditPolicy#addSelectionHandles()
     * Mark the selected connection with a bolder, highlighted line
     */
    @Override
    protected void addSelectionHandles() {
        super.addSelectionHandles();

        getConnectionFigure().setLineWidth(getConnectionFigure().getLocalLineWidth() + 1);
        getConnectionFigure().setForegroundColor(ColorFactory.highlight);
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.SelectionHandlesEditPolicy#removeSelectionHandles()
     * Return the connection to the original state (before slection)
     */
    @Override
    protected void removeSelectionHandles() {
        super.removeSelectionHandles();
        getConnectionFigure().setLineWidth(getConnectionFigure().getLocalLineWidth());
        getConnectionFigure().setLineStyle(getConnectionFigure().getLocalLineStyle());
        getConnectionFigure().setForegroundColor(getConnectionFigure().getLocalLineColor());
    }

    protected ConnectionFigure getConnectionFigure() {
        return (ConnectionFigure) ((GraphicalEditPart) getHost()).getFigure();
    }

}