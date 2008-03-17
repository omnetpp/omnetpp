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
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;

public class NedEditorLauncher implements IEditorLauncher {

    public void open(IPath filePath) {
        IWorkbench workbench = PlatformUI.getWorkbench();
        IWorkbenchWindow workbenchWindow = workbench.getActiveWorkbenchWindow();
        IWorkbenchPage page = workbenchWindow.getActivePage();
        try {
            IFile file = getNedFile(filePath);
            NEDResources res = NEDResourcesPlugin.getNEDResources();
            if (!ProjectUtils.isOpenOmnetppProject(file.getProject())) {
                IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                MessageDialog.openInformation(workbenchWindow.getShell(),
                        "Not an OMNeT++ Project",
                        "NED Files can be graphically edited only if the Project is" +
                        " an OMNeT++ Project. Please use the Convert to OMNeT++ Project " +
                        "Wizard to convert this project.");
                return;
            }
            if (res.getNedSourceFolderFor(file) == null) {
                IDE.openEditor(page, file, EditorsUI.DEFAULT_TEXT_EDITOR_ID);
                MessageDialog.openInformation(workbenchWindow.getShell(),
                        "NED file is not in source folder",
                        "This NED file is not under a NED Source Folder, and" +
                        " will be opened in a standard text editor. If you want to edit it graphically, " +
                        "please move it into a NED Source Folder. NED Source Folders " +
                        "can be configured in the Project Properties dialog.");
                return;
            }
            // open graphically
            IDE.openEditor(page, file, NedEditor.ID);
        } catch (CoreException e) {
            NedEditorPlugin.logError(e);
        }
    }

    private IFile getNedFile(IPath path) throws CoreException {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        IFile[] resultFiles = root.findFilesForLocation(path);

        if (resultFiles.length > 0)
            return resultFiles[0];
        else
            return null;

    }
}
