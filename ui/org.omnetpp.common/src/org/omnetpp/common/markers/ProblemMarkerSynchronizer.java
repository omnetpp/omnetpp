package org.omnetpp.common.markers;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
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
 * synchronizes this table to the actual IFile markers. This way,
 * only *real* marker changes reach the workspace, and excessive
 * updates are prevented.
 *
 * @author Andras
 */
public class ProblemMarkerSynchronizer {
    protected static class MarkerData {
		String type;
		Map<String, Object> attrs;
	}

	// data for markers to synchronize
	protected HashMap<IFile, List<MarkerData>> markerTable = new HashMap<IFile, List<MarkerData>>();
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
	 * Include the given file in the synchronization process. This is not needed when
	 * you call addMarker() for the file; however if there's no addMarker() for that file,
	 * that file will be ignored (existing markers left untouched) unless you register them
	 * with registerFile().
	 */
	public void registerFile(IFile file) {
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
	 * Stores data for a marker to be added to the given file. Implies registerFile().
	 */
	public void addMarker(IFile file, String markerType, Map<String, Object> markerAttrs) {
		registerFile(file);

		MarkerData markerData = new MarkerData();
		markerData.type = markerType;
		markerData.attrs = markerAttrs;
		markerTable.get(file).add(markerData);
	}

	public String getBaseMarkerType() {
		return markerBaseType;
	}

	/**
	 * Returns the number of markers registered in this synchronizer.
	 */
	public int getNumberOfFiles() {
		return markerTable.size();
	}

	/**
	 * Returns the number of markers registered in this synchronizer.
	 */
	public int getNumberOfMarkers() {
		int count = 0;
		for (IFile file : markerTable.keySet())
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
	    // process each file registered
		for (IFile file : markerTable.keySet()) {
			if (file.exists()) {
				List<MarkerData> list = markerTable.get(file);

				// add markers that aren't on IFile yet
				for (MarkerData markerData : list)
					if (!fileContainsMarker(file, markerData))
						createMarker(file, markerData);

				// remove IFile markers which aren't in our table
				IMarker[] markers = file.findMarkers(markerBaseType, true, 0);
				for (IMarker marker : markers)
					if (!listContainsMarker(list, marker))
						{marker.delete(); markersRemoved++;}
			}
		}

		// debug
//		if (markersAdded==0 && markersRemoved==0)
//			System.out.println("markerSychronizer: no marker change");
//		else
//			System.out.println("markerSychronizer: added "+markersAdded+", removed "+markersRemoved+" markers");
	}

	protected boolean fileContainsMarker(IFile file, MarkerData markerData) throws CoreException {
		IMarker[] markers = file.findMarkers(markerData.type, false, 0);
		for (IMarker marker : markers)
			if (markerAttributesAreEqual(marker.getAttributes(), markerData))
				return true;
		return false;
	}

	@SuppressWarnings("unchecked")
	protected boolean listContainsMarker(List<MarkerData> list, IMarker marker) throws CoreException {
		Map markerAttributes = marker.getAttributes();
		for (MarkerData markerData : list)
			if (markerAttributesAreEqual(markerAttributes, markerData))
				return true;
		return false;
	}

	@SuppressWarnings("unchecked")
	protected boolean markerAttributesAreEqual(Map markerAttributes, MarkerData markerData) {
		return markerAttributes.equals(markerData.attrs);
	}

	protected void createMarker(IFile file, MarkerData markerData) throws CoreException {
		IMarker marker = file.createMarker(markerData.type);
		marker.setAttributes(markerData.attrs);
		markersAdded++;
	}
}
