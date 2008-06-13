package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.views.VectorBrowserView;


public class ShowOutputVectorViewAction extends AbstractScaveAction {
	public ShowOutputVectorViewAction() {
		setText("Show Output Vector View");
		setToolTipText("Show Output Vector View");
	}
	
	@Override
	protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
		try {
			IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
			workbenchPage.showView(VectorBrowserView.ID);
		} 
		catch (PartInitException e) {
			IStatus error = ScavePlugin.getErrorStatus(0, "Failed to open Output Vector View", e);
			ErrorDialog dialog = new ErrorDialog(scaveEditor.getSite().getShell(), null, null, error, IStatus.ERROR);
			dialog.open();
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return true;
	}
}
