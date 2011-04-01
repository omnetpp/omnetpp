package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.SubProgressMonitor;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.Debug;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.ParamResolutionStatus.Entry;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * Job for performing the parameter resolution asynchronously.
 * 
 * It works on a read-only copy of the ini file and returns the result in its status.
 * 
 * The actual work is factored out to the ParamCollector class, so
 * they can be called directly too.
 * 
 * @author tomi
 */
class ParamResolutionJob extends Job {
    // cancel status codes
    public final static int USER_CANCELED = 0;
    public final static int DOC_CHANGED   = 1;

    private IInifileDocument doc;
    private int retryCount;
    
    public ParamResolutionJob(IInifileDocument doc) {
        super("Ini file analysis");
        this.doc = doc;
        setPriority(SHORT);
    }
    
    @Override
    protected IStatus run(IProgressMonitor monitor) {
        IStatus status = tryRun(monitor);
        if (status.isOK()) {
            retryCount = 0;
            return status;
        }
        if (status.getSeverity() == IStatus.CANCEL) {
            if (status.getCode() == USER_CANCELED)
                retryCount = 0;
            else {
                // stopped because NED or ini changed under us -- retry after some delay
                long delay = 200 * (1<<Math.min(retryCount++, 4)); // exponential backoff up to 3200ms
                Debug.println("ParamResolutionJob: will retry in " + delay + "ms");
                schedule(delay);
            }
        }
        return status;
    }
    
    private IStatus tryRun(IProgressMonitor monitor) {
        final IReadonlyInifileDocument docCopy = doc.getImmutableCopy();
        final INedResources nedResources = NedResourcesPlugin.getNedResources();
        final INedTypeResolver nedResolver = nedResources.getImmutableCopy(); 
        
        List<Entry> entries = new ArrayList<Entry>();
        String[] sections = docCopy.getSectionNames();
        monitor.beginTask("Analyzing " + docCopy.getDocumentFile().getName(), sections.length * 100);
        
        long startTime = System.currentTimeMillis();
        
        class SubMonitor extends SubProgressMonitor {
            public SubMonitor(IProgressMonitor monitor, int ticks) {
                super(monitor, ticks);
            }

            @Override
            public boolean isCanceled() {
                return super.isCanceled() || !doc.isImmutableCopyUpToDate(docCopy) || !nedResources.isImmutableCopyUpToDate(nedResolver);
            }
        };
        
        try {
            for (String activeSection : sections) {
                if (monitor.isCanceled() || !doc.isImmutableCopyUpToDate(docCopy) || !nedResources.isImmutableCopyUpToDate(nedResolver))
                    throw new OperationCanceledException();
                
                monitor.subTask("Resolving module parameters and properties.");
                Entry entry =
                    ParamCollector.collectParametersAndProperties(docCopy, nedResolver, activeSection, new SubMonitor(monitor, 100));
                entries.add(entry);
            }

        } catch (OperationCanceledException e) {
            if (monitor.isCanceled()) {
                Debug.println("ParamResolutionJob: canceled after "+(System.currentTimeMillis()-startTime)+"ms");
                return new Status(IStatus.CANCEL, InifileEditorPlugin.PLUGIN_ID, USER_CANCELED, "", null);
            }
            else {
                Debug.println("ParamResolutionJob: stopped because input changed after "+(System.currentTimeMillis()-startTime)+"ms");
                return new Status(IStatus.CANCEL, InifileEditorPlugin.PLUGIN_ID, DOC_CHANGED, "", null);
            }
        } finally {
            monitor.done();
        }
        
        Debug.println("ParamResolutionJob: completed in "+(System.currentTimeMillis()-startTime)+"ms");
        return new ParamResolutionStatus(entries, docCopy, nedResolver);
    }
}
