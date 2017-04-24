/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.Arrays;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Adds an item to the inputs.
 */
public class NewInputFileAction extends AbstractScaveAction {
    public NewInputFileAction() {
        setText("New Input...");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/newinputfile.png"));
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        IContainer baseDir = ((FileEditorInput)editor.getEditorInput()).getFile().getParent();
        InputFileDialog dialog = new InputFileDialog(editor.getSite().getShell(), "Create Input", "", true, baseDir);
        if (dialog.open() == Window.OK) {
            String value = dialog.getValue();
            List list = Arrays.asList(value.split(" *; *"));
            ScaveModelUtil.addInputFiles(editor.getEditingDomain(), editor.getAnalysis(), list);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return true;
    }
}
