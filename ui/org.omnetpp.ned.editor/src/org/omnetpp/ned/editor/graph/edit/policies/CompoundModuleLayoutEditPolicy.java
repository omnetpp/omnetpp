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
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.editpolicies.ResizableEditPolicy;
import org.eclipse.gef.requests.ChangeBoundsRequest;
import org.eclipse.gef.requests.CreateRequest;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.editor.graph.GraphicalNedEditor;
import org.omnetpp.ned.editor.graph.commands.ChangeDisplayPropertyCommand;
import org.omnetpp.ned.editor.graph.commands.CloneSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.CreateSubmoduleCommand;
import org.omnetpp.ned.editor.graph.commands.SetConstraintCommand;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.SubmoduleEditPart;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IConnectableElement;

/**
 * Layout policy used in compound modules. Handles cloning, creation, resizing of submodules
 *
 * @author rhornig
 */
public class CompoundModuleLayoutEditPolicy extends DesktopLayoutEditPolicy {

    public CompoundModuleLayoutEditPolicy(XYLayout layout) {
        super();
        setXyLayout(layout);
    }

    /**
     * Override to return the <code>Command</code> to perform an {@link
     * RequestConstants#REQ_CLONE CLONE}. By default, <code>null</code> is
     * returned.
     *
     * @param request the Clone Request
     * @return A command to perform the Clone.
     */
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
    	// only create a command if we want to create a submodule
    	if (!(request.getNewObject() instanceof SubmoduleElementEx))
    		return null;

        CreateSubmoduleCommand create
        		= new CreateSubmoduleCommand((CompoundModuleElementEx) getHost().getModel(),
        									 (SubmoduleElementEx) request.getNewObject());
        create.setLocation((Rectangle)getConstraintFor(request));
        create.setLabel("Add");

        return create;
    }

    @Override
    protected Command createChangeConstraintCommand(EditPart child, Object newConstraint) {
        // do not allow change if we are read only components
        if (!PolicyUtil.isEditable(child))
            return null;

        Rectangle modelConstraint = (Rectangle)newConstraint;

        // disable the resize if we reach the minimal size
        if (modelConstraint.width == getMinimumSizeFor((GraphicalEditPart)child).width ||
                modelConstraint.height == getMinimumSizeFor((GraphicalEditPart)child).height)
            return UnexecutableCommand.INSTANCE;

        // check if we have a shape in the figure. If not, the resize command should change the icon size
        // property not the shape size
        IConnectableElement submodule = (IConnectableElement) child.getModel();
        IFigure childFigure = ((GraphicalEditPart)child).getFigure();
        Rectangle oldBounds = (Rectangle)childFigure.getParent().getLayoutManager().getConstraint(childFigure);
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

            ChangeDisplayPropertyCommand dpchange = new ChangeDisplayPropertyCommand(submodule, IDisplayString.Prop.IMAGESIZE);
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
        if (!PolicyUtil.isEditable(child)) {
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

}