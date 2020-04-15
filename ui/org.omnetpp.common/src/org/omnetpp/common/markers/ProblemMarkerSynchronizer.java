/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.markers;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.Debug;

/**
 * Solves the following problem: when an editor continuously analyzes
 * its contents (for example from a reconciler job) and converts
 * errors/warnings into markers, a common problem is that most markers
 * get deleted and added back each time the analyzer runs. This
 * triggers excessive refreshes in the UI (in text editor margin
 * and the Problems view).
 *
 * This class collects would-be markers in a table, and then
 * synchronizes this table to the actual IResource markers. This way,
 * only *real* marker changes reach the workspace, and excessive
 * updates are prevented.
 *
 * Methods take IResource not IFile, so one can add markers to
 * folders and projects as well, not only to files.
 *
 * ProblemMarkerSynchronizers are one-shot objects, they cannot be reused.
 * Clients need to create a new instance for every synchronization.
 *
 * @author Andras
 */
public class ProblemMarkerSynchronizer {
    private static boolean debug = true;

    protected static class MarkerData {
        final String type;
        final HashMap<String, Object> attrs;  // all instances must use the same Map implementation (e.g. HashMap), otherwise hash codes will be incompatible

        public MarkerData(String type, HashMap<String, Object> attrs) {
            super();
            Assert.isNotNull(type);
            Assert.isNotNull(attrs);
            this.type = type;
            this.attrs = attrs;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + attrs.hashCode();
            result = prime * result + type.hashCode();
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            MarkerData other = (MarkerData) obj;
            if (!attrs.equals(other.attrs))
                return false;
            if (!type.equals(other.type))
                return false;
            return true;
        }
    }

    // data for markers to synchronize
    protected HashMap<IResource, List<MarkerData>> markerTable = new HashMap<IResource, List<MarkerData>>();
    protected String markerBaseType;

    // statistics
    private int markersAdded = 0;
    private int markersRemoved = 0;

    /**
     * Creates a new marker synchronizer which will synchronize problem markers
     * (IMarker.PROBLEM) and its subclasses. Other marker types will remain untouched.
     */
    public ProblemMarkerSynchronizer() {
        this(IMarker.PROBLEM);
    }

    /**
     * Creates a new marker synchronizer which will synchronize the given marker type
     * and its subclasses. Other marker types will remain untouched.
     */
    public ProblemMarkerSynchronizer(String markerBaseType) {
        this.markerBaseType = markerBaseType;
    }

    /**
     * Include the given file (or other resource) in the synchronization process.
     * This is not needed when you call addMarker() for the file; however if there's
     * no addMarker() for that file, that file will be ignored (existing markers left
     * untouched) unless you register them with register().
     */
    public void register(IResource file) {
        Assert.isNotNull(file);
        if (!markerTable.containsKey(file))
            markerTable.put(file, new ArrayList<MarkerData>());
    }

    /**
     * Returns true if no files registered.
     */
    public boolean isEmpty() {
        return markerTable.isEmpty();
    }

    /**
     * Stores data for a marker to be added to the given file (or other resource).
     * Implies registerFile().
     */
    public void addMarker(IResource file, String markerType, Map<String, Object> markerAttrs) {
        register(file);

        HashMap<String, Object> markerAttrsHashMap = markerAttrs.getClass().equals(HashMap.class) ?
                (HashMap<String, Object>)markerAttrs : new HashMap<String, Object>(markerAttrs);
        MarkerData markerData = new MarkerData(markerType, markerAttrsHashMap);
        markerTable.get(file).add(markerData);
    }

    public String getBaseMarkerType() {
        return markerBaseType;
    }

    /**
     * Returns the number of files (or other resources) registered in this synchronizer.
     */
    public int getNumberOfFiles() {
        return markerTable.size();
    }

    /**
     * Returns the number of markers registered in this synchronizer.
     */
    public int getNumberOfMarkers() {
        int count = 0;
        for (IResource file : markerTable.keySet())
            count += markerTable.get(file).size();
        return count;
    }

