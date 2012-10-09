package org.omnetpp.ned.core;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;


/**
 * Validates all NED files for consistency (no such parameter/gate/module-type, redeclarations,
 * duplicate types, cycles in the inheritance chain, etc). All consistency problem markers
 * (NEDCONSISTENCYPROBLEM_MARKERID) are managed within this job.
 */
public class NedValidationJob extends Job {
    private boolean validateOpenNedFilesOnly = false; //TODO such preference might be useful for netbooks
    private int retryCount = 0;

    public NedValidationJob() {
        super("Checking NED files...");
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        // validate
        boolean success = tryValidate(monitor);
        if (success) {
            retryCount = 0;
            return Status.OK_STATUS;
        }
        if (monitor.isCanceled()) {
            retryCount = 0;
            return Status.CANCEL_STATUS;
        }

        // stopped because NED changed under us -- retry after some delay
        long delay = 200 * (1<<Math.min(retryCount++, 4)); // exponential backoff up to 3200ms
        Debug.println("NED validation job: will retry in " + delay + "ms");
        schedule(delay);
        return Status.CANCEL_STATUS;
    }

    /**
     * Validates an immutable copy of NedResources; stop immediately if the copy becomes obsolete
     */
    protected boolean tryValidate(IProgressMonitor monitor) {
        // Note: no try-catch here -- we let exceptions terminate the job and be logged by the platform
        long startTime = System.currentTimeMillis();
        Debug.println("NED validation job started working");

        INedResources nedResources = NedResourcesPlugin.getNedResources();

        int numWorkUnits = 3 + nedResources.getNedFiles().size();
        monitor.beginTask(getName(), numWorkUnits);

        // we'll analyze an immutable copy, so that NedResources doesn't need to be locked
        INedTypeResolver immutableResolver = nedResources.getImmutableCopy();
        monitor.worked(1);

        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(INedTypeResolver.NEDCONSISTENCYPROBLEM_MARKERID);
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(markerSync);

        // clear old consistency error markers from NED trees
        clearConsistencyProblemSeverities(immutableResolver);

        // issue error message for duplicates
        immutableResolver.addErrorsForDuplicates(errorStore);
        monitor.worked(1);

        // validate all files
        for (IFile file : immutableResolver.getNedFiles()) {
            NedFileElementEx nedFileElement = immutableResolver.getNedFileElement(file);
            errorStore.setFile(file);
            new NedValidator(immutableResolver, file.getProject(), errorStore).validate(nedFileElement);
            monitor.worked(1);

            if (!nedResources.isImmutableCopyUpToDate(immutableResolver)) {
                Debug.println("NED validation job: NED trees changed meanwhile, abandoning after " + (System.currentTimeMillis()-startTime) + "ms of work");
                return false;
            }
            if (monitor.isCanceled()) {
                Debug.println("NED validation job: cancelled after " + (System.currentTimeMillis()-startTime) + "ms of work");
                return false;
            }
        }

        // lock NedResources while we copy marker severities onto NED trees in it
        synchronized (nedResources) {
            if (!nedResources.isImmutableCopyUpToDate(immutableResolver)) {
                Debug.println("NED validation job: discarding results due to NED changes");
                return false;
            }
            try {
                long midTime = System.currentTimeMillis();

                nedResources.fireBeginChangeEvent();

                // clear old consistency error markers from NED trees
                clearConsistencyProblemSeverities(nedResources);

                // put validation errors
                for (INedElement element : errorStore.getAffectedElements()) {
                    INedElement original = element.getOriginal();
                    original.setConsistencyProblemMaxLocalSeverity(element.getConsistencyProblemMaxLocalSeverity());
                }

                // we need to do the synchronization in a background job, to avoid deadlocks
                markerSync.runAsWorkspaceJob();

                Debug.println("NED validation job completed: spent " + (midTime-startTime) + "ms in background, " + (System.currentTimeMillis()-midTime) + "ms blocking, " + markerSync.getNumberOfMarkers() + " markers on " + markerSync.getNumberOfFiles() + " files");
                monitor.worked(1);

                return true;
            }
            finally {
                nedResources.fireEndChangeEvent();
            }
        }
    }

    private void clearConsistencyProblemSeverities(INedTypeResolver resolver) {
        for (IFile file : resolver.getNedFiles())
            resolver.getNedFileElement(file).clearConsistencyProblemMarkerSeverities();
    }

}
