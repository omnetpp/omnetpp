/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.commands.CommandStack;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.SetInputFileCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Edits an InputFile item.
 */
public class EditInputFileAction extends AbstractScaveAction {
    public EditInputFileAction() {
        setText("Edit Input...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EDIT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        InputFile inputFile = (InputFile) asStructuredOrEmpty(selection).getFirstElement();
        IContainer baseDir = ((FileEditorInput)editor.getEditorInput()).getFile().getParent();
        InputFileDialog dialog = new InputFileDialog(editor.getSite().getShell(), "Edit Input", inputFile.getName(), false, baseDir);
        if (dialog.open() == Window.OK) {
            String value = dialog.getValue();
            ICommand command = new SetInputFileCommand(inputFile, value);
            editor.getInputsPage().getCommandStack().execute(command);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return asStructuredOrEmpty(selection).getFirstElement() instanceof InputFile;
    }
}
