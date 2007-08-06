package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspaceRunnable;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.ned.engine.NEDErrorCategory;
import org.omnetpp.ned.engine.NEDErrorStore;
import org.omnetpp.ned.model.INEDElement;

/**
 * A marker synchronizer specialized to sync NED parsing and consistency problems. Can accept error stores
 * received from the NED parser.
 * @author rhornig
 */
public class NEDProblemMarkerSynchronizer extends ProblemMarkerSynchronizer {
    public static final String NEDPROBLEM_MARKERID = "org.omnetpp.ned.core.nedproblem";
    public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.ned.core.nedconsistencyproblem";

    /**
     * Generic constructor. Synchronizes ALL problem markers.
     */
    public NEDProblemMarkerSynchronizer() {
        super();
    }

    /**
     * @param markerBaseType The marker type that should be synchronized. the rest type remains untouched.
     */
    public NEDProblemMarkerSynchronizer(String markerBaseType) {
        super(markerBaseType);
    }

    /**
     * Adds all markers found in the error store to the file for later synchronization;
     * @param file
     * @param errors
     */
    public void addMarkersToFileFromErrorStore(IFile file, NEDErrorStore errors) {
        INEDElement context = NEDResourcesPlugin.getNEDResources().getNEDFileModel(file);

        if (errors == null)
            return;

        for (int i = 0; i < errors.numMessages(); i++) {
            // XXX hack: parse out line number from string. NEDErrorStore should
            // rather store line number as int...
            String loc = errors.errorLocation(i);
            int markerSeverity = IMarker.SEVERITY_INFO;
            NEDErrorCategory category = NEDErrorCategory.swigToEnum(errors.errorCategoryCode(i));
            switch (category) {
                case ERRCAT_FATAL:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
                case ERRCAT_ERROR:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
                case ERRCAT_WARNING:
                    markerSeverity = IMarker.SEVERITY_WARNING;
                    break;
                case ERRCAT_INFO:
                    markerSeverity = IMarker.SEVERITY_INFO;
                    break;
                default:
                    markerSeverity = IMarker.SEVERITY_ERROR;
                    break;
            }
            addMarker(file, context, NEDPROBLEM_MARKERID, markerSeverity, errors.errorText(i), loc);
        }
    }

    /**
     * Adds a marker to be synchronized
     * @param file The file in which the problem found
     * @param element The NEDElement the error contains (or null if N/A)
     * @param type
     * @param severity
     * @param message
     * @param location is file
     */
    public void addMarker(IFile file, INEDElement element, String type, int severity, String message, String loc) {
        Map<String, Object> markerAttrs = new HashMap<String, Object>();
        markerAttrs.put(IMarker.MESSAGE, message);
        markerAttrs.put(IMarker.SEVERITY, severity);
        markerAttrs.put(IMarker.LINE_NUMBER, parseLineNumber(loc));
        addMarker(file, type, markerAttrs);

        // mark all element up to the root (including the file itself, as invalid)
        for (INEDElement e = element; e != null; e = e.getParent()) {
            e.setValid(false);
        }
    }

    private int parseLineNumber(String loc) {
        if (loc == null) return 1;
        StringTokenizer t = new StringTokenizer(loc,":");
        while (t.hasMoreTokens()) loc = t.nextToken();
        int line = 1;
        try {line = Integer.parseInt(loc);} catch (Exception e) {}
        return line;
    }

    /**
     * Defers the synchronization to a later time when the resource tree is no longer locked.
     * You should use this method, if the current thread locks the workspace resources
     * (ie we are in a resource change notification)
     */
    public void runAsWorkspaceJob() {
        Job job = new Job("Updating problem markers") {
            @Override
            public IStatus run(IProgressMonitor monitor) {
                try {
                    ResourcesPlugin.getWorkspace().run(new IWorkspaceRunnable() {
                        public void run(IProgressMonitor monitor) throws CoreException {
                            addRemoveMarkers();
                        }
                    }, null);
                } catch (CoreException e) {
                    return Status.CANCEL_STATUS;
                }
                return Status.OK_STATUS;
            }
        };
        job.setSystem(true);
        job.schedule();
        job.setPriority(Job.INTERACTIVE);
    }
}
