package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.ui.actions.RetargetAction;

public class ReLayoutRetargetAction extends RetargetAction {

	public ReLayoutRetargetAction() {
		super(ReLayoutAction.ID, ReLayoutAction.MENUNAME);
		setToolTipText(ReLayoutAction.TOOLTIP);
		setImageDescriptor(ReLayoutAction.IMAGE);
	}

}
