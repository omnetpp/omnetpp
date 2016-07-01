/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.cdt.Activator;

/**
 * Removes generated _m.cc/h files when their msg file is deleted.
 *
 * @author Andras
 */
public class MsgFileRemoveDetector implements IResourceChangeListener {
    @Override
    public void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null)
                return;
            final List<IFile> removedMsgFiles = new ArrayList<IFile>();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    if (delta.getKind() == IResourceDelta.REMOVED && MakefileTools.isMsgFile(delta.getResource()))
                        removedMsgFiles.add((IFile)delta.getResource());
                    return true;
                }
            });
            if (!removedMsgFiles.isEmpty())
                followChanges(removedMsgFiles);
        }
        catch (CoreException e) {
            Activator.logError("Error during workspace change notification: ", e);
        }
    }

    protected void followChanges(final List<IFile> removedMsgFiles) {
        WorkspaceJob job = new WorkspaceJob("Follow folder renames") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                doFollowChanges(removedMsgFiles, monitor);
                return Status.OK_STATUS;
            }
        };

        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.SHORT);
        job.setSystem(true);
        job.schedule();
    }

    protected void doFollowChanges(List<IFile> removedMsgFiles, IProgressMonitor monitor) {
        for (IFile f : removedMsgFiles) {
            IContainer container = f.getParent();
            if (container.exists()) {
                String nameBase = f.getName().replaceAll("\\.msg$", "");
                deleteIfExists(container.getFile(new Path(nameBase + "_m.h")), monitor);
                deleteIfExists(container.getFile(new Path(nameBase + "_m.cc")), monitor);
                deleteIfExists(container.getFile(new Path(nameBase + "_m.cpp")), monitor);
            }
        }

    }

    protected void deleteIfExists(IFile file, IProgressMonitor monitor) {
        try {
            if (file.exists())
                file.delete(true, monitor);
        } catch (CoreException e) {
            Activator.logError("Cannot delete generated file " + file.getFullPath(), e);
        }
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }
}