    /**
     * Defers the synchronization to a later time when the resource tree is no longer locked.
     * You should use this method if the current thread locks the workspace resources
     * (i.e. we are in a resource change notification).
     */
    public void runAsWorkspaceJob() {
        if (!isEmpty()) {
            WorkspaceJob job = new WorkspaceJob("Updating problem markers") {
                @Override
                public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                    addRemoveMarkers();
                    return Status.OK_STATUS;
                }
            };

            // the job should not run together with other jobs accessing the workspace resources,
            // to prevent subtle deadlocks (for example: progressMonitor does Display.readAndDispatch
            // which invokes a timerExec, whose run() contains validateAllNedFiles, etc...)
            job.setRule(ResourcesPlugin.getWorkspace().getRoot());
            job.setPriority(Job.INTERACTIVE);
            job.setSystem(true);
            job.schedule();
        }
    }

    protected void addRemoveMarkers() throws CoreException {

        long startTime = System.currentTimeMillis();

        // process each file registered
        for (IResource file : markerTable.keySet()) {
            if (file.exists()) {
                // idea: compare the two sets of markers (old, existing).
                // Markers are basically an attribute map (plus a type string),
                // so we have sets of the attribute maps. Plus, we need a mapping
                // back from the attribute set to the original IMarker (or MarkerData),
                // hence the two maps of maps below.

                // query existing markers
                Map<MarkerData,IMarker> existingMarkers = new HashMap<MarkerData, IMarker>();
                IMarker[] tmp = file.findMarkers(markerBaseType, true, 0);
                for (IMarker marker : tmp)
                    if (isManagedByThisSynchronizer(marker))
                        existingMarkers.put(makeMarkerData(marker), marker);

                // new markers
                List<MarkerData> list = markerTable.get(file);
                Set<MarkerData> newMarkers = new HashSet<MarkerData>();
                for (MarkerData markerData : list)
                    newMarkers.add(markerData);

                // if changed, synchronize (this "if" is not strictly needed, but improves
                // performance of the most common case ("no change")).
                //
                if (!newMarkers.equals(existingMarkers.keySet())) {

                    // add markers that aren't on IResource yet.
                    for (MarkerData newMarker : newMarkers)
                        if (!existingMarkers.keySet().contains(newMarker))
                            createMarker(file, newMarker);

                    // remove IResource markers which aren't in our table
                    for (MarkerData oldMarker : existingMarkers.keySet())
                        if (!newMarkers.contains(oldMarker))
                            {existingMarkers.get(oldMarker).delete(); markersRemoved++;}
                }
            }
        }

        // debug
        if (debug) {
            long millis = System.currentTimeMillis() - startTime;
            if (markersAdded==0 && markersRemoved==0)
                Debug.println("markerSychronizer: no marker change, took " + millis + "ms");
            else
                Debug.println("markerSychronizer: added "+markersAdded+", removed "+markersRemoved+" markers, took "+millis + "ms");
        }
    }

    /**
     * Decides whether the given marker falls under the authority of this marker synchronizer.
     * If this method returns false for a marker, the synchronizer will completely ignore that marker.
     * By default this method returns true, which means that the only filter will be the baseMarkerType
     * passed to the constructor. Override e.g. if you have markers coming from different sources
     * (and hence managed by different synchronizers) on the same file.
     */
    protected boolean isManagedByThisSynchronizer(IMarker marker) throws CoreException {
        return true;  // by default, this is decided solely by marker type, which is already observed by the IResource.findMarkers() method
    }

    protected MarkerData makeMarkerData(IMarker marker) throws CoreException {
        MarkerData markerData = new MarkerData(marker.getType(), new HashMap<String, Object>(marker.getAttributes()));
        return markerData;
    }

    protected void createMarker(IResource file, MarkerData markerData) throws CoreException {
        IMarker marker = file.createMarker(markerData.type);
        marker.setAttributes(markerData.attrs);
        Assert.isTrue(isManagedByThisSynchronizer(marker)); // we must only create markers that are compatible with this synchronizer
        markersAdded++;
    }
}
