package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.misc.NedConnectionEndHandle;
import org.omnetpp.ned.editor.graph.misc.NedConnectionStartHandle;

/**
 * @author rhornig
 * Defines selection handle creation/removal
 */
public class NedConnectionEndpointEditPolicy extends ConnectionEndpointEditPolicy {

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.SelectionHandlesEditPolicy#addSelectionHandles()
     * Mark the selected connection with a bolder, highlighted line
     */
    @Override
    protected void addSelectionHandles() {
        super.addSelectionHandles();

        getConnectionFigure().setLineWidth(getConnectionFigure().getLocalLineWidth() + 1);
        Color color = isHostEditable() ? ColorFactory.highlight : ColorFactory.lowlight;
        getConnectionFigure().setForegroundColor(color);
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.ConnectionEndpointEditPolicy#createSelectionHandles()
     * Redefine to create special connection handles that return NedConnectionEndpointTracker 
     */
    @SuppressWarnings("unchecked")
    @Override
	protected List createSelectionHandles() {
    	List list = new ArrayList();
    	list.add(new NedConnectionEndHandle((ConnectionEditPart)getHost()));
    	list.add(new NedConnectionStartHandle((ConnectionEditPart)getHost()));
     	return list;
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
        return (ConnectionFigure) getConnection();
    }

    /**
     * @return Whether the host part of this policy is editable or not
     */
    public boolean isHostEditable() {
        if (getHost() instanceof IReadOnlySupport) 
            return ((IReadOnlySupport)getHost()).isEditable();
        // by default it's editable
        return true;
    }

}