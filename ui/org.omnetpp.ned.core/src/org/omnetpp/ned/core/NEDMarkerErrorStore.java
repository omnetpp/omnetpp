package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.INEDErrorStore;
import org.omnetpp.ned.model.NEDTreeUtil;

/**
 * Standard implementation of INEDErrorStore
 * 
 * @author Andras
 */
public class NEDMarkerErrorStore implements INEDErrorStore {
    public static final String NEDPROBLEM_MARKERID = "org.omnetpp.ned.core.nedproblem";
    public static final String NEDCONSISTENCYPROBLEM_MARKERID = "org.omnetpp.ned.core.nedconsistencyproblem";
    
    public static final String NED_ID = "ned-id";

    private static int nextMarkerNedId = 0;
    
    private IFile file;
	private String markerType;
    private ProblemMarkerSynchronizer markerSync;

    public NEDMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync) {
    	this(file, markerSync, markerSync.getBaseMarkerType());
    }

    public NEDMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync, String markerType) {
    	this.file = file;
    	this.markerType = markerType;
		this.markerSync = markerSync;
		markerSync.registerFile(file);
	}

	public void addError(INEDElement context, String message) {
		add(IMarker.SEVERITY_ERROR, context, getLineOf(context), message);
	}


	public void addError(INEDElement context, int line, String message) {
		add(IMarker.SEVERITY_ERROR, context, line, message);
	}

	public void addWarning(INEDElement context, String message) {
		add(IMarker.SEVERITY_WARNING, context, getLineOf(context), message);
	}

	public void addWarning(INEDElement context, int line, String message) {
		add(IMarker.SEVERITY_WARNING, context, line, message);
	}

	private int getLineOf(INEDElement context) {
		return NEDTreeUtil.getLineFrom(context.getSourceLocation());
	}

	public void add(int severity, INEDElement context, int line, String message) {
		Assert.isNotNull(context);

		// TODO we should generate a unique ID for each marker and add it to the marker as an attribute and also add
		// to the problematic NED file's errorMarkerIds collection. Later we can check whether there is an error
		// on a NEDElement and also get the IDs of those markers (if the user needs them, we can lookup those
		// markers from the file)

		Map<String, Object> markerAttrs = new HashMap<String, Object>();
        markerAttrs.put(IMarker.MESSAGE, message);
        markerAttrs.put(IMarker.SEVERITY, severity);
        markerAttrs.put(IMarker.LINE_NUMBER, line);
        markerAttrs.put(NED_ID, ++nextMarkerNedId);
        markerSync.addMarker(file, markerType, markerAttrs);
        
        context.addMarkerNedId(nextMarkerNedId);
	}

}
