package org.omnetpp.common.markers;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;

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
		String type;
		Map<String, Object> attrs;
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

		MarkerData markerData = new MarkerData();
		markerData.type = markerType;
		markerData.attrs = markerAttrs;
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
            if (!debug) 
                job.setSystem(true);
            job.schedule();
        }
    }

	@SuppressWarnings("unchecked")
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
			    Map<Map,IMarker> existingMarkerAttrs = new HashMap<Map, IMarker>();
			    IMarker[] tmp = file.findMarkers(markerBaseType, true, 0);
			    for (IMarker marker : tmp) {
			        // must convert to HashMap, because MarkerAttributesMap calculates hashCode differently
			        Map attrs = new HashMap();
			        attrs.putAll(marker.getAttributes()); 
			        existingMarkerAttrs.put(attrs, marker);
			    }

			    // new markers
			    List<MarkerData> list = markerTable.get(file);
			    Map<Map, MarkerData> newMarkerAttrs = new HashMap<Map, MarkerData>();
			    for (MarkerData markerData : list)
			        newMarkerAttrs.put(markerData.attrs, markerData);
			        
				// add markers that aren't on IResource yet.
			    // note: the "..or types not equal" condition adds about 30% to the runtime cost;
			    // this can be reduced by eliminating double lookups (results in slightly uglier code)
				for (Map attrs : newMarkerAttrs.keySet())
					if (!existingMarkerAttrs.keySet().contains(attrs) || !existingMarkerAttrs.get(attrs).getType().equals(newMarkerAttrs.get(attrs).type))
						createMarker(file, newMarkerAttrs.get(attrs));

				// remove IResource markers which aren't in our table
                for (Map attrs : existingMarkerAttrs.keySet())
                    if (!newMarkerAttrs.keySet().contains(attrs) || !existingMarkerAttrs.get(attrs).getType().equals(newMarkerAttrs.get(attrs).type))
						{existingMarkerAttrs.get(attrs).delete(); markersRemoved++;}
			}
		}

		// debug
		if (debug) {
	        long millis = System.currentTimeMillis() - startTime;
		    if (markersAdded==0 && markersRemoved==0)
		        System.out.println("markerSychronizer: no marker change, took " + millis + "ms");
		    else
		        System.out.println("markerSychronizer: added "+markersAdded+", removed "+markersRemoved+" markers, took "+millis + "ms");
		}
	}

	protected void createMarker(IResource file, MarkerData markerData) throws CoreException {
		IMarker marker = file.createMarker(markerData.type);
		marker.setAttributes(markerData.attrs);
		markersAdded++;
	}
}
