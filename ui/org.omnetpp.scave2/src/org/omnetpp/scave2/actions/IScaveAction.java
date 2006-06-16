package org.omnetpp.scave2.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.Viewer;

public interface IScaveAction extends IAction {
	public void selectionChanged(ISelection selection);
	public void setViewer(Viewer viewer);
}
