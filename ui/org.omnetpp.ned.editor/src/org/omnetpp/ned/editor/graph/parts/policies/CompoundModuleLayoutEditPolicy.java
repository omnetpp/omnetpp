/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts.policies;

import java.util.List;

import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.PositionConstants;
import org.eclipse.draw2d.RectangleFigure;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.PrecisionRectangle;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPolicy;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.LayerConstants;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.RectangleF;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.commands.ChangeDisplayPropertyCommand;
import org.omnetpp.ned.editor.graph.commands.CloneSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.CreateNedTypeElementCommand;
import org.omnetpp.ned.editor.graph.commands.CreateSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.InsertCommand;
import org.omnetpp.ned.editor.graph.commands.RemoveCommand;
import org.omnetpp.ned.editor.graph.commands.SetConstraintCommand;
import org.omnetpp.ned.editor.graph.misc.ModelFactory;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.editor.graph.parts.ModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IModuleKindTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.CommentElement;
import org.omnetpp.ned.model.pojo.ModuleInterfaceElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Layout policy used in compound modules. Handles cloning, creation, resizing of submodules
 * via creating appropriate commands.
 *
 * @author rhornig
 */
public class CompoundModuleLayoutEditPolicy extends ConstrainedLayoutEditPolicy {

    protected Request currentRequest;

    public CompoundModuleLayoutEditPolicy() {
        super();
    }

    public CompoundModuleEditPart getHost() {
        return (CompoundModuleEditPart) super.getHost(); // may be installed only on compound modules
    }

    /**
     * Checks if the provided type is allowed to be inserted into the current
     * edit part. Inner types are allowed only in top level types.
     */
    protected boolean isInsertable(INedElement element) {
        // no inner types are allowed if we are already an inner type (no more than 1 level of nesting)
        CompoundModuleElementEx compModule = getHost().getModel();
        if (compModule.getNedTypeInfo().isInnerType())  // the host is already an inner type
            return false;

        // check if the dropped element has inner types. in this case it should not be added as an inner-type
        if (element instanceof CompoundModuleElementEx &&
            ((CompoundModuleElementEx)element).getFirstTypesChild() != null)
            return false;

        return true;
    }

    /**
     * Networks are not allowed as inner types.
     */
    protected boolean isAllowedType(INedElement element) {
        // every type is allowed except networks
        return element instanceof INedTypeElement &&
            !(element instanceof CompoundModuleElementEx && ((CompoundModuleElementEx)element).isNetwork());
    }

    @Override
    public Command getCommand(Request request) {
        // store the request so we can access it during command creation
        currentRequest = request;
        return super.getCommand(request);
    }

    @SuppressWarnings("unchecked")
    @Override
    protected Command getCloneCommand(ChangeBoundsRequest request) {
        CompoundCommand compoundCmd = new CompoundCommand();

        // creating a new a submodules based on a simple or compound module types in the selection
        for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts())
            if (currPart.getModel() instanceof IModuleKindTypeElement) {
                // the type in the selected editpart is a simple/compound or module interface
                // prepare a submodule creation command for it
                INedTypeElement typeElement = (INedTypeElement)currPart.getModel();

                // skip networks. we should not instantiate them
                if (!isAllowedType(typeElement))
                    continue;

                String instanceName = StringUtils.toInstanceName(typeElement.getName());
                String fullyQualifiedTypeName = typeElement.getNedTypeInfo().getFullyQualifiedName();
                ModelFactory factory = new ModelFactory(NedElementTags.NED_SUBMODULE, instanceName, fullyQualifiedTypeName, typeElement instanceof ModuleInterfaceElement);
                CreateRequest createRequest = new CreateRequest();
                createRequest.setFactory(factory);
                createRequest.setLocation(request.getLocation());
                // get a submodule creation command
                compoundCmd.add(getCreateCommand(createRequest));
            }

