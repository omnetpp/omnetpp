package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.ReconnectRequest;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.editor.graph.figures.ModuleFigure;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INamedGraphNode;
import org.omnetpp.ned2.model.NEDElementFactoryEx;
import org.omnetpp.ned2.model.NedElementExUtil;
import org.omnetpp.ned2.model.pojo.NEDElementFactory;
import org.omnetpp.ned2.model.pojo.NEDElementTags;

public class NedNodeEditPolicy extends GraphicalNodeEditPolicy {

    @Override
    protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {
        ConnectionCommand command = (ConnectionCommand) request.getStartCommand();
        command.setDestModule(getNedNodeModel());
        ConnectionAnchor ctor = getNedNodeEditPart().getTargetConnectionAnchor(request);
        if (ctor == null) return null;
        command.setDestGate(getNedNodeEditPart().getGate(ctor));
        return command;
    }

    @Override
    protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
        ConnectionNodeEx conn = (ConnectionNodeEx)request.getNewObject();
//        conn.setArrowDirection(ConnectionNodeEx.NED_ARROWDIR_L2R);
        ConnectionCommand command = new ConnectionCommand(conn);
        command.setSrcModule(getNedNodeModel());
        ConnectionAnchor ctor = getNedNodeEditPart().getSourceConnectionAnchor(request);
        command.setSrcGate(getNedNodeEditPart().getGate(ctor));
        request.setStartCommand(command);
        return command;
    }

    /**
     * Feedback should be added to the scaled feedback layer.
     * 
     * @see org.eclipse.gef.editpolicies.GraphicalEditPolicy#getFeedbackLayer()
     */
    @Override
    protected IFigure getFeedbackLayer() {
        /*
         * Fix for Bug# 66590 Feedback needs to be added to the scaled feedback
         * layer
         */
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    protected ModuleEditPart getNedNodeEditPart() {
        return (ModuleEditPart) getHost();
    }

    protected INamedGraphNode getNedNodeModel() {
        return (INamedGraphNode) getHost().getModel();
    }

    @Override
    protected Command getReconnectTargetCommand(ReconnectRequest request) {

        ConnectionCommand cmd = new ConnectionCommand((ConnectionNodeEx) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getNedNodeEditPart().getTargetConnectionAnchor(request);
        cmd.setDestModule(getNedNodeModel());
        cmd.setDestGate(getNedNodeEditPart().getGate(ctor));
        return cmd;
    }

    @Override
    protected Command getReconnectSourceCommand(ReconnectRequest request) {
        ConnectionCommand cmd = new ConnectionCommand((ConnectionNodeEx) request.getConnectionEditPart().getModel());

        ConnectionAnchor ctor = getNedNodeEditPart().getSourceConnectionAnchor(request);
        cmd.setSrcModule(getNedNodeModel());
        cmd.setSrcGate(getNedNodeEditPart().getGate(ctor));
        return cmd;
    }

    protected ModuleFigure getNodeFigure() {
        return (ModuleFigure) ((GraphicalEditPart) getHost()).getFigure();
    }

}