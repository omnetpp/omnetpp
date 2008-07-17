package org.omnetpp.scave.common;

import java.util.HashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRunnable;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.omnetpp.scave.builder.Activator;

public class ScaveMarkers {
	public static final String MARKERTYPE_SCAVEPROBLEM = "org.omnetpp.scave.builder.scaveproblem";
	private static boolean debug = false;

	public static void deleteMarkers(final IFile file, final String type) {
		try {
			IMarker[] markers = file.findMarkers(type, true, IResource.DEPTH_ZERO);
			if (markers.length == 0)
				return;
		} catch (CoreException e1) {}

		runInWorkspace("delete markers", file, new IWorkspaceRunnable() {
		    public void run(IProgressMonitor monitor) throws CoreException {
		    	file.deleteMarkers(type, true, IResource.DEPTH_ZERO);
		    	monitor.done();
		    	if (debug)
					System.out.println("markers deleted: "+type+" from "+file);
		    }
		});
	}

	public static void setMarker(final IFile file, final String type, final int severity, final String message, final int line) {
		runInWorkspace("set markers", file, new IWorkspaceRunnable() {
		    public void run(IProgressMonitor monitor) throws CoreException {
		    	file.deleteMarkers(type, true, IResource.DEPTH_ZERO);
		    	IMarker marker = file.createMarker(type);
                marker.setAttributes(createMarkerAttributes(severity, message, line));
		    	monitor.done();
		    	if (debug)
					System.out.println("markers deleted: "+type+" from "+file);
		    }
		});
	}


	/**
	 * Utility function to add markers to a file.
	 */
	public static void addMarker(final IFile file, final String type, final int severity, final String message, final int line) {

        // taken from MarkerUtilities see. Eclipse FAQ 304
        runInWorkspace("add marker", file, new IWorkspaceRunnable() {
            public void run(IProgressMonitor monitor) throws CoreException {
                IMarker marker = file.createMarker(type);
                marker.setAttributes(createMarkerAttributes(severity, message, line));
                monitor.done();
                if (debug)
                    System.out.println("marker added: "+type+" on "+file+" line "+line+": "+message);
            }
        });
	}

	@SuppressWarnings("unchecked")
	public static HashMap createMarkerAttributes(int severity, String message, int line) {
        final HashMap map = new HashMap();
        map.put(IMarker.MESSAGE, message);
        map.put(IMarker.SEVERITY, severity);
        if (line > 0)
        	map.put(IMarker.LINE_NUMBER, line);
        return map;
	}

	protected static void runInWorkspace(String operationName, IFile file, final IWorkspaceRunnable operation) {
		if (ResourcesPlugin.getWorkspace().isTreeLocked()) {
			(new WorkspaceJob(operationName) {
				public IStatus runInWorkspace(IProgressMonitor monitor)	throws CoreException {
					operation.run(monitor);
					return Status.OK_STATUS;
				}
			}).schedule();
		}
		else {
			try {
				ResourcesPlugin.getWorkspace().run(operation, null, 0, null);
			} catch (CoreException e) {
				Activator.logError(String.format("Cannot %s, file '%s'", operationName, file), e);
			}
		}
	}
}
