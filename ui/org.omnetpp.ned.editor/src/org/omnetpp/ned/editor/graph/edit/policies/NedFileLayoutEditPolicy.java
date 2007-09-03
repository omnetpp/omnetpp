package org.omnetpp.ned.editor.graph.edit.policies;

import java.util.List;

import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Insets;
import org.eclipse.draw2d.geometry.PrecisionRectangle;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.FlowLayoutEditPolicy;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;

import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.ned.editor.graph.commands.CloneCommand;
import org.omnetpp.ned.editor.graph.commands.CreateNedTypeElementCommand;
import org.omnetpp.ned.editor.graph.commands.ReorderCommand;
import org.omnetpp.ned.editor.graph.commands.SetCompoundModuleConstraintCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.EditPartUtil;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * Layout policy used in the top level NedFile element allowing a vertical, toolbar like
 * layout, rearrange of components.
 *
 * @author rhornig
 */
public class NedFileLayoutEditPolicy extends FlowLayoutEditPolicy {

	/** (non-Javadoc)
	 * @see org.eclipse.gef.editpolicies.OrderedLayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
	 * we create a resize police where only SOUTH and EAST sides are modifiable
	 */
	@Override
	protected EditPolicy createChildEditPolicy(EditPart child) {
		ResizableEditPolicy policy = new NedResizeEditPolicy();
		// we need only resize support for the south east and corner for compound modules
		if (child instanceof CompoundModuleEditPart)
            policy.setResizeDirections(PositionConstants.SOUTH_EAST);
        else  // and no resize for other types
            policy.setResizeDirections(PositionConstants.NONE);

        if (!EditPartUtil.isEditable(child)) {
            policy.setResizeDirections(PositionConstants.NONE);
            policy.setDragAllowed(false);
        }

		return policy;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gef.editpolicies.FlowLayoutEditPolicy#isHorizontal()
	 * WARNING we override this function so it is possible to use the ToolbarLayout
	 * with the FlowLayoutPolicy, because implementation of FlowLayoutEditPolicy#isHorizonta()
	 * depends on FlowLayout.
	 */
	@Override
	protected boolean isHorizontal() {
		return false;
	}

	@SuppressWarnings("unchecked")
	@Override
	protected Command getCloneCommand(ChangeBoundsRequest request) {

		EditPart iPoint = getInsertionReference(request);
		INEDElement insertBeforeNode = iPoint != null ? (INEDElement)iPoint.getModel() : null;
		NedFileElementEx parent = (NedFileElementEx)getHost().getModel();
		CloneCommand cloneCmd = new CloneCommand(parent, insertBeforeNode);

		// iterate through all involved editparts and add their model to the coning list
		for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts())
		    if (currPart.getModel() instanceof INedTypeElement)
		        cloneCmd.add((INedTypeElement)currPart.getModel());

		return cloneCmd;
	}

