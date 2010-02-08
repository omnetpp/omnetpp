/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.ned.model.AbstractNedErrorStore;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;

/**
 * Standard implementation of INedErrorStore
 *
 * @author Andras
 */
public class NedMarkerErrorStore extends AbstractNedErrorStore {

    public static final String NEDELEMENT_ID = "nedelement-id";

    private IFile file;
	private String markerType;
    private ProblemMarkerSynchronizer markerSync;
    private int problemsAdded = 0;

    public NedMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync) {
    	this(file, markerSync, markerSync.getBaseMarkerType());
    }

    public NedMarkerErrorStore(IFile file, ProblemMarkerSynchronizer markerSync, String markerType) {
    	this.file = file;
    	this.markerType = markerType;
		this.markerSync = markerSync;
		markerSync.register(file);
	}

	public void add(int severity, INedElement context, int line, String message) {
		Assert.isNotNull(context);
        problemsAdded++;

		// create would-be marker
		Map<String, Object> markerAttrs = new HashMap<String, Object>();
        markerAttrs.put(IMarker.MESSAGE, message);
        markerAttrs.put(IMarker.SEVERITY, severity);
        markerAttrs.put(IMarker.LINE_NUMBER, line);
        markerAttrs.put(NEDELEMENT_ID, (int)context.getId());
        markerSync.addMarker(file, markerType, markerAttrs);

        if (markerType.equals(INedTypeResolver.NEDSYNTAXPROBLEM_MARKERID))
        	context.syntaxProblemMarkerAdded(severity);
        else if (markerType.equals(INedTypeResolver.NEDCONSISTENCYPROBLEM_MARKERID))
        	context.consistencyProblemMarkerAdded(severity);
        else
        	throw new IllegalArgumentException(); // wrong marker type
    }

	public int getNumProblems() {
		return problemsAdded;
	}

}
