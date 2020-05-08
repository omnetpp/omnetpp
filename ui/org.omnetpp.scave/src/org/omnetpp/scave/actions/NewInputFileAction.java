/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.Arrays;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Adds an item to the inputs.
 */
public class NewInputFileAction extends AbstractScaveAction {
    public NewInputFileAction() {
        setText("New Input...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_NEWINPUTFILE));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IContainer baseDir = ((FileEditorInput)editor.getEditorInput()).getFile().getParent();
        InputFileDialog dialog = new InputFileDialog(editor.getSite().getShell(), "Create Input", "", true, baseDir);
        if (dialog.open() == Window.OK) {
            String value = dialog.getValue();
            List<String> list = Arrays.asList(value.split(" *; *"));
            ScaveModelUtil.addInputFiles(editor.getInputsPage().getCommandStack(), editor.getAnalysis(), list);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}
