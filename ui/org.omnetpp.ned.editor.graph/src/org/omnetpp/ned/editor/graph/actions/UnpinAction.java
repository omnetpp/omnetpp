package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.Request;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.edit.SubmoduleEditPart;

public class UnpinAction extends org.eclipse.gef.ui.actions.SelectionAction {

	private static final String UNPIN_REQUEST = "UnpinRequest"; 

	public static final String ID = "Unpin";
	public static final String MENUNAME = "Unpin";
	public static final String TOOLTIP = "Allow the module to move freely";
	public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_UNPIN);

	Request request;

	public UnpinAction(IWorkbenchPart part) {
		super(part);
		request = new Request(UNPIN_REQUEST);
		setText(MENUNAME);
		setId(ID);
		setToolTipText(TOOLTIP);
		setImageDescriptor(IMAGE);
		setHoverImageDescriptor(IMAGE);
	}

	protected boolean calculateEnabled() {
		return canPerformAction();
	}

	private boolean canPerformAction() {
		if (getSelectedObjects().isEmpty())
			return false;
		List parts = getSelectedObjects();
		for (int i = 0; i < parts.size(); i++) {
			Object o = parts.get(i);
			if (!(o instanceof SubmoduleEditPart))
				return false;
		}
		return true;
	}

	private Command getCommand() {
		List editparts = getSelectedObjects();
		CompoundCommand cc = new CompoundCommand();
		cc.setDebugLabel("Unpin submodule");//$NON-NLS-1$
		for (int i = 0; i < editparts.size(); i++) {
			EditPart part = (EditPart) editparts.get(i);
			cc.add(part.getCommand(request));
		}
		return cc;
	}

	public void run() {
		execute(getCommand());
	}

}
