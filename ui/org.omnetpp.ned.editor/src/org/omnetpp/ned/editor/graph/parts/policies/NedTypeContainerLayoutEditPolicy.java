/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.policies;

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
import org.omnetpp.common.displaymodel.DimensionF;
import org.omnetpp.ned.editor.graph.commands.CloneCommand;
import org.omnetpp.ned.editor.graph.commands.CreateNedTypeElementCommand;
import org.omnetpp.ned.editor.graph.commands.InsertCommand;
import org.omnetpp.ned.editor.graph.commands.RemoveCommand;
import org.omnetpp.ned.editor.graph.commands.ReorderCommand;
import org.omnetpp.ned.editor.graph.commands.SetCompoundModuleConstraintCommand;
import org.omnetpp.ned.editor.graph.figures.CompoundModuleTypeFigure;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.editor.graph.parts.TypesEditPart;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Layout policy used in the top level NedFile element and in TypesElement allowing a
 * vertical, toolbar like layout, provides creation and rearrange commands.
 *
 * @author rhornig
 */
public class NedTypeContainerLayoutEditPolicy extends FlowLayoutEditPolicy {

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

    protected boolean isInsertable(INedElement element) {
        // no inner types are allowed if we are already an inner type (no more than 1 level of nesting)
        if (getHost() instanceof TypesEditPart) {
            TypesElement typesModel = ((TypesEditPart)getHost()).getModel();
            if (typesModel.getEnclosingTypeElement().getNedTypeInfo().isInnerType())
                return false;
        }

        // check if the dropped element has inner types. in this case it should not be added as an inner-type (no nesting allowed)
        if (element instanceof CompoundModuleElementEx &&
            ((CompoundModuleElementEx)element).getFirstTypesChild() != null)
            return false;

        // every type is allowed (but networks are not inside an inner type)
        return element instanceof INedTypeElement &&
                  !(getHost() instanceof TypesEditPart // this is an inner type container (not a top level ned file)
                    && element instanceof CompoundModuleElementEx
                    && ((CompoundModuleElementEx)element).isNetwork());
    }

    @SuppressWarnings("unchecked")
    @Override
    protected Command getCloneCommand(ChangeBoundsRequest request) {

        EditPart iPoint = getInsertionReference(request);
        INedElement insertBeforeNode = iPoint != null ? (INedElement)iPoint.getModel() : null;
        INedElement parent = (INedElement)getHost().getModel();
        CloneCommand cloneCmd = new CloneCommand(parent, insertBeforeNode);

        // iterate through all involved editparts and add their model to the coning list
        for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts())
            if (currPart.getModel() instanceof INedTypeElement && isInsertable((INedTypeElement)currPart.getModel()))
                cloneCmd.add((INedTypeElement)currPart.getModel());

        return cloneCmd;
    }

    // creating a new ned type element
    @Override
    protected Command getCreateCommand(CreateRequest request) {
        // do not allow adding an inner-type into a module which is already an inner type

        Object element = request.getNewObject();

        if (!isInsertable((INedElement)element))
            return UnexecutableCommand.INSTANCE;

        INedTypeElement newTypeElement = (INedTypeElement)element;
        EditPart insertionPoint = getInsertionReference(request);
        INedElement where = insertionPoint != null ? (INedElement)insertionPoint.getModel() : null;
        INedElement parent = (INedElement)getHost().getModel();
        return new CreateNedTypeElementCommand(parent, where, newTypeElement);
    }

    // adding an already existing node is part of a move operation (moving a from
    // one nedTypeComtainer to an other (i.e. from top level to inner)
    @Override
    protected Command createAddCommand(EditPart childToAdd, EditPart after) {
        INedElement parent = (INedElement)getHost().getModel();
        INedElement element = (INedElement)childToAdd.getModel();

        if (!isInsertable(element))
            return UnexecutableCommand.INSTANCE;

        INedElement where = after != null ? (INedElement)after.getModel() : null;
        CompoundCommand command = new CompoundCommand("Move");
        command.add(new RemoveCommand(element));
        command.add(new InsertCommand(parent, element, where));
        return command;
    }

    /* (non-Javadoc)
     * @see org.eclipse.gef.editpolicies.OrderedLayoutEditPolicy#createMoveChildCommand(org.eclipse.gef.EditPart, org.eclipse.gef.EditPart)
     * if wherePart is null we must insert the child at the end
     */
    @Override
    protected Command createMoveChildCommand(EditPart movedPart, EditPart wherePart) {
        INedElement where = wherePart != null ? (INedElement)wherePart.getModel() : null;
        INedElement node = (INedElement)movedPart.getModel();
        return new ReorderCommand(where, node);
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
        if (modelConstraint.width < 0) 
            modelConstraint.x += figureBounds.width / 2;
        if (modelConstraint.height < 0) 
            modelConstraint.y += figureBounds.height / 2;

        // create the constraint change command
        if (child instanceof CompoundModuleEditPart) {
            CompoundModuleEditPart compoundModuleEditPart = (CompoundModuleEditPart) child;
            CompoundModuleElementEx module = compoundModuleEditPart.getModel();
            CompoundModuleTypeFigure cfigure = (CompoundModuleTypeFigure)((GraphicalEditPart)child).getFigure();
            SetCompoundModuleConstraintCommand cmd = new SetCompoundModuleConstraintCommand(module);
            float scale = compoundModuleEditPart.getScale();
            // from the constraint size subtract the difference between the whole figure size and
            // the selection rectangle
            Dimension newSize = modelConstraint.getSize().expand(cfigure.getHandleBounds().getSize());
            newSize.shrink(figureBounds.width, figureBounds.height);
            // reduce the size with the module border
            Insets inset = cfigure.getSubmoduleArea().getBorder().getInsets(cfigure);
            newSize.shrink(inset.getWidth(), inset.getHeight());
            cmd.setSize(DimensionF.fromPixels(newSize, scale));

            // if size constraint is not specified, then remove it from the model too
            // TODO is this needed?
            if ((modelConstraint.width < 0 || modelConstraint.height < 0) && module.getDisplayString().getCompoundSize() == null)
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
    protected Rectangle getConstraintFor(ChangeBoundsRequest request, GraphicalEditPart child) {
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
