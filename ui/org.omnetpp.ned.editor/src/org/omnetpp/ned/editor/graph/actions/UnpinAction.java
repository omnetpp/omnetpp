package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;

import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.commands.SetConstraintCommand;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.policies.PolicyUtil;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.INamedGraphNode;

public class UnpinAction extends org.eclipse.gef.ui.actions.SelectionAction {

	public static final String ID = "Pinned";
	public static final String MENUNAME = "Pinned";
	public static final String TOOLTIP = "Pins the module to a location";
	public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN);

	public UnpinAction(IWorkbenchPart part) {
		super(part, IAction.AS_CHECK_BOX);
		setText(MENUNAME);
		setId(ID);
		setToolTipText(TOOLTIP);
		setImageDescriptor(IMAGE);
		setHoverImageDescriptor(IMAGE);
	}

	/**
	 * This command can be executed ONLY on submodules who has fixed location
	 * @return
	 */
	@Override
    protected boolean calculateEnabled() {
        Command cmd = getCommand();
        if (cmd == null) {
            setChecked(false);
            return false;
        }
        return cmd.canExecute();
	}

	@Override
	protected void refresh() {
	    int size = getSelectedObjects().size();
        if (size > 0) {
	        Point loc = ((INamedGraphNode)((GraphicalEditPart)
	                            getSelectedObjects().get(size-1)).getModel()).getDisplayString().getLocation(null);
	        setChecked(loc != null);
	    }
	    super.refresh();
	}

    @Override
    public void run() {
        execute(getCommand());
    }

    /**
     * Returns the <code>Command</code> to unpin a group of children.
     * @param request the ChangeBoundsRequest
     * @param toggleEnabled whether we want to toggle the state of the child
     * @param pinState if toggleEnabled was false, we can set the new pin state directly here
     * @return the Command
     */
    protected Command getCommand() {
        CompoundCommand resize = new CompoundCommand();

        for (GraphicalEditPart child : (List<GraphicalEditPart>)getSelectedObjects()) {
            SetConstraintCommand c = createTogglePinCommand(child);
            if (c != null)
                resize.add(c);
        }
        // do not provide a command if there were no submodules that can be pinned unpinned
        if (resize.size() < 1) return UnexecutableCommand.INSTANCE;

        return resize.unwrap();
    }



    /**
     * Generate a constraint change command in response to a pin/unpin request
     * @param request
     * @param child
     * @param toggleEnabled whether we want to toggle the state of the child
     * @param pinState if toggleEnabled was false, we can set the new pin state directly here
     * @return The created ConstraintCommand
     */
    protected SetConstraintCommand createTogglePinCommand(GraphicalEditPart child) {
        if (child.getModel() instanceof SubmoduleNodeEx && PolicyUtil.isEditable(child)) {
            SubmoduleNodeEx smodule = (SubmoduleNodeEx) child.getModel();
            // get the compound module scaling factor
            float scale = ((ModuleEditPart)child).getScale();
            // otherwise create a command that deletes the location from the display string
            SetConstraintCommand cmd = new SetConstraintCommand(smodule, scale);
            // pin the module at the current temporary location if it is not fixed
            cmd.setPinLocation(isChecked() ? child.getFigure().getBounds().getCenter() : null);
            return cmd;
        }

        return null;
    }

}
