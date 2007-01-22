package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.edit.SubmoduleEditPart;

public class UnpinAction extends org.eclipse.gef.ui.actions.SelectionAction {

	public static final String REQ_UNPIN = "unpin"; 

	public static final String ID = "Unpin";
	public static final String MENUNAME = "Unpin";
	public static final String TOOLTIP = "Allow the module to move freely";
	public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN);

	GroupRequest request;

	public UnpinAction(IWorkbenchPart part) {
		super(part);
		request = new GroupRequest(REQ_UNPIN);
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
	protected boolean calculateEnabled() {
        Command cmd = getCommand();
        if (cmd == null)
            return false;
        return cmd.canExecute();
	}

	/**
	 * get a command for the request from the containing compound module
	 * @return
	 */
	private Command getCommand() {
		List selEditParts = getSelectedObjects();
		// include the selected editparts, 
		request.setEditParts(selEditParts);
		
		if (selEditParts.size() < 1 || !(selEditParts.get(0) instanceof SubmoduleEditPart))
			return null;
		
		// get the parent of the currently selected 
		EditPart compoundModuleEditPart = ((EditPart)selEditParts.get(0)).getParent();
		// get the command from the containing compound module edit part
		return compoundModuleEditPart.getCommand(request);
	}

	public void run() {
		execute(getCommand());
	}

}
