package org.omnetpp.scave.common;

import java.util.HashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspaceRunnable;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.scave.builder.Activator;

public class ScaveMarkers {
	public static final String MARKERTYPE_SCAVEPROBLEM = "org.omnetpp.scave.builder.scaveproblem";

	/**
	 * Utility function to add markers to a file.
	 */
	@SuppressWarnings("unchecked")
	public static void addMarker(final IFile file, final String type, int severity, String message, int line) {

        // taken from MarkerUtilities see. Eclipse FAQ 304
        final HashMap map = new HashMap();
        map.put(IMarker.MESSAGE, message);
        map.put(IMarker.SEVERITY, severity);
        if (line > 0)
        	map.put(IMarker.LINE_NUMBER, line);

        IWorkspaceRunnable r = new IWorkspaceRunnable() {
            public void run(IProgressMonitor monitor) throws CoreException {
                IMarker marker = file.createMarker(type);
                marker.setAttributes(map);
            }
        };

        try {
			file.getWorkspace().run(r, null, 0, null);
			System.out.println("marker added: "+type+" on "+file+" line "+line+": "+message);
		} catch (CoreException e) {
			Activator.logError("cannot add marker", e);
		}
	}
}
