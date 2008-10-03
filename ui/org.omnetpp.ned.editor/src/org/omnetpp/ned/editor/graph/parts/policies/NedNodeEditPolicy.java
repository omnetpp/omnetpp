package org.omnetpp.ned.editor.graph.parts.policies;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.ReconnectRequest;

import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.ned.editor.graph.commands.CreateConnectionCommand;
import org.omnetpp.ned.editor.graph.commands.ReconnectCommand;
import org.omnetpp.ned.editor.graph.parts.ModuleEditPart;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;

/**
 * Handle all connection related requests for graphically represented nodes (compound and submodules)
 *
 * @author rhornig
 */
public class NedNodeEditPolicy extends GraphicalNodeEditPolicy {

	// Used to give feedback during dragging
	@Override
	protected Connection createDummyConnection(Request req) {
		ConnectionFigure cf = new ConnectionFigure();
		cf.setArrowHeadEnabled(true);
		return cf;
	}

    /**
     * Feedback should be added to the scaled feedback layer.
     *
     * @see org.eclipse.gef.editpolicies.GraphicalEditPolicy#getFeedbackLayer()
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    
    /**
     * Returns either the module name or "" if the module is a compound
     */
    private static String getModuleNameForConnection(IConnectableElement connectable) {
        if (connectable instanceof CompoundModuleElementEx)
            return "";
        return connectable.getName();
    }
    
	// called during connection creation on the first click
	@Override
    protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
        ConnectionElementEx conn = (ConnectionElementEx)request.getNewObject();
        conn.setSrcModule(getModuleNameForConnection(getGraphNodeModel()));
        CreateConnectionCommand command = new CreateConnectionCommand(conn, getModuleEditPart().getCompoundModulePart().getCompoundModuleModel());
        request.setStartCommand(command);
        return command;
    }

    // called when clicked on the second node during connection creation
    @Override
    protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {
        CreateConnectionCommand command = (CreateConnectionCommand)request.getStartCommand();
        command.getConnection().setDestModule(getModuleNameForConnection(getGraphNodeModel()));
        return command;
    }

    @Override
    protected Command getReconnectTargetCommand(ReconnectRequest request) {
        ModuleEditPart srcEP = (ModuleEditPart)request.getConnectionEditPart().getSource();
        ModuleEditPart targetEP = (ModuleEditPart)request.getTarget();
        // allow the reconnection only inside the same compound module
        if (srcEP.getCompoundModulePart() != targetEP.getCompoundModulePart())
            return UnexecutableCommand.INSTANCE;

        ConnectionElementEx conn = (ConnectionElementEx)request.getConnectionEditPart().getModel();
        ReconnectCommand command = new ReconnectCommand(conn, true);
        command.getTemplateConnection().setDestModule(getModuleNameForConnection(getGraphNodeModel()));
        return command;
    }

    @Override
    protected Command getReconnectSourceCommand(ReconnectRequest request) {
        ModuleEditPart targetEP = (ModuleEditPart)request.getConnectionEditPart().getTarget();
        ModuleEditPart srcEP = (ModuleEditPart)request.getTarget();
        // allow the reconnection only inside the same compound module
        if (srcEP.getCompoundModulePart() != targetEP.getCompoundModulePart())
            return UnexecutableCommand.INSTANCE;
        
        ConnectionElementEx conn = (ConnectionElementEx)request.getConnectionEditPart().getModel();
        ReconnectCommand command = new ReconnectCommand(conn, false);
        command.getTemplateConnection().setSrcModule(getModuleNameForConnection(getGraphNodeModel()));
        return command;
    }

    protected ModuleEditPart getModuleEditPart() {
        return (ModuleEditPart) getHost();
    }

    protected IConnectableElement getGraphNodeModel() {
        return (IConnectableElement) getHost().getModel();
    }

}