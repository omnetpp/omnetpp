/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Common Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/cpl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.omnetpp.ned.editor.graph.edit;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.ManhattanConnectionRouter;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.draw2d.RelativeBendpoint;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.editparts.AbstractConnectionEditPart;
import org.omnetpp.ned.editor.graph.edit.policies.WireBendpointEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.WireEditPolicy;
import org.omnetpp.ned.editor.graph.edit.policies.WireEndpointEditPolicy;
import org.omnetpp.ned.editor.graph.figures.FigureFactory;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.old.WireBendpointModel;
import org.omnetpp.ned.editor.graph.model.old.WireModel;
import org.omnetpp.ned2.model.NEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Implements a Connection Editpart to represnt a Wire like connection.
 * 
 */
// TODO remove dependency from PropertyCHangeListener
public class WireEditPart extends AbstractConnectionEditPart implements PropertyChangeListener, NEDChangeListener {

    public void activate() {
        super.activate();
        // register as listener of the model object
        getWire().addListener(this);
    }

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
    protected IFigure createFigure() {
    	Connection connx = FigureFactory.createNewWire(getWire());
    	return connx;    }

    public void deactivate() {
        // register as listener of the model object
        getWire().removeListener(this);
        super.deactivate();
    }

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
     * Returns the Figure associated with this, which draws the Wire.
     * 
     * @return Figure of this.
     */
    protected IFigure getWireFigure() {
        return (PolylineConnection) getFigure();
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
