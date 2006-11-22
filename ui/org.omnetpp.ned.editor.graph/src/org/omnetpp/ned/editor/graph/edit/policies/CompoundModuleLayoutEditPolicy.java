package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.XYLayout;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.eclipse.gef.requests.GroupRequest;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.ned.editor.graph.actions.UnpinAction;
import org.omnetpp.ned.editor.graph.commands.CloneSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.CreateSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.SetConstraintCommand;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned2.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned2.model.interfaces.INamedGraphNode;


/**
 * Layout policy used in compound modules. Handles cloning, creation, resizing of submodules
 * @author rhornig
 */
public class CompoundModuleLayoutEditPolicy extends DesktopLayoutEditPolicy {

    public CompoundModuleLayoutEditPolicy(XYLayout layout) {
        super();
        setXyLayout(layout);
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy#getCommand(org.eclipse.gef.Request)
     * Factors out the Unpin request
     */
    @Override
    public Command getCommand(Request request) {
        // check if the edit part is editable
    	if (UnpinAction.REQ_UNPIN.equals(request.getType()))
    		return getUnpinChildrenCommand((GroupRequest)request);
    	return super.getCommand(request);
    }
    
    /**
     * Override to return the <code>Command</code> to perform an {@link
     * RequestConstants#REQ_CLONE CLONE}. By default, <code>null</code> is
     * returned.
     * 
     * @param request
     *            the Clone Request
     * @return A command to perform the Clone.
     */
    @SuppressWarnings("unchecked")
	@Override
    protected Command getCloneCommand(ChangeBoundsRequest request) {
        CloneSubmoduleCommand cloneCmd 
            = new CloneSubmoduleCommand((CompoundModuleNodeEx) getHost().getModel(), 
                                        ((ModuleEditPart)getHost()).getScale());

        for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts()) {
            cloneCmd.addModule((SubmoduleNodeEx)currPart.getModel(), 
            					(Rectangle) getConstraintForClone(currPart, request));
        }
        return cloneCmd;
    }

    @Override
    protected Command getCreateCommand(CreateRequest request) {
    	// only create a command if we want to create a submodule
    	if (!(request.getNewObject() instanceof SubmoduleNodeEx))
    		return null;
    	
        CreateSubmoduleCommand create 
        		= new CreateSubmoduleCommand((CompoundModuleNodeEx) getHost().getModel(),
        									 (SubmoduleNodeEx) request.getNewObject());
        create.setLocation((Rectangle)getConstraintFor(request));
        create.setLabel("Add");

        return create;
    }

    
    protected Command createChangeConstraintCommand(EditPart child, Object constraint) {
        // HACK for fixing issue when the model returns unspecified size (-1,-1)
        // we have to calculate the center point in that direction manually using the size info
        // from the figure directly (which knows it's size) This is the inverse transformation of
        // CenteredXYLayout's traf.
        Rectangle figureBounds = ((GraphicalEditPart)child).getFigure().getBounds();
        Rectangle modelConstraint = (Rectangle)constraint;
        if (modelConstraint.width < 0) modelConstraint.x += figureBounds.width / 2;
        if (modelConstraint.height < 0) modelConstraint.y += figureBounds.height / 2;

        // get the compound module scaling factor
        float scale = ((ModuleEditPart)child).getScale();
        
        // create the constraint change command 
        INamedGraphNode module = (INamedGraphNode) child.getModel();
        SetConstraintCommand cmd = new SetConstraintCommand(module, scale);
        cmd.setConstraint(modelConstraint);

        // if size constrant is not specified, then remove it from the model too
        // TODO is this needed?
        if ((modelConstraint.width < 0 || modelConstraint.height < 0) && module.getDisplayString().getSize(null) == null)
            cmd.setSize(null);
        
        return cmd;
    }
    
    /**
     * Returns the <code>Command</code> to unpin a group of children.
     * @param request the ChangeBoundsRequest
     * @return the Command
     */
    protected Command getUnpinChildrenCommand(GroupRequest request) {
    	CompoundCommand resize = new CompoundCommand();
    	Command c;
    	GraphicalEditPart child;
    	List children = request.getEditParts();

    	for (int i = 0; i < children.size(); i++) {
    		child = (GraphicalEditPart)children.get(i);
    		c = createUnpinCommand(request, child);
    		if (c != null) 
    			resize.add(c);
    	}
    	// do not provide a command if there were no submodules with location info (ie. all was unpinned)
    	if (resize.size() < 1) return null;
    	
    	return resize.unwrap();
    }
    /**
     * Generate a constraint change command in response to an unpin request
     * @param child
     * @return
     */
    protected Command createUnpinCommand(Request request, EditPart child) {
        // check if the editpart is editable and do not create a commend if it's not
        if (child instanceof IReadOnlySupport 
                && !((IReadOnlySupport)child).isEditable())
            return null;
        
        // create the constraint change command 
        INamedGraphNode module = (INamedGraphNode) child.getModel();
        // do not create a command for submodules that do not have a location
        if (module.getDisplayString().getLocation(null) == null)
        	return null;

        // get the compound module scaling factor
        float scale = ((ModuleEditPart)child).getScale();
        // otherwise create a command that deletes the location from the displayestring
        SetConstraintCommand cmd = new SetConstraintCommand(module, scale);
        // delete the location info, so the node can be moved freely by the layouting algorythm
        // we leave the size unchanged
        cmd.setLocation(null);

        return cmd;
    }

    /**
     * We create a generic resize policy that allows resizing in any direction. 
     * @see org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
     * 
     */
    @Override
    protected EditPolicy createChildEditPolicy(EditPart child) {
        ResizableEditPolicy policy = new NedResizeEditPolicy();
        // check if the editpart is editable and do not allow resize or drag operations
        if (child instanceof IReadOnlySupport 
                && !((IReadOnlySupport)child).isEditable()) {
            policy.setResizeDirections(PositionConstants.NONE);
            policy.setDragAllowed(false);
        }

        return policy;
    }

    /*
     * (non-Javadoc)
     * 
     * @see org.eclipse.gef.editpolicies.LayoutEditPolicy#createSizeOnDropFeedback(org.eclipse.gef.requests.CreateRequest)
     */
    // this command is created when the user adds a new child by drag/resizing with a creation
    // tool selected. ie. specifying the width/length for the new child along with the location
    @Override
    protected IFigure createSizeOnDropFeedback(CreateRequest createRequest) {
        IFigure figure;

         figure = new RectangleFigure();
        ((RectangleFigure) figure).setFill(false);
        figure.setForegroundColor(ColorFactory.highlight);

        addFeedback(figure);
        return figure;
    }

    /**
     * Returns the layer used for displaying feedback. We must return the scaled feedback layer
     * 
     * @return the feedback layer
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }
}