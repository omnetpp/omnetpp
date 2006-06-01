package org.omnetpp.scave2.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IActionDelegate;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave2.editors.ScaveEditor;

public class OpenDatasetActionDelegate implements IActionDelegate {
	
	public void run(IAction action) {
		IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
		IEditorPart editor = page.getActiveEditor();
		ISelection selection = page.getSelection();
		if (editor instanceof ScaveEditor && selection instanceof IStructuredSelection) {
			ScaveEditor scaveEditor = (ScaveEditor)editor;
			IStructuredSelection structuredSelection = (IStructuredSelection)selection;
			Object element = structuredSelection.getFirstElement();
			if (element instanceof Dataset) {
				Dataset dataset = (Dataset)element;
				scaveEditor.openDataset(dataset);
			}
		}
	}

	public void selectionChanged(IAction action, ISelection selection) {
	}
}
