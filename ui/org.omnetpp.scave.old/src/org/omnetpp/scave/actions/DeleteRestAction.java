/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;

import org.omnetpp.scave.editors.DatasetEditor;


public class DeleteRestAction extends Action {

    private final IWorkbenchWindow window;

    public DeleteRestAction(IWorkbenchWindow window) {
        this.window = window;
        // The id is used to refer to the action in a menu or toolbar
        setId(ICommandIds.CMD_DELETE_REST);

        // Associate the action with a pre-defined command, to allow key bindings.
        // TODO this would probably need a command definition in plugin.xml? (Otherwise we get "undefined command" exception in the log)
        //setActionDefinitionId(ICommandIds.CMD_DELETE_REST);

        // Set other properties
        setText("Delete Rest");
        setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(IDE.SharedImages.IMG_OBJ_PROJECT_CLOSED)); // FIXME icon
        //setImageDescriptor(ScavePlugin.getImageDescriptor("/icons/sample.gif")); --THIS FILE DOESN'T EXIST
        setToolTipText("Keep only selected items (or shown items if none are selected)in the dataset");
        setAccelerator(SWT.CTRL|SWT.SHIFT|SWT.DEL);
    }

    public void run() {
        IEditorPart editor = window.getActivePage().getActiveEditor();
        if (editor instanceof DatasetEditor) {
            DatasetEditor dsEditor = (DatasetEditor)editor;
            dsEditor.getFilterPanel().deleteRest();
        }
    }
}
