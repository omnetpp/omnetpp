package org.omnetpp.ned.editor.graph.edit;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ManhattanConnectionRouter;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.RelativeBendpoint;
import org.eclipse.draw2d.RoutingAnimator;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;
import org.omnetpp.ned.editor.graph.edit.policies.WireBendpointEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.WireEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.WireEndpointEditPolicy;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDElementUtil;
import org.omnetpp.ned2.model.WireBendpointModel;

/**
 * Implements a Connection Editpart to represnt a Wire like connection.
 * 
 */
// TODO remove dependency from PropertyChangeListener
public class WireEditPart extends AbstractConnectionEditPart implements PropertyChangeListener, INEDChangeListener {

    @Override
    public void activate() {
        super.activate();
        // register as listener of the model object
        getWire().addListener(this);
    }

    @Override
    public void activateFigure() {
        super.activateFigure();
        /*
         * Once the figure has been added to the ConnectionLayer, start
         * listening for its router to change.
         */
        getFigure().addPropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
    }

    /**
     * Adds extra EditPolicies as required.
     */
    @Override
    protected void createEditPolicies() {
        installEditPolicy(EditPolicy.CONNECTION_ENDPOINTS_ROLE, new WireEndpointEditPolicy());
        // Note that the Connection is already added to the diagram and knows
        // its Router.
        refreshBendpointEditPolicy();
        installEditPolicy(EditPolicy.CONNECTION_ROLE, new WireEditPolicy());
    }

    /**
     * Returns a newly created Figure to represent the connection.
     * 
     * @return The created Figure.
     */
    @Override
    protected IFigure createFigure() {
        PolylineConnection conn = new PolylineConnection();
        conn.addRoutingListener(RoutingAnimator.getDefault());
        PolygonDecoration arrow;

        // draw an arrow at the destModule side if it's not a bidirectional connection
        if (getWire().getArrowDirection() == NEDElementUtil.NED_ARROWDIR_BIDIR)
            arrow = null;
        else {
            arrow = new PolygonDecoration();
            arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
            arrow.setScale(6, 3);
            conn.setTargetDecoration(arrow);
        }
    	return conn;    
    }

    @Override
    public void deactivate() {
        // deregister as listener of the model object
        getWire().removeListener(this);
        super.deactivate();
    }

    @Override
    public void deactivateFigure() {
        getFigure().removePropertyChangeListener(Connection.PROPERTY_CONNECTION_ROUTER, this);
        super.deactivateFigure();
    }

    /**
     * Returns the model of this represented as a Wire.
     * 
     * @return Model of this as <code>Wire</code>
     */
    protected ConnectionNodeEx getWire() {
        return (ConnectionNodeEx) getModel();
    }


    /**
     * Listens to changes in properties of the Wire (like the contents being
     * carried), and reflects is in the visuals.
     * 
     * @param event
     *            Event notifying the change.
     */
    public void propertyChange(PropertyChangeEvent event) {
        String property = event.getPropertyName();
        if (Connection.PROPERTY_CONNECTION_ROUTER.equals(property)) {
            refreshBendpoints();
            refreshBendpointEditPolicy();
        }
    }

    /**
     * Updates the bendpoints, based on the model.
     */
    protected void refreshBendpoints() {
        if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter) return;
        List modelConstraint = getWire().getBendpoints();
        List figureConstraint = new ArrayList();
        for (int i = 0; i < modelConstraint.size(); i++) {
            WireBendpointModel wbp = (WireBendpointModel) modelConstraint.get(i);
            RelativeBendpoint rbp = new RelativeBendpoint(getConnectionFigure());
            rbp.setRelativeDimensions(wbp.getFirstRelativeDimension(), wbp.getSecondRelativeDimension());
            rbp.setWeight((i + 1) / ((float) modelConstraint.size() + 1));
            figureConstraint.add(rbp);
        }
        getConnectionFigure().setRoutingConstraint(figureConstraint);
    }

    private void refreshBendpointEditPolicy() {
        if (getConnectionFigure().getConnectionRouter() instanceof ManhattanConnectionRouter)
            installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, null);
        else
            installEditPolicy(EditPolicy.CONNECTION_BENDPOINTS_ROLE, new WireBendpointEditPolicy());
    }

    /**
     * Refreshes the visual aspects of this, based upon the model (Wire). It
     * changes the wire color depending on the state of Wire.
     * 
     */
    @Override
    protected void refreshVisuals() {
        refreshBendpoints();
        // TODO implement display property support for connections here
    }

	public void attributeChanged(NEDElement node, String attr) {
        refreshVisuals();
	}

	public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
        refreshVisuals();
	}

	public void childRemoved(NEDElement node, NEDElement child) {
        refreshVisuals();
	}
}
