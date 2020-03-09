/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.jobs;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.core.runtime.jobs.ISchedulingRule;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.MultiRule;
import org.omnetpp.scave.common.IndexFileUtils;

/**
 * This job generates index files for vector files in the workspace.
 *
 * @author tomi
 */
public class VectorFileIndexerJob extends WorkspaceJob {

    private List<IFile> filesToBeIndexed;

    public VectorFileIndexerJob(String name, IFile[] filesToBeIndexed) {
        super(name);

        ArrayList<ISchedulingRule> rule = new ArrayList<ISchedulingRule>();
        this.filesToBeIndexed = new ArrayList<IFile>();
        for (IFile file : filesToBeIndexed)
            if (IndexFileUtils.isExistingVectorFile(file)) {
                this.filesToBeIndexed.add(file);
                rule.add(file);
                rule.add(IndexFileUtils.getIndexFileFor(file));
            }

        setRule(MultiRule.combine(rule.toArray(new ISchedulingRule[rule.size()])));
        setPriority(Job.LONG);
    }

    /**
     * Generate indeces for files in the queue.
     */
    @Override
    public IStatus runInWorkspace(IProgressMonitor monitor)
            throws CoreException {

        if (!filesToBeIndexed.isEmpty()) {
            try {
                monitor.beginTask(getName(), filesToBeIndexed.size());

                for (IFile file : filesToBeIndexed) {
                    if (monitor.isCanceled())
                        return Status.CANCEL_STATUS;

                    monitor.subTask("Indexing "+file.getName());
                    if (file.exists() && IndexFileUtils.isExistingVectorFile(file) && !IndexFileUtils.isIndexFileUpToDate(file)) {
                        IProgressMonitor subMonitor = new SubProgressMonitor(monitor, 1);
                        IndexFileUtils.performIndexing(file, subMonitor);
                        if (subMonitor.isCanceled())
                            return Status.CANCEL_STATUS;
                    }
                }
            }
            finally {
                monitor.done();
            }
        }
        return Status.OK_STATUS;
    }
}
