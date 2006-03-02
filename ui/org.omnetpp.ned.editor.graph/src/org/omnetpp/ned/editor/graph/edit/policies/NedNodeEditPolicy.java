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
import org.omnetpp.ned.editor.graph.edit.NedNodeEditPart;
import org.omnetpp.ned.editor.graph.figures.FigureFactory;
import org.omnetpp.ned.editor.graph.figures.NedFigure;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.INedNode;
import org.omnetpp.ned.editor.graph.model.NEDElementFactoryEx;
import org.omnetpp.ned.editor.graph.model.NedElementExUtil;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;

public class NedNodeEditPolicy extends org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy {

    protected Connection createDummyConnection(Request req) {
        PolylineConnection conn = FigureFactory.createNewWire(null);
        return conn;
    }

    protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {
        ConnectionCommand command = (ConnectionCommand) request.getStartCommand();
        command.setTarget(getNedNodeModel());
        ConnectionAnchor ctor = getNedNodeEditPart().getTargetConnectionAnchor(request);
        if (ctor == null) return null;
        command.setTargetGate(getNedNodeEditPart().getGate(ctor));
        return command;
    }

    protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
        ConnectionCommand command = new ConnectionCommand();
        command.setWire((ConnectionNodeEx)NEDElementFactoryEx.getInstance().createNodeWithTag(NedElementExUtil.NED_CONNECTION));
        command.setSource(getNedNodeModel());
        ConnectionAnchor ctor = getNedNodeEditPart().getSourceConnectionAnchor(request);
        command.setSourceGate(getNedNodeEditPart().getGate(ctor));
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

    protected NedNodeEditPart getNedNodeEditPart() {
        return (NedNodeEditPart) getHost();
    }

    protected INedNode getNedNodeModel() {
        return (INedNode) getHost().getModel();
    }

    protected Command getReconnectTargetCommand(ReconnectRequest request) {

        ConnectionCommand cmd = new ConnectionCommand();
        cmd.setWire((ConnectionNodeEx) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getNedNodeEditPart().getTargetConnectionAnchor(request);
        cmd.setTarget(getNedNodeModel());
        cmd.setTargetGate(getNedNodeEditPart().getGate(ctor));
        return cmd;
    }

    protected Command getReconnectSourceCommand(ReconnectRequest request) {
        ConnectionCommand cmd = new ConnectionCommand();
        cmd.setWire((ConnectionNodeEx) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getNedNodeEditPart().getSourceConnectionAnchor(request);
        cmd.setSource(getNedNodeModel());
        cmd.setSourceGate(getNedNodeEditPart().getGate(ctor));
        return cmd;
    }

    protected NedFigure getNodeFigure() {
        return (NedFigure) ((GraphicalEditPart) getHost()).getFigure();
    }

}