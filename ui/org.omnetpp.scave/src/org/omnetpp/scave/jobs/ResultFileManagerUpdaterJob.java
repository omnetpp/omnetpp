package org.omnetpp.scave.jobs;

import static org.omnetpp.scave.common.IndexFileUtils.isExistingVectorFile;
import static org.omnetpp.scave.common.IndexFileUtils.isIndexFileUpToDate;
import static org.omnetpp.scave.common.ScaveMarkers.MARKERTYPE_SCAVEPROBLEM;
import static org.omnetpp.scave.common.ScaveMarkers.deleteMarkers;
import static org.omnetpp.scave.common.ScaveMarkers.setMarker;

import java.util.Queue;
import java.util.concurrent.Callable;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.IJobChangeEvent;
import org.eclipse.core.runtime.jobs.ISchedulingRule;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.core.runtime.jobs.JobChangeAdapter;
import org.eclipse.core.runtime.jobs.MultiRule;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.builder.Activator;
import org.omnetpp.scave.common.IndexFileUtils;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.ResultFileFormatException;
import org.omnetpp.scave.engineext.ResultFileManagerEx;

public class ResultFileManagerUpdaterJob extends Job {

    private static final boolean debug = true;

    private enum Operation {
        Load,
        Unload
    };

    private static class Task {
        Operation operation;
        IFile file;

        public Task(Operation operation, IFile file) {
            this.operation = operation;
            this.file = file;
        }
    }

    private ResultFileManager manager;
    private Queue<Task> tasks = new ConcurrentLinkedQueue<Task>();

    public ResultFileManagerUpdaterJob(ResultFileManager manager) {
        super("Loading files");
        this.manager = manager;
    }

    public void load(IFile file) {
        tasks.offer(new Task(Operation.Load, file));
        if (getState() == NONE)
            schedule();
    }

    public void unload(IFile file) {
        tasks.offer(new Task(Operation.Unload, file));
        if (getState() == NONE)
            schedule();
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        try {
            Task task;
            while ((task = tasks.peek()) != null) {
                if (monitor.isCanceled())
                    return Status.CANCEL_STATUS;
//              if (isInterrupted())
//                  break;

                task = tasks.poll();
                IFile file = task.file;
                ISchedulingRule rule = getSchedulingRuleFor(file);

                try {
                    Job.getJobManager().beginRule(rule, null);
                    switch (task.operation) {
                    case Load: doLoad(file); break;
                    case Unload: doUnload(file); break;
                    }

                }
                catch (Exception e) {
                    Activator.logError(e);
                }
                finally {
                    Job.getJobManager().endRule(rule);
                }
            }
            return Status.OK_STATUS;
        }
        finally {
            monitor.done();
        }
    }

    /**
     * Loads the specified <code>file</code> into the ResultFileManager.
     * If a vector file is loaded, it checks that the index file is up-to-date.
     * When not, it generates the index first and then loads it from the index.
     */
    public void doLoad(final IResource resource) {
        if (debug) Debug.format("  load: %s ", resource);

        if (resource.getLocation().toFile().exists()) {
            Exception exception = null;
            try {
                ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                    public Object call() throws Exception {
                        // Do not try to load from the vector file whose index is not up-to-date,
                        // because the ResultFileManager loads it from the vector file and it takes too much time
                        // for ~100MB files.
                        // Create or update the index file first, and try again.
                        if (resource instanceof IFile) {
                            IFile file = (IFile)resource;
                            if (isExistingVectorFile(file) && !isIndexFileUpToDate(file))
                                generateIndexAndLoad(file);
                            else
                                loadInternal(resource);
                        }
                        else
                            loadInternal(resource);
                        return null;
                    }
                });
            }
            catch (Exception e) {
                exception = e;
            }

            if (resource instanceof IFile) {
                IFile file = (IFile)resource;
                updateMarkers(file, exception);
            }
        }
    }

    public void doUnload(final IFile file) {
        if (debug) Debug.format("  unloadFile: %s%n ", file);
        ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
            public Object call() throws Exception {
                String resourcePath = file.getFullPath().toString();
                ResultFile resultFile = manager.getFile(resourcePath);
                if (resultFile != null) {
                    manager.unloadFile(resultFile);
                    if (debug) Debug.println("done");
                }
                return null;
            }
        });
    }

    private void generateIndexAndLoad(final IFile file) {
        if (debug) Debug.format("indexing: %s%n", file);
        VectorFileIndexerJob indexer = new VectorFileIndexerJob("Indexing "+file, new IFile[] {file});
        indexer.addJobChangeListener(new JobChangeAdapter() {
            public void done(IJobChangeEvent event) {
                // load from the newly created index file
                    // even if the workspace is not refreshed automatically
                if (event.getResult().getSeverity() != IStatus.ERROR) {
                    ISchedulingRule rule = getSchedulingRuleFor(file);
                    try {
                        Job.getJobManager().beginRule(rule, null);
                        loadInternal(file);
                    }
                    finally {
                        Job.getJobManager().endRule(rule);
                    }
                }
                else {
                    doUnload(file);
                }
            }
        });
        indexer.schedule();
    }

    private void loadInternal(IResource file) {
        final String resourcePath = file.getFullPath().toString();
        final String osPath = file.getLocation().toOSString();
        if (file.getLocation().toFile().isDirectory())
            ;//manager.loadDirectory(resourcePath, osPath, true); //TODO
        else {
            int flags = ResultFileManagerEx.RELOAD_IF_CHANGED | ResultFileManagerEx.ALLOW_INDEXING | ResultFileManagerEx.SKIP_IF_LOCKED;
            manager.loadFile(resourcePath, osPath, "TODO", flags, null); //TODO
        }
        if (debug) Debug.println("done");
    }

    private void updateMarkers(IFile file, Exception e) {
        if (debug && e != null)
            Debug.format("exception: %s ", e);

        if (e == null) {
            deleteMarkers(file, MARKERTYPE_SCAVEPROBLEM);
        }
        else if (e instanceof ResultFileFormatException) {
            ScavePlugin.logError("Wrong file: " + file.getLocation().toOSString(), e);
            ResultFileFormatException fileFormatException = (ResultFileFormatException)e;
            if (isExistingVectorFile(file)) {
                IFile indexFile = IndexFileUtils.getIndexFileFor(file);
                String message = fileFormatException.getMessage();
                int lineNo = fileFormatException.getLineNo();
                setMarker(indexFile, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Wrong file: "+message, lineNo);
                setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_WARNING, "Could not load file. Reason: "+message, -1);
            }
            else {
                String message = fileFormatException.getMessage();
                int lineNo = fileFormatException.getLineNo();
                setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Wrong file: "+message, lineNo);
            }

        }
        else {
            ScavePlugin.logError("Could not load file: " + file.getLocation().toOSString(), e);
            setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Could not load file. Reason: "+e.getMessage(), -1);
        }
    }

    private ISchedulingRule getSchedulingRuleFor(IFile file) {
        if (isExistingVectorFile(file))
            return MultiRule.combine(file, IndexFileUtils.getIndexFileFor(file));
        else
            return file;
    }
}
