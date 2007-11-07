package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.ned.model.AbstractNedErrorStore;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;

/**
 * Standard implementation of INEDErrorStore
 *
 * @author Andras
 */
public class NEDMarkerErrorStore extends AbstractNedErrorStore {

    public static final String NEDELEMENT_ID = "nedelement-id";

    private IFile file;
	private String markerType;
    private ProblemMarkerSynchronizer markerSync;
    private int problemsAdded = 0;

    public NEDMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync) {
    	this(file, markerSync, markerSync.getBaseMarkerType());
    }

    public NEDMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync, String markerType) {
    	this.file = file;
    	this.markerType = markerType;
		this.markerSync = markerSync;
		markerSync.register(file);
	}

	public void add(int severity, INEDElement context, int line, String message) {
		Assert.isNotNull(context);
        problemsAdded++;

		// create would-be marker
		Map<String, Object> markerAttrs = new HashMap<String, Object>();
        markerAttrs.put(IMarker.MESSAGE, message);
        markerAttrs.put(IMarker.SEVERITY, severity);
        markerAttrs.put(IMarker.LINE_NUMBER, line);
        markerAttrs.put(NEDELEMENT_ID, (int)context.getId());
        markerSync.addMarker(file, markerType, markerAttrs);

        if (markerType.equals(INEDTypeResolver.NEDSYNTAXPROBLEM_MARKERID))
        	context.syntaxProblemMarkerAdded(severity);
        else if (markerType.equals(INEDTypeResolver.NEDCONSISTENCYPROBLEM_MARKERID))
        	context.consistencyProblemMarkerAdded(severity);
        else
        	throw new IllegalArgumentException(); // wrong marker type
    }

	public int getNumProblems() {
		return problemsAdded;
	}

}
