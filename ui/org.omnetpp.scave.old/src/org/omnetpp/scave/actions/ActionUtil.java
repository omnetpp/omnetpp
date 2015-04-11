/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IFileEditorInput;
import org.eclipse.ui.IWorkbenchWindow;

public class ActionUtil {

    /**
     * Creates a new file in the folder/project currently selected in the
     * Navigator, or next to the file in the current editor.
     */
    public static IFile createNewFile(IWorkbenchWindow window, String fileName) {
        IFile file = null;

        // try to find a project or folder in the current selection, and new file will be child of that;
        // or if we find a file, we make it its sibling
        ISelection sel = window.getSelectionService().getSelection();
        if (sel instanceof IStructuredSelection) {
            IStructuredSelection structuredSel = (IStructuredSelection)sel;
            Object[] selArr = structuredSel.toArray();
            for (int i=0; i<selArr.length; i++) {
                Object o = selArr[i];
                //System.out.println("SELECTED: ("+o.getClass().getCanonicalName()+") "+o.toString());
                if (o instanceof IFolder) {
                    IFolder folder = (IFolder)o;
                    file = folder.getFile(fileName);
                    break;
                }
                if (o instanceof IProject) {
                    IProject proj = (IProject)o;
                    file = proj.getFile(fileName);
                    break;
                }
                if (o instanceof IFile) {
                    IFile selFile = (IFile)o;
                    IContainer container = selFile.getParent();
                    if (container instanceof IFolder) {
                        IFolder folder = (IFolder)container;
                        file = folder.getFile(fileName);
                        break;
                    }
                    if (container instanceof IProject) {
                        IProject proj = (IProject)container;
                        file = proj.getFile(fileName);
                        break;
                    }
                }
            }
        }

        // if the above didn't work out, try to derive filename from current editor
        if (file==null) {
            IEditorPart editor = window.getActivePage().getActiveEditor();
            IEditorInput input = editor.getEditorInput();
            if (input instanceof IFileEditorInput) {
                IFileEditorInput fileInput = (IFileEditorInput)input;
                IFile editorFile = fileInput.getFile();
                IContainer container = editorFile.getParent();
                if (container instanceof IFolder) {
                    IFolder folder = (IFolder)container;
                    file = folder.getFile(fileName);
                }
                else if (container instanceof IProject) {
                    IProject proj = (IProject)container;
                    file = proj.getFile(fileName);
                }
            }
        }

        // if we've found something after all, construct a FileEditorInput with it
        return file;
    }
}
