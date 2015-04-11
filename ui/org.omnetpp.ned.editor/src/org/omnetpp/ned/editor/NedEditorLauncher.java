/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IEditorLauncher;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.editors.text.EditorsUI;
import org.eclipse.ui.ide.IDE;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;

public class NedEditorLauncher implements IEditorLauncher {

    public void open(IPath filePath) {
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
        IWorkbenchPage page = workbenchWindow.getActivePage();
        try {
            IFile file = getFileFor(filePath);
            if (file == null) {
                MessageDialog.openInformation(workbenchWindow.getShell(),
                        "NED File Not Found",
                        "NED file has not been found. Please refresh the workspace.");
                return;
            }
            INedResources res = NedResourcesPlugin.getNedResources();
            if (!ProjectUtils.isOpenOmnetppProject(file.getProject())) {
                boolean addNature = MessageDialog.openQuestion(workbenchWindow.getShell(),
                        "Not an OMNeT++ Project",
                        "NED files can be graphically edited only if the project is " +
                        "an OMNeT++ Project. Do you want add OMNeT++ support to the project?\n\n" +
                        "NOTE: This operation will NOT add C++ development support to the project -- " +
                        "if you want to be able to edit sources and rebuild binaries, it is recommended " +
                        "that you delete this project from the workspace, and recreate it with the " +
                        "New OMNeT++ Project wizard.");
                if (addNature) {
                    ProjectUtils.addOmnetppNature(file.getProject(), null);
                    if (!ProjectUtils.isOpenOmnetppProject(file.getProject())) {
                        MessageDialog.openInformation(workbenchWindow.getShell(),
                                "Not an OMNeT++ Project",
                                "Conversion unsuccessful.");
                        IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                    }
                    else {
                        IDE.openEditor(page, file, NedEditor.ID);
                    }
                }
                else {
                    // open in text editor
                    IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                }
                return;
            }
            if (!res.isNedFile(file)) {
                IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                MessageDialog.openInformation(workbenchWindow.getShell(),
                        "Not a NED File",
                        "This NED file is not under a NED source folder or is in an excluded NED package, " +
                        "and will be opened in a standard text editor. If you want to edit it graphically, " +
                        "please move it into a NED source folder or remove package exclusion. " +
                        "NED Source Folders and package exclusions can be configured in the Project Properties dialog.");
                return;
            }
            // open graphically
            IDE.openEditor(page, file, NedEditor.ID);
        } catch (CoreException e) {
            NedEditorPlugin.logError(e);
        }
    }

    @SuppressWarnings("deprecation")
    private IFile getFileFor(IPath path) throws CoreException {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IFile[] files = root.findFilesForLocation(path);
        // choose the most inner one
        IFile foundFile = null;
        for (IFile file : files) {
            if (foundFile == null || file.getFullPath().segmentCount() < foundFile.getFullPath().segmentCount())
                foundFile = file;
        }
        return foundFile;
    }
}