        // allow submodule cloning only INSIDE the submodule area
        Point p = request.getLocation();
        if (!getHost().isOnBorder(p.x, p.y)) {
            // create a direct clone command for all submodule parts
            CloneSubmoduleCommand cloneCmd = new CloneSubmoduleCommand(getHost().getModel());

            for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts())
                if (currPart.getModel() instanceof SubmoduleElementEx) {
                    Rectangle rect = (Rectangle)getConstraintForClone(currPart, request);
                    RectangleF bounds = RectangleF.fromPixels(rect, getHost().getScale());
                    cloneCmd.addModule((SubmoduleElementEx)currPart.getModel(), bounds);
                }

            if (cloneCmd.canExecute())
                compoundCmd.add(cloneCmd);
        }

        return compoundCmd;
    }

    @Override
    protected Command getCreateCommand(CreateRequest request) {
        INedElement element = (INedElement)request.getNewObject();
        CompoundModuleElementEx compoundModule = getHost().getModel();
        // submodule creation
        if (element instanceof SubmoduleElementEx) {
            // do no allow dropping a submodule on the compound module title
            Point p = request.getLocation();
            if (getHost().isOnBorder(p.x, p.y))
                return UnexecutableCommand.INSTANCE;

            CreateSubmoduleCommand createCommand = new CreateSubmoduleCommand(compoundModule, (SubmoduleElementEx)element);
            Rectangle constraint = (Rectangle)getConstraintFor(request);
            createCommand.setConstraint(RectangleF.fromPixels(constraint, getHost().getScale()));
            createCommand.setLabel("Create submodule");
            return createCommand;
        }

        // inner type creation
        if (!isAllowedType(element) || !isInsertable(element))
            return UnexecutableCommand.INSTANCE;

        // only NedTypeElements are allowed here
        CompoundCommand command = new CompoundCommand("Create type");

        // We do not need any banner comments for inner types. - remove them
        CommentElement bannerComment = (CommentElement)element.getFirstChildWithAttribute(NedElementTags.NED_COMMENT, CommentElement.ATT_LOCID, "banner");
        if (bannerComment != null)
            bannerComment.removeFromParent();

        TypesElement typesElement = compoundModule.getFirstTypesChild();
        // add a new types element if necessary.
        if (typesElement == null) {
            typesElement = (TypesElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_TYPES);
            command.add(new InsertCommand(compoundModule, typesElement));
        }
        command.add(new CreateNedTypeElementCommand(typesElement, null, (INedTypeElement)element));
        return command;
    }

    @Override
    protected Command createAddCommand(EditPart childToAdd, Object constraint) {
        INedElement element = (INedElement)childToAdd.getModel();

        // if the add is not targeted to the title/border i.e. it is dropped inside
        // a the submodule area (and the added element is a type), we should
        // create a new submodule with that type instead of creating an inner type
        // (CloneCommand does this exactly so we reuse it)
        Point p = ((ChangeBoundsRequest)currentRequest).getLocation();
        if (isAllowedType(element) && !getHost().isOnBorder(p.x, p.y))
            return getCloneCommand((ChangeBoundsRequest)currentRequest);

        if (!isAllowedType(element) || !isInsertable(element)) // do not allow networks or nesting of inner types
            return UnexecutableCommand.INSTANCE;

        // move the type to the new compound module
        CompoundModuleElementEx compoundModule = getHost().getModel();
        CompoundCommand command = new CompoundCommand("Move type");
        command.add(new RemoveCommand(element));
        TypesElement typesElement = compoundModule.getFirstTypesChild();
        // add a new types element if necessary.
        if (typesElement == null) {
            typesElement = (TypesElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_TYPES);
            command.add(new InsertCommand(compoundModule, typesElement));
        }
        command.add(new InsertCommand(typesElement, element));
        return command;
    }

    @Override
    protected Command createChangeConstraintCommand(EditPart child, Object newConstraint) {
        // do not allow change if we are read only components
        if (!EditPartUtil.isEditable(child))
            return null;

        Rectangle modelConstraint = (Rectangle)newConstraint;

        // disable the resize if we reach the minimal size
        if (modelConstraint.width == getMinimumSizeFor((GraphicalEditPart)child).width ||
                modelConstraint.height == getMinimumSizeFor((GraphicalEditPart)child).height)
            return UnexecutableCommand.INSTANCE;

        // check if we have a shape in the figure. If not, the resize command should change the icon size
        // property not the shape size
        IConnectableElement submodule = (IConnectableElement) child.getModel();
        SubmoduleFigure childFigure = (SubmoduleFigure)((GraphicalEditPart)child).getFigure();
        Rectangle oldBounds = childFigure.getShapeBounds();
        boolean widthChanged = modelConstraint.width != oldBounds.width;
        boolean heightChanged = modelConstraint.height != oldBounds.height;
        if (child instanceof SubmoduleEditPart && !((SubmoduleFigure)childFigure).isShapeVisible() && (widthChanged || heightChanged)) {
            // icon resizing operation
            // calculate the desired size, if we support arbitrary resizing, we can simply set
            // the received width value, but for now we have to choose between vs, s, l, vl
            int iconsize = widthChanged ? modelConstraint.width :
                           heightChanged ? modelConstraint.height :
                               (int)Math.sqrt(modelConstraint.width*modelConstraint.height);
            String newImageSize = "";
            if (iconsize <= 20)
                newImageSize = "vs";    // 16x16 pixel icons
            else if (iconsize <= 32)
                newImageSize = "s";     // 24x24 pixel icons
            else if (iconsize <= 50)
                newImageSize = "";      // 40x40 pixel icons
            else if (iconsize <= 80)
                newImageSize = "l";     // 60x60 pixel icons
            else
                newImageSize = "vl";    // 100x100 pixel icons

            ChangeDisplayPropertyCommand displayStringChange = new ChangeDisplayPropertyCommand(submodule, IDisplayString.Prop.IMAGE_SIZE, newImageSize);
            displayStringChange.setLabel("Set icon size");
            return displayStringChange;
        }
        else {
            // move or resize operation
            float scale = ((ModuleEditPart)child).getScale();
            SetConstraintCommand constraintCmd = new SetConstraintCommand(submodule, RectangleF.fromPixels(oldBounds, scale));
            constraintCmd.setConstraint(RectangleF.fromPixels(modelConstraint, scale));
            return constraintCmd;
        }
    }

    /**
     * We create a generic resize policy that allows resizing in any direction. But restrict it in some cases
     * for example on inherited submodules or for submodules which has icons
     * @see org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy#createChildEditPolicy(org.eclipse.gef.EditPart)
     *
     */
    @Override
    protected EditPolicy createChildEditPolicy(EditPart child) {
        ResizableEditPolicy policy = new NedResizeEditPolicy();
        // check if the edit part is editable and do not allow resize or drag operations
        if (!EditPartUtil.isEditable(child)) {
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
        figure.setForegroundColor(GraphicalNedEditor.HIGHLIGHT_COLOR);

        addFeedback(figure);
        return figure;
    }

    /**
     * Returns the layer used for displaying feedback. We must return the scaled feedback layer
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

    // DESKTOPLAYOUTEDITPOLICY
    /**
     * Returns a MODEL object constraint equivalent to the DRAW2D constraint. We transform the
     * draw2d constraint here back to model constraint (ie. the location is the refpoint of the
     * figure in the model, but the top left corner in draw2d) The model to draw2d trasform is done
     * in  DesktopLayout
     *  @param rect Draw2d constraint
     *  @return The equivalent model constraint (location pointing to the figure's refpoint)
     * @see org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy#getConstraintFor(org.eclipse.draw2d.geometry.Rectangle)
     */

    @Override
    public Rectangle getConstraintFor(Rectangle rect) {
        return new Rectangle(rect.getCenter(), rect.getSize());
    }

    protected Rectangle getConstraintForClone(GraphicalEditPart part, ChangeBoundsRequest request) {
        IFigure figure = part.getFigure();
        Rectangle bounds = new PrecisionRectangle(
                          figure instanceof SubmoduleFigure ? ((SubmoduleFigure)figure).getShapeBounds()
                                                                   : figure.getBounds());

        figure.translateToAbsolute(bounds);
        bounds = request.getTransformedRectangle(bounds);

        getHost().getContentPane().translateToRelative(bounds);
        bounds.translate(getLayoutOrigin().getNegated());
        return getConstraintFor(bounds);
    }

    private static final Dimension DEFAULT_SIZE = new Dimension(-1, -1);

    /**
     * Overridden to prevent sizes from becoming too small, and to prevent preferred sizes
     * from getting lost. If the Request is a MOVE, the existing width and height are
     * preserved. During RESIZE, the new width and height have a lower bound determined by
     * {@link #getMinimumSizeFor(GraphicalEditPart)}.
     *
     * @see ConstrainedLayoutEditPolicy#getConstraintFor(ChangeBoundsRequest, GraphicalEditPart)
     */
    protected Rectangle getConstraintFor(ChangeBoundsRequest request, GraphicalEditPart child) {
        IFigure childFig = child.getFigure();
        Rectangle rect = new PrecisionRectangle(
                          childFig instanceof SubmoduleFigure ? ((SubmoduleFigure)childFig).getShapeBounds()
                                                                   : childFig.getBounds());
        Rectangle original = rect.getCopy();
        child.getFigure().translateToAbsolute(rect);
        rect = request.getTransformedRectangle(rect);
        child.getFigure().translateToRelative(rect);
        rect.translate(getLayoutOrigin().getNegated());

        if (request.getSizeDelta().width == 0 && request.getSizeDelta().height == 0) {
            Rectangle cons = getCurrentConstraintFor(child);
            if (cons != null) //Bug 86473 allows for unintended use of this method
                rect.setSize(cons.width, cons.height);
        } else { // resize
            Dimension minSize = getMinimumSizeFor(child);
            if (rect.width < minSize.width) {
                rect.width = minSize.width;
                if (rect.x > (original.right() - minSize.width))
                    rect.x = original.right() - minSize.width;
            }
            if (rect.height < minSize.height) {
                rect.height = minSize.height;
                if (rect.y > (original.bottom() - minSize.height))
                    rect.y = original.bottom() - minSize.height;
            }
        }
        return getConstraintFor(rect);
    }

    /**
     * Returns a Rectangle at the given Point with width and height of -1.
     * <code>XYLayout</code> uses width or height equal to '-1' to mean use the figure's
     * preferred size.
     * @param p the input Point
     * @return a Rectangle
     */
    public Rectangle getConstraintFor(Point p) {
        return new Rectangle(p, DEFAULT_SIZE);
    }

    /**
     * Retrieves the child's current constraint from the <code>LayoutManager</code>.
     * @param child the child
     * @return the current constraint
     */
    protected Rectangle getCurrentConstraintFor(GraphicalEditPart child) {
        IFigure childFig = child.getFigure();
        if (childFig instanceof SubmoduleFigure)
            return ((SubmoduleFigure)childFig).getShapeBounds();
        else
            return childFig.getBounds();
    }

    /**
     * Determines the <em>minimum</em> size that the specified child can be resized to. Called
     * from {@link #getConstraintFor(ChangeBoundsRequest, GraphicalEditPart)}. By default,
     * a small <code>Dimension</code> is returned.
     * @param child the child
     * @return the minimum size
     */
    protected Dimension getMinimumSizeFor(GraphicalEditPart child) {
        return new Dimension(8, 8);
    }

    /**
     * Places the feedback rectangle where the User indicated.
     * @see LayoutEditPolicy#showSizeOnDropFeedback(CreateRequest)
     */
    protected void showSizeOnDropFeedback(CreateRequest request) {
        Point p = new Point(request.getLocation().getCopy());
        IFigure feedback = getSizeOnDropFeedback(request);
        feedback.translateToRelative(p);
        Dimension size = request.getSize().getCopy();
        feedback.translateToRelative(size);
        feedback.setBounds(new Rectangle(p, size).expand(getCreationFeedbackOffset(request)));
    }
}