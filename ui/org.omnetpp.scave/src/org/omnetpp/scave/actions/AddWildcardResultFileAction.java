package org.omnetpp.scave.actions;

import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Adds workspace files specified by name to the input.
 * The name may contain '?' and '*' wildcards.
 * The user is asked to enter the name.
 */
public class AddWildcardResultFileAction extends AbstractScaveAction {
	public AddWildcardResultFileAction() {
		setText("Wildcard...");
		setToolTipText("Use wildcards to specify a set of scalar or vector files");
	}

	static class InputValidator implements IInputValidator {
		public String isValid(String text) {
			text = text.trim();
			if (text.length() == 0)
				return "Input cannot be empty";
			for (String fileNamePattern : StringUtils.split(text, ",")) {
				if (!fileNamePattern.matches(".*\\.[^/]+$"))
					return "File name patterns must specify file extension";
				if (!fileNamePattern.endsWith(".vec") && !fileNamePattern.endsWith(".sca"))
					return "Files must have .vec or .sca extension";
			}
			return null;
		}
	}
	
	@Override
	protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
		InputValidator inputValidator = new InputValidator();
		InputDialog dialog = new InputDialog(editor.getSite().getShell(), "Add files with wildcard", 
				"Enter the file name. You can use ? and * wildcards to specify multiple files.", 
				"*.vec, *.sca", inputValidator);
		if (dialog.open() == Window.OK) {
			String text = dialog.getValue();
			
			// split up text to multiple patterns, and add each one
			for (String fileNamePattern : StringUtils.split(text, ",")) {
				fileNamePattern = fileNamePattern.trim();
				if (inputValidator.isValid(fileNamePattern) == null) {
					InputFile inputFile = ScaveModelFactory.eINSTANCE.createInputFile();
					inputFile.setName(fileNamePattern);

					editor.executeCommand(
							AddCommand.create(
									editor.getEditingDomain(),
									editor.getAnalysis().getInputs(),
									ScaveModelPackage.eINSTANCE.getInputs_Inputs(),
									inputFile));
				}
			}
		}
	}

	@Override
	protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
		return true;
	}
}
