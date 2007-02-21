package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.views.VectorBrowserView;


public class ShowVectorBrowserViewAction extends Action {
	public ShowVectorBrowserViewAction() {
		setText("Show Vector Data View");
		setToolTipText("Show Vector Data View");
	}
	
	@Override
	public void run() {
		try {
			IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
			workbenchPage.showView(VectorBrowserView.ID);
		} 
		catch (PartInitException e) {
			// TODO log the exception
		}
	}
}
