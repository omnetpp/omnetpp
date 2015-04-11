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


public class ExtendDatasetAction extends Action {

    private final IWorkbenchWindow window;

    public ExtendDatasetAction(IWorkbenchWindow window) {
        this.window = window;
        // The id is used to refer to the action in a menu or toolbar
        setId(ICommandIds.CMD_EXTEND_DATASET);

        // Associate the action with a pre-defined command, to allow key bindings.
        // TODO this would probably need a command definition in plugin.xml? (Otherwise we get "undefined command" exception in the log)
        //setActionDefinitionId(ICommandIds.CMD_EXTEND_DATASET);

        // Set other properties
        setText("Extend Dataset...");
        setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(IDE.SharedImages.IMG_OBJS_TASK_TSK)); // FIXME icon
        //setImageDescriptor(ScavePlugin.getImageDescriptor("/icons/sample.gif")); --THIS FILE DOESN'T EXIST
        setToolTipText("Add new items to current dataset");
        setAccelerator(SWT.INSERT);
    }

    public void run() {
        IEditorPart editor = window.getActivePage().getActiveEditor();
        if (editor instanceof DatasetEditor) {
            DatasetEditor dsEditor = (DatasetEditor)editor;
            dsEditor.getFilterPanel().extendDataset();
        }
    }
}
