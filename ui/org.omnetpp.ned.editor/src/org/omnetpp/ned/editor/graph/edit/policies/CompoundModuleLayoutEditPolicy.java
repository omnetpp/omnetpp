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
import org.eclipse.gef.commands.CompoundCommand;
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
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.INamedGraphNode;

/**
 * Layout policy used in compound modules. Handles cloning, creation, resizing of submodules
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

    @Override
    protected Command createChangeConstraintCommand(EditPart child, Object constraint) {
        IFigure childFigure = ((GraphicalEditPart)child).getFigure();
        Rectangle figureBounds = childFigure.getBounds();
        Rectangle modelConstraint = (Rectangle)constraint;
        // do not allow change if we are read only components
        if (!PolicyUtil.isEditable(child))
            return null;

        // HACK for fixing issue when the model returns unspecified size (-1,-1)
        // we have to calculate the center point in that direction manually using the size info
        // from the figure directly (which knows it's size) This is the inverse transformation of
        // CenteredXYLayout's transformation.
        if (modelConstraint.width < 0) modelConstraint.x += figureBounds.width / 2;
        if (modelConstraint.height < 0) modelConstraint.y += figureBounds.height / 2;

        // disable the resize if we reach the minimal size
        if (modelConstraint.width == getMinimumSizeFor((GraphicalEditPart)child).width ||
                modelConstraint.height == getMinimumSizeFor((GraphicalEditPart)child).height)
            return UnexecutableCommand.INSTANCE;

        // get the compound module scaling factor
        float scale = ((ModuleEditPart)child).getScale();

        CompoundCommand compound = new CompoundCommand();

        // create the constraint change command
        INamedGraphNode module = (INamedGraphNode) child.getModel();
        SetConstraintCommand constrCmd = new SetConstraintCommand(module, scale);
        constrCmd.setConstraint(modelConstraint);

        // check if we have a shape in the figure. If not, the resize command should change the icon size
        // property not the shape size
        ChangeDisplayPropertyCommand dpchange = null;
        // icon resize valid only for submodules, where no icon is present and there was a change in the size
        // of the figure
        if (child instanceof SubmoduleEditPart && !((SubmoduleFigure)childFigure).isShapeVisible() &&
                modelConstraint.width > 0) {
            // delete the move command (we do not change the position if icon resizing is in progress)
            constrCmd = null;
            dpchange = new ChangeDisplayPropertyCommand(((SubmoduleEditPart)child).getSubmoduleModel(), IDisplayString.Prop.IMAGESIZE);
            dpchange.setLabel("Set icon size");
            // calculate the desired size, if we support arbitrary resizing, we can simply set
            // the received width value, but for now we have to choose between vs, s, l, vl
            int ratio = 100*modelConstraint.width / 40;
            String newImageSize = "";
            if (ratio < 60)
                newImageSize = "vs";
            else if (ratio < 100)
                newImageSize = "s";
            else if (ratio < 150)
                newImageSize = "";
            else if (ratio < 250)
                newImageSize = "l";
            else
                newImageSize = "vl";

            dpchange.setValue(newImageSize);
        }

        compound.add(dpchange);
        compound.add(constrCmd);
        return compound;
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
     *
     * @return the feedback layer
     */
    @Override
    protected IFigure getFeedbackLayer() {
        return getLayer(LayerConstants.SCALED_FEEDBACK_LAYER);
    }

}