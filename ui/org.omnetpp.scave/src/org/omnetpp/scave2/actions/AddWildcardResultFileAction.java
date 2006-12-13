package org.omnetpp.scave2.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave2.editors.ScaveEditor;

/**
 * Adds workspace files specified by name to the input.
 * The name may contain '?' and '*' wildcards.
 * The user is asked to enter the name.
 */
public class AddWildcardResultFileAction extends AbstractScaveAction {
	public AddWildcardResultFileAction() {
		setText("Add with wildcard...");
		setToolTipText("Use wildcards to specify a set of scalar or vector files");
	}

	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		//ResourceListSelectionDialog dialog = new ResourceListSelectionDialog(editor.getSite().getShell(), ResourcesPlugin.getWorkspace().getRoot(), IResource.FILE);
		InputDialog dialog = new InputDialog(editor.getSite().getShell(),
				"Add files with wildcard", "Enter the file name. You can use ? and * wildcards to specify multiple files.", "", null);
		if (dialog.open() == Window.OK) {
			String text = dialog.getValue();
			InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
			inputFile.setName(text);
			
			editor.executeCommand(
				AddCommand.create(
					editor.getEditingDomain(),
					editor.getAnalysis().getInputs(),
					ScaveModelPackage.eINSTANCE.getInputs_Inputs(),
					inputFile));
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return true;
	}
}
