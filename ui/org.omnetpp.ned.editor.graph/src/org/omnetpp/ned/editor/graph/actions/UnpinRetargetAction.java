package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.ui.actions.RetargetAction;

public class UnpinRetargetAction extends RetargetAction {

	public UnpinRetargetAction() {
		super(UnpinAction.ID, UnpinAction.MENUNAME);
		setToolTipText(UnpinAction.TOOLTIP);
		setImageDescriptor(UnpinAction.IMAGE);
	}

}
