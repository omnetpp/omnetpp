/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.project.ProjectUtils;

/**
 * @author Andras
 */
public class BuildSpecFolderRenameDetector implements IResourceChangeListener {
    protected static class CopiedFolder {
        IFolder folder;
        IPath copiedFrom;

        @Override
        public String toString() { return copiedFrom + " -> " + folder.getFullPath(); }
    }

    @Override
    public void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null)
                return;
            final List<CopiedFolder> copiedFolders = new ArrayList<CopiedFolder>();
            final List<IFolder> removedFolders = new ArrayList<IFolder>();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    if (resource instanceof IProject)
                        return ProjectUtils.isOpenOmnetppProject((IProject)resource); // only go into OMNeT++ projects
                    if (resource instanceof IFolder) {
                        IFolder folder = (IFolder)resource;
                        int kind = delta.getKind();
                        int flags = delta.getFlags();
                        if (kind == IResourceDelta.REMOVED)
                            removedFolders.add(folder);
                        else if (kind == IResourceDelta.ADDED && (flags & (IResourceDelta.MOVED_FROM|IResourceDelta.COPIED_FROM)) != 0) {
                            CopiedFolder copiedFolder = new CopiedFolder();
                            copiedFolder.folder = folder;
                            copiedFolder.copiedFrom = delta.getMovedFromPath();
                            copiedFolders.add(copiedFolder);
                        }
                    }
                    return true;
                }
            });
            if (!copiedFolders.isEmpty() || !removedFolders.isEmpty())
                followChanges(copiedFolders, removedFolders);
        }
        catch (CoreException e) {
            Activator.logError("Error during workspace change notification: ", e);
        }
    }

    protected void followChanges(final List<CopiedFolder> copiedFolders, final List<IFolder> removedFolders) {
        WorkspaceJob job = new WorkspaceJob("Follow folder renames") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                doFollowChanges(copiedFolders, removedFolders);
                return Status.OK_STATUS;
            }
        };

        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.SHORT);
        job.setSystem(true);
        job.schedule();
    }

    protected void doFollowChanges(List<CopiedFolder> copiedFolders, List<IFolder> removedFolders) throws CoreException {
        // configure copied folders
        IWorkspaceRoot wsRoot = ResourcesPlugin.getWorkspace().getRoot();
        while (!copiedFolders.isEmpty()) {
            // in each iteration, process folders in the project of the 1st folder
            IProject project = copiedFolders.get(0).folder.getProject();
            BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project);
            boolean changed = false;
            for (CopiedFolder copiedFolder : copiedFolders.toArray(new CopiedFolder[0])) {
                IFolder folder = copiedFolder.folder;
                if (folder.getProject().equals(project)) {
                    IFolder origFolder = wsRoot.getFolder(copiedFolder.copiedFrom);
                    if (origFolder.getProject().equals(project)) {  // ignore cross-project copies/moves, not worth the trouble
                        int type = buildSpec.getFolderMakeType(origFolder);
                        if (type != BuildSpecification.NONE) {
                            buildSpec.setFolderMakeType(folder, type);
                            if (type == BuildSpecification.MAKEMAKE)
                                buildSpec.setMakemakeOptions(folder, buildSpec.getMakemakeOptions(origFolder).clone());
                            changed = true;
                        }
                    }
                    copiedFolders.remove(copiedFolder); // done
                }
            }
            if (changed)
                buildSpec.save();
        }

        // remove deleted folders
        while (!removedFolders.isEmpty()) {
            // in each iteration, process folders in the project of the 1st folder
            IProject project = removedFolders.get(0).getProject();
            BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project);
            boolean changed = false;
            for (IFolder folder : removedFolders.toArray(new IFolder[0])) {
                if (folder.getProject().equals(project)) {
                    if (buildSpec.getFolderMakeType(folder) != BuildSpecification.NONE) {
                        buildSpec.setFolderMakeType(folder, BuildSpecification.NONE);
                        changed = true;
                    }
                    removedFolders.remove(folder); // done
                }
            }
            if (changed)
                buildSpec.save();
        }
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }
}