	// adding an already existing node is not supported
	@Override
    protected Command createAddCommand(EditPart child, EditPart after) {
		return null;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gef.editpolicies.OrderedLayoutEditPolicy#createMoveChildCommand(org.eclipse.gef.EditPart, org.eclipse.gef.EditPart)
	 * if wherePart is null we must insert the child at the end
	 */
	@Override
    protected Command createMoveChildCommand(EditPart movedPart, EditPart wherePart) {
		INEDElement where = wherePart != null ? (INEDElement)wherePart.getModel() : null;
		INEDElement node = (INEDElement)movedPart.getModel();
		return new ReorderCommand(where, node);
	}

	@Override
    protected Command getCreateCommand(CreateRequest request) {
	    Object element = request.getNewObject();
	    if (!(element instanceof INedTypeElement))
	        return UnexecutableCommand.INSTANCE;

	    INedTypeElement newTypeElement = (INedTypeElement)element;
		EditPart insertionPoint = getInsertionReference(request);
		INEDElement where = insertionPoint != null ? (INEDElement)insertionPoint.getModel() : null;
		NedFileElementEx parent = (NedFileElementEx)getHost().getModel();
		return new CreateNedTypeElementCommand(parent, where, newTypeElement);
	}

	/**
	 * Returns a Command to resize a group of children, based on the ChangeBoundsRequest
	 * passed.
	 */
	@SuppressWarnings("unchecked")
	protected Command getResizeChildrenCommand(ChangeBoundsRequest request) {
		CompoundCommand resize = new CompoundCommand();
		Command c;

		for (GraphicalEditPart child : (List<GraphicalEditPart>)request.getEditParts()) {
			c = createChangeConstraintCommand(request, child, getConstraintFor(request, child));
			resize.add(c);
		}
		return resize.unwrap();
	}

    protected Command createChangeConstraintCommand(ChangeBoundsRequest request,
    						EditPart child, Object constraint) {
        // HACK for fixing issue when the model returns unspecified size (-1,-1)
        // we have to calculate the center point in that direction manually using the size info
        // from the figure directly (which knows it's size) This is the inverse transformation of
        // CenteredXYLayout's transformation
        Rectangle figureBounds = ((GraphicalEditPart)child).getFigure().getBounds();
        Rectangle modelConstraint = (Rectangle)constraint;
        if (modelConstraint.width < 0) modelConstraint.x += figureBounds.width / 2;
        if (modelConstraint.height < 0) modelConstraint.y += figureBounds.height / 2;

        // create the constraint change command
        if (child.getModel() instanceof CompoundModuleElementEx) {
            CompoundModuleElementEx module = (CompoundModuleElementEx) child.getModel();
            CompoundModuleFigure cfigure = (CompoundModuleFigure)((GraphicalEditPart)child).getFigure();
            SetCompoundModuleConstraintCommand cmd = new SetCompoundModuleConstraintCommand(module);
            // from the constraint size subtract the difference between the whole figure size and
            // the selection rectangle
            Dimension newSize = modelConstraint.getSize().expand(cfigure.getHandleBounds().getSize());
            newSize.shrink(figureBounds.width, figureBounds.height);
            // reduce the size with the module border
            Insets inset = cfigure.getCompoundModuleBorder().getInsets(cfigure);
            newSize.shrink(inset.getWidth(), inset.getHeight());
            cmd.setSize(newSize);

            // if size constraint is not specified, then remove it from the model too
            // TODO is this needed?
            if ((modelConstraint.width < 0 || modelConstraint.height < 0) && module.getDisplayString().getCompoundSize(null) == null)
                cmd.setSize(null);
            return cmd;
        }

        return UnexecutableCommand.INSTANCE;
    }

    /**
	 * Generates a draw2d constraint object derived from the specified child EditPart using
	 * the provided Request. The returned constraint will be translated to the application's
	 * model later using {@link #translateToModelConstraint(Object)}.
	 * @param request the ChangeBoundsRequest
	 * @param child the child EditPart for which the constraint should be generated
	 * @return the draw2d constraint
	 */
	protected Object getConstraintFor(ChangeBoundsRequest request, GraphicalEditPart child) {
		Rectangle rect = new PrecisionRectangle(child.getFigure().getBounds());
		child.getFigure().translateToAbsolute(rect);
		rect = request.getTransformedRectangle(rect);
		child.getFigure().translateToRelative(rect);
		rect.translate(getLayoutContainer().getClientArea().getLocation().getNegated());
		return rect;
	}

    /**
	 * Factors out RESIZE  requests, otherwise calls <code>super</code>.
	 * @see org.eclipse.gef.EditPolicy#getCommand(Request)
	 */
	@Override
    public Command getCommand(Request request) {
		if (REQ_RESIZE_CHILDREN.equals(request.getType()))
			return getResizeChildrenCommand((ChangeBoundsRequest)request);

		return super.getCommand(request);
	}
}
