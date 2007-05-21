package org.omnetpp.scave.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.dialogs.ResourceListSelectionDialog;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Adds a workspace file to the input.
 * The user is asked to select the file.
 */
public class AddResultFileAction extends AbstractScaveAction {
	public AddResultFileAction() {
        setText("Add File...");
        setToolTipText("Add a scalar or vector file to the inputs");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		ResourceListSelectionDialog dialog = new ResourceListSelectionDialog(editor.getSite().getShell(), ResourcesPlugin.getWorkspace().getRoot(), IResource.FILE);
		dialog.setTitle("Select Result File");
		if (dialog.open() == Window.OK) {
			Object[] result = dialog.getResult();
			if (result != null && result.length == 1 && result[0] instanceof IFile) {
				IFile file = (IFile)result[0];
				InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
				inputFile.setName(file.getFullPath().toString());

				editor.executeCommand(
					AddCommand.create(
						editor.getEditingDomain(),
						editor.getAnalysis().getInputs(),
						ScaveModelPackage.eINSTANCE.getInputs_Inputs(),
						inputFile));
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return true;
	}
}
