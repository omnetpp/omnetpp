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
package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.ReconnectRequest;
import org.omnetpp.ned.editor.graph.edit.BaseEditPart;
import org.omnetpp.ned.editor.graph.figures.FigureFactory;
import org.omnetpp.ned.editor.graph.figures.NedFigure;
import org.omnetpp.ned.editor.graph.model.NedNode;
import org.omnetpp.ned.editor.graph.model.Wire;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;

public class NedNodeEditPolicy extends org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy {

    protected Connection createDummyConnection(Request req) {
        PolylineConnection conn = FigureFactory.createNewWire(null);
        return conn;
    }

    protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {
        ConnectionCommand command = (ConnectionCommand) request.getStartCommand();
        command.setTarget(getLogicSubpart());
        ConnectionAnchor ctor = getLogicEditPart().getTargetConnectionAnchor(request);
        if (ctor == null) return null;
        command.setTargetGate(getLogicEditPart().getGate(ctor));
        return command;
    }

    protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
        ConnectionCommand command = new ConnectionCommand();
        command.setWire(new Wire());
        command.setSource(getLogicSubpart());
        ConnectionAnchor ctor = getLogicEditPart().getSourceConnectionAnchor(request);
        command.setSourceGate(getLogicEditPart().getGate(ctor));
        request.setStartCommand(command);
        return command;
    }

    /**
     * Feedback should be added to the scaled feedback layer.
     * 
     * @see org.eclipse.gef.editpolicies.GraphicalEditPolicy#getFeedbackLayer()
     */
    protected IFigure getFeedbackLayer() {
        /*
         * Fix for Bug# 66590 Feedback needs to be added to the scaled feedback
         * layer
         */
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    protected BaseEditPart getLogicEditPart() {
        return (BaseEditPart) getHost();
    }

    protected NedNode getLogicSubpart() {
        return (NedNode) getHost().getModel();
    }

    protected Command getReconnectTargetCommand(ReconnectRequest request) {

        ConnectionCommand cmd = new ConnectionCommand();
        cmd.setWire((Wire) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getLogicEditPart().getTargetConnectionAnchor(request);
        cmd.setTarget(getLogicSubpart());
        cmd.setTargetGate(getLogicEditPart().getGate(ctor));
        return cmd;
    }

    protected Command getReconnectSourceCommand(ReconnectRequest request) {
        ConnectionCommand cmd = new ConnectionCommand();
        cmd.setWire((Wire) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getLogicEditPart().getSourceConnectionAnchor(request);
        cmd.setSource(getLogicSubpart());
        cmd.setSourceGate(getLogicEditPart().getGate(ctor));
        return cmd;
    }

    protected NedFigure getNodeFigure() {
        return (NedFigure) ((GraphicalEditPart) getHost()).getFigure();
    }

}