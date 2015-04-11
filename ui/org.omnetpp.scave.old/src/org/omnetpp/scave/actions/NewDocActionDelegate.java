/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.PartInitException;

import org.omnetpp.scave.wizards.NonExistingFileEditorInput;

/**
 * Base class for actions that open a new editor window with a new document.
 */
public abstract class NewDocActionDelegate implements IWorkbenchWindowActionDelegate {
    private IWorkbenchWindow window;

    private ISelection selection;

    /* (non-Javadoc)
     * Method declared on IWorkbenchActionDelegate
     */
    public void dispose() {
        // do nothing
    }

    /* (non-Javadoc)
     * Method declared on IWorkbenchActionDelegate
     */
    public void init(IWorkbenchWindow window) {
        this.window = window;
    }

    /**
     * Opens an editor with a new filename.
     */
    public void run(IAction action) {
        // FIXME we ignore Action here -- shouldn't we?
        // FIXME sometimes window is null and createNewFile() crashes
        IFile file = ActionUtil.createNewFile(window, getNewFileName());
        if (file==null) {
            // TODO some error message or whatever
            return;
        }

        // try to find out default editor for this file
        IEditorDescriptor desc = window.getWorkbench().getEditorRegistry().getDefaultEditor(file.getName());
        if (desc==null) {
            // TODO some error message or whatever
            return;
        }
        String editorId = desc.getId(); //"org.omnetpp.scave.editors.ScalarDatasetEditor";

        // construct IEditorInput
        IFileEditorInput input = new NonExistingFileEditorInput(file);
        IWorkbenchPage page = window.getActivePage();

        try {
            page.openEditor(input, editorId);
        } catch (PartInitException e) {
            e.printStackTrace();  // TODO like this: EditorsPlugin.log(e);
        }
    }

    /**
     * Redefine to supply a file name, like "Untitled 3.txt"
     */
    protected abstract String getNewFileName();

    /**
     * Do nothing here - we will let simple rules in the XML
     * config file react to selections.
     */
    public void selectionChanged(IAction action, ISelection selection) {
        this.selection = selection;
    }
}
