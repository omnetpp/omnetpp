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
    public NedValidationJob() {
        super("Checking NED files...");
    }

    @Override
    protected IStatus run(IProgressMonitor monitor) {
        long sleepMillis = 200;
        while (true) {
            // try validating an immutable copy (stop if it becomes obsolete)
            if (tryValidate(monitor))
                return Status.OK_STATUS;  // successful
            if (monitor.isCanceled())
                return Status.CANCEL_STATUS;

            // sleep a little, then try again
            Debug.println("NED validation job: sleeping " + sleepMillis + "ms before trying again");
            try { Thread.sleep(sleepMillis); } catch (InterruptedException e) { }
            sleepMillis = Math.min(2*sleepMillis, 4000); // back off up to 4s
            if (monitor.isCanceled())
                return Status.CANCEL_STATUS;
        }
    }


    protected boolean tryValidate(IProgressMonitor monitor) {
        // Note: no try-catch here -- we let exceptions terminate the job and be logged by the platform 
        long startTime = System.currentTimeMillis();
        Debug.println("NED validation job started working");

        INedResources nedResources = NedResourcesPlugin.getNedResources();

        // we'll analyze an immutable copy, so that NedResources doesn't need to be locked
        INedTypeResolver immutableResolver = nedResources.getImmutableCopy(); 

        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(INedTypeResolver.NEDCONSISTENCYPROBLEM_MARKERID);
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(markerSync);

        //TODO handle progress monitor!!!

        // clear old consistency error markers from NED trees
        clearConsistencyProblemSeverities(immutableResolver);
        
        // issue error message for duplicates
        immutableResolver.addErrorsForDuplicates(errorStore);

        // validate all files
        for (IFile file : immutableResolver.getNedFiles()) {
            NedFileElementEx nedFileElement = immutableResolver.getNedFileElement(file);
            errorStore.setFile(file);
            new NedValidator(immutableResolver, file.getProject(), errorStore).validate(nedFileElement);

            if (!nedResources.isImmutableCopyUpToDate(immutableResolver)) {
                Debug.println("NED validation job: NED trees changed meanwhile, abandoning after " + (System.currentTimeMillis()-startTime) + "ms");
                return false;
            }
        }

        // lock NedResources while we put on place error markers
        synchronized (nedResources) {
            if (nedResources.isImmutableCopyUpToDate(immutableResolver)) {
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
                    Debug.println("typeinfo: refreshLocalCount:" + NedTypeInfo.debugRefreshLocalCount + "  refreshInheritedCount:" + NedTypeInfo.debugRefreshInheritedCount);

                    return true;
                } 
                finally {
                    nedResources.fireEndChangeEvent();
                }
            }
        }
        Debug.println("NED validation job: results discarded due to NED changes");
        return false;
    }

    private void clearConsistencyProblemSeverities(INedTypeResolver resolver) {
        for (IFile file : resolver.getNedFiles())
            resolver.getNedFileElement(file).clearConsistencyProblemMarkerSeverities();
    }

}
