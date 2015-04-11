/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.util.StringUtils;

/**
 * Detects if a CDT project has an old ".cproject" file, and tells the user
 * to re-create the project.
 *
 * @author Andras
 */
public class CProjectChecker {
    // listener invokes checkAllOpenProjects() when a project is opened (note: if many projects are opened together, we get one batch notification)
    protected IResourceChangeListener listener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            Assert.isTrue(event.getType() == IResourceChangeEvent.POST_CHANGE);
            try {
                final boolean[] haveNewlyOpenedProjects = new boolean[1];
                haveNewlyOpenedProjects[0] = false;
                event.getDelta().accept(new IResourceDeltaVisitor() {
                    public boolean visit(IResourceDelta delta) throws CoreException {
                        IResource resource = delta.getResource();
                        if (resource instanceof IProject && resource.isAccessible()) {
                            if ((delta.getKind() == IResourceDelta.ADDED || delta.getKind() == IResourceDelta.CHANGED) && (delta.getFlags() & IResourceDelta.OPEN) != 0)
                                haveNewlyOpenedProjects[0] = true;
                            return false;
                        }
                        return true;
                    }
                });
                if (haveNewlyOpenedProjects[0])
                    checkAllOpenProjects();
            }
            catch (CoreException e) {
                Activator.logError(e);
            }
        }
    };

    public CProjectChecker() {
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(listener, IResourceChangeEvent.POST_CHANGE);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(listener);
    }

    public void checkAllOpenProjects() {
        IProject[] projects = ResourcesPlugin.getWorkspace().getRoot().getProjects();
        List<IProject> outOfDateProjects = new ArrayList<IProject>();
        for (IProject project : projects) {
            if (project.isAccessible() && containsOldCProjectFile(project))
                outOfDateProjects.add(project);
        }
        if (!outOfDateProjects.isEmpty())
            warnUserAsync(outOfDateProjects);
    }

    protected boolean containsOldCProjectFile(IProject project) {
        IFile cprojectFile = project.getFile(".cproject");
        try {
            if (!project.getWorkspace().isTreeLocked())
                cprojectFile.refreshLocal(IResource.DEPTH_ZERO, null);
            if (cprojectFile.exists()) {
                // old files can be recognized because the <cproject> tag (the root element)
                // doesn't have a storage_type_id attribute yet.
                InputStream is = cprojectFile.getContents();
                byte[] buffer = new byte[1024];
                int n = is.read(buffer);
                String contents = new String(buffer, 0, n, "UTF-8");
                if (contents.contains("<?fileVersion 4.0.0?>") && !contents.contains("<cproject storage_type_id="))
                    return true;
            }
        }
        catch (Exception e) { // IOException, CoreException
            Activator.logError("Error checking whether " + cprojectFile.getFullPath() + " is compatible with CDT 8.0", e);
        }
        return false;   // .cproject is OK, or we couldn't check (exception)
    }

    protected void warnUserAsync(final List<IProject> outOfDateProjects) {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                Shell parent = activeWorkbenchWindow==null ? null : activeWorkbenchWindow.getShell();

                String[] projectNames = new String[outOfDateProjects.size()];
                for (int i=0; i<projectNames.length; i++)
                    projectNames[i] = outOfDateProjects.get(i).getName();

                MessageDialog.openWarning(parent,
                        "Out-of-Date CDT Project Files Detected",
                        "The following project(s) contain an old CDT project file (.cproject):\n\n" +
                        "- " + StringUtils.join(projectNames, ", ") + "\n\n" +
                        "We recommend that you re-create the projects, as old project files may cause " +
                        "problems for the C++ indexer. You can re-create a project by deleting it " +
                        "(answer \"No\" when asked if you want to delete the files as well), and creating " +
                        "a new project in the same directory using File -> New -> OMNeT++ Project -- " +
                        "this will overwrite the project settings while leaving contents intact.");
            }
        });
    }
}
