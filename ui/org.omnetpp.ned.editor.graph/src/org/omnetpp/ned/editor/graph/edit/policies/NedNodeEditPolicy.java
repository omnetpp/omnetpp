package org.omnetpp.ned.editor.graph.edit.policies;

import org.eclipse.draw2d.Connection;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.requests.DropRequest;
import org.eclipse.gef.requests.ReconnectRequest;
import org.omnetpp.ned.editor.graph.commands.ConnectionCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.SubmoduleEditPart;
import org.omnetpp.ned.editor.graph.figures.ConnectionFigure;
import org.omnetpp.ned.editor.graph.figures.GateAnchor;
import org.omnetpp.ned.editor.graph.figures.ModuleFigure;
import org.omnetpp.ned2.model.ConnectionNodeEx;
import org.omnetpp.ned2.model.INamedGraphNode;

public class NedNodeEditPolicy extends GraphicalNodeEditPolicy {
	
	// Used to give feedback during dragging
	@Override
	protected Connection createDummyConnection(Request req) {
		ConnectionFigure cf = new ConnectionFigure();
		cf.setArrowEnabled(true);
		return cf;
	}

	// called during connection creation on the first click
	@Override
    protected Command getConnectionCreateCommand(CreateConnectionRequest request) {
        ConnectionNodeEx conn = (ConnectionNodeEx)request.getNewObject();
        ConnectionCommand command = new ConnectionCommand(conn);
        command.setSrcModule(getGraphNodeModel());
        GateAnchor anchor = (GateAnchor)getModuleEditPart().getSourceConnectionAnchor(request);
        if (anchor == null) 
        	return UnexecutableCommand.INSTANCE;
        
        request.setStartCommand(command);
        return command;
    }

    // called when clicked on the second node durnig connection creation
    @Override
    protected Command getConnectionCompleteCommand(CreateConnectionRequest request) {
        ConnectionCommand command = (ConnectionCommand) request.getStartCommand();
        command.setDestModule(getGraphNodeModel());
        GateAnchor anchor = (GateAnchor)getModuleEditPart().getTargetConnectionAnchor(request);
        if (anchor == null) 
        	return UnexecutableCommand.INSTANCE;
        
        return command;
    }

    @Override
    protected Command getReconnectTargetCommand(ReconnectRequest request) {
    	ConnectionNodeEx conn = (ConnectionNodeEx) request.getConnectionEditPart().getModel();
        ConnectionCommand cmd = new ConnectionCommand(conn);

        GateAnchor anchor = (GateAnchor)getModuleEditPart().getTargetConnectionAnchor(request);
        if (anchor == null) 
        	return UnexecutableCommand.INSTANCE;

        cmd.setDestModule(getGraphNodeModel());
        return cmd;
    }

    @Override
    protected Command getReconnectSourceCommand(ReconnectRequest request) {
    	ConnectionNodeEx conn = (ConnectionNodeEx) request.getConnectionEditPart().getModel();
        ConnectionCommand cmd = new ConnectionCommand(conn);

        GateAnchor anchor = (GateAnchor)getModuleEditPart().getSourceConnectionAnchor(request);
        if (anchor == null) 
        	return UnexecutableCommand.INSTANCE;
        
        cmd.setSrcModule(getGraphNodeModel());
        return cmd;
    }
    
    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.GraphicalNodeEditPolicy#getTargetEditPart(org.eclipse.gef.Request)
     * Filter the possible targets for the connection creation/reconnect requests so that only
     * submodules in the same parent can be connectected and the compound module should be targeted ONLY
     * if the mouse is over the border of the compound module. 
     */
//    @Override
//    public EditPart getTargetEditPart(Request request) {
//    	
//    	if (ep != null && REQ_RECONNECT_SOURCE.equals(request.getType()))
//    		return null;
//
//    	EditPart destEditPart = super.getTargetEditPart(request);
//		EditPart srcEditPart = ((CreateConnectionRequest)request).getSourceEditPart();
//
//		// if both editpart is Submodule, they must share the same parent (ie. compound module)
//		if (srcEditPart instanceof SubmoduleEditPart 
//				&& destEditPart instanceof SubmoduleEditPart
//				&& srcEditPart.getParent() != destEditPart.getParent()) 
//			return false;
//		// if one of them is Submodule and the other is compound module then the
//		// submodule's parent must be the compound module (ie. submodule can be connected ony to it's parent)
//		if (srcEditPart instanceof SubmoduleEditPart 
//				&& destEditPart instanceof CompoundModuleEditPart
//				&& srcEditPart.getParent() != destEditPart) 
//			return false;
//		if (srcEditPart instanceof CompoundModuleEditPart 
//				&& destEditPart instanceof SubmoduleEditPart
//				&& srcEditPart != destEditPart.getParent()) 
//			return false;
//		// do not allow connetction between compound modules at all
//		if (srcEditPart instanceof CompoundModuleEditPart 
//				&& destEditPart instanceof CompoundModuleEditPart) {
//			return false;
//		}
//	}
//	// if the selection target is a CompoundModule, allow selection ONLY using it's borders
//	if (editpart instanceof CompoundModuleEditPart) {
//		CompoundModuleEditPart cmep = (CompoundModuleEditPart)editpart;
//		return cmep.isOnBorder(getLocation().x, getLocation().y);
//	}
//    	
//    	return ep;
//    }
    
    /**
     * Feedback should be added to the scaled feedback layer.
     * 
     * @see org.eclipse.gef.editpolicies.GraphicalEditPolicy#getFeedbackLayer()
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    protected ModuleEditPart getModuleEditPart() {
        return (ModuleEditPart) getHost();
    }

    protected INamedGraphNode getGraphNodeModel() {
        return (INamedGraphNode) getHost().getModel();
    }

    protected ModuleFigure getModuleFigure() {
        return (ModuleFigure)getHostFigure();
    }

}