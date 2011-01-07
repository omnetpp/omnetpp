/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.ConstrainedLayoutEditPolicy;
import org.eclipse.gef.editpolicies.LayoutEditPolicy;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.commands.ChangeDisplayPropertyCommand;
import org.omnetpp.ned.editor.graph.commands.CloneSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.CreateNedTypeElementCommand;
import org.omnetpp.ned.editor.graph.commands.CreateSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.InsertCommand;
import org.omnetpp.ned.editor.graph.commands.RemoveCommand;
import org.omnetpp.ned.editor.graph.commands.SetConstraintCommand;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.editor.graph.parts.ModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.NedTypeEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Layout policy used in compound modules. Handles cloning, creation, resizing of submodules
 *
 * @author rhornig
 */
public class CompoundModuleLayoutEditPolicy extends ConstrainedLayoutEditPolicy {

    public CompoundModuleLayoutEditPolicy() {
        super();
    }

    @SuppressWarnings("unchecked")
	@Override
    protected Command getCloneCommand(ChangeBoundsRequest request) {
        CloneSubmoduleCommand cloneCmd
            = new CloneSubmoduleCommand((CompoundModuleElementEx) getHost().getModel(),
                                        ((ModuleEditPart)getHost()).getScale());

        for (GraphicalEditPart currPart : (List<GraphicalEditPart>)request.getEditParts()) {
            cloneCmd.addModule((SubmoduleElementEx)currPart.getModel(),
            					(Rectangle) getConstraintForClone(currPart, request));
        }
        return cloneCmd;
    }

    @Override
    protected Command getCreateCommand(CreateRequest request) {
        Object element = request.getNewObject();
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)getHost().getModel();
    	// submodule creation
    	if (element instanceof SubmoduleElementEx) {
            CreateSubmoduleCommand create = new CreateSubmoduleCommand(compoundModule, (SubmoduleElementEx)element);
    	    create.setConstraint((Rectangle)getConstraintFor(request));
    	    create.setLabel("Create submodule");
    	    return create;
    	}
    	// inner type creation
        if (element instanceof INedTypeElement) {
            CompoundCommand command = new CompoundCommand("Create type");
            TypesElement typesElement = compoundModule.getFirstTypesChild();
            // add a new types element if necessary.
            if (typesElement == null) {
                typesElement = (TypesElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_TYPES);
                command.add(new InsertCommand(compoundModule, typesElement));
            }
            command.add(new CreateNedTypeElementCommand(typesElement, null, (INedTypeElement)element));
            return command;
        }

        return UnexecutableCommand.INSTANCE;
    }

    @Override
    protected Command createAddCommand(EditPart childToAdd, Object constraint) {
        CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)getHost().getModel();
        if (childToAdd instanceof NedTypeEditPart) {
            CompoundCommand command = new CompoundCommand("Move type");
            INedElement parent = (INedElement)getHost().getModel();
            INedElement node = (INedElement)childToAdd.getModel();
            command.add(new RemoveCommand(node));
            TypesElement typesElement = compoundModule.getFirstTypesChild();
            // add a new types element if necessary.
            if (typesElement == null) {
                typesElement = (TypesElement)NedElementFactoryEx.getInstance().createElement(NedElementTags.NED_TYPES);
                command.add(new InsertCommand(compoundModule, typesElement));
            }
            command.add(new InsertCommand(typesElement, node));
            return command;
        }
        
        return UnexecutableCommand.INSTANCE;
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

            ChangeDisplayPropertyCommand dpchange = new ChangeDisplayPropertyCommand(submodule, IDisplayString.Prop.IMAGE_SIZE);
            dpchange.setLabel("Set icon size");
            dpchange.setValue(newImageSize);
            return dpchange;
        }
        else {
            // move or resize operation
            float scale = ((ModuleEditPart)child).getScale();
            SetConstraintCommand constrCmd = new SetConstraintCommand(submodule, scale, oldBounds);
            constrCmd.setConstraint(modelConstraint);
            return constrCmd;
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
        Rectangle bounds = new PrecisionRectangle(((SubmoduleFigure)figure).getShapeBounds());
        
        figure.translateToAbsolute(bounds);
        bounds = request.getTransformedRectangle(bounds);
        
        ((GraphicalEditPart)getHost()).getContentPane().translateToRelative(bounds);
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
    	Rectangle rect = new PrecisionRectangle(((SubmoduleFigure)child.getFigure()).getShapeBounds());
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
    	SubmoduleFigure fig = (SubmoduleFigure)child.getFigure();
    	return fig.getShapeBounds();
    }

    /**
     * Determines the <em>minimum</em> size that the specified child can be resized to. Called
     * from {@link #getConstraintFor(ChangeBoundsRequest, GraphicalEditPart)}. By default,
     * a small <code>Dimension</code> is returned.
     * @param child the child
     * @return the minumum size
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