package org.omnetpp.scave2.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;

public interface IScaveAction extends IAction {
	public void selectionChanged(ISelection selection);
}
