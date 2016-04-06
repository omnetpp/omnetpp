/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

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
    /**
     * Marker attribute; stores the Id or Ids (as Long or long[]) of the element the marker refers to,
     * and the Id of the element's original (INedElement.getOriginal())
     */
    public static final String NEDELEMENT_ID = "nedelement-id";

    private IFile file;
    private String markerType;
    private ProblemMarkerSynchronizer markerSync;
    private int problemsAdded = 0;
    private Set<INedElement> affectedElements = new HashSet<INedElement>();


    public NedMarkerErrorStore(ProblemMarkerSynchronizer markerSync) {
        this(markerSync, markerSync.getBaseMarkerType());
    }

    public NedMarkerErrorStore(ProblemMarkerSynchronizer markerSync, String markerType) {
        this.markerType = markerType;
        this.markerSync = markerSync;
    }

    public void setFile(IFile file) {
        this.file = file;
        markerSync.register(file);
    }

    public void add(int severity, INedElement context, int line, String message) {
        Assert.isNotNull(file, "setFile() needs to be called first");
        Assert.isNotNull(context);
        problemsAdded++;

        // create would-be marker
        Map<String, Object> markerAttrs = new HashMap<String, Object>();
        markerAttrs.put(IMarker.MESSAGE, message);
        markerAttrs.put(IMarker.SEVERITY, severity);
        markerAttrs.put(IMarker.LINE_NUMBER, line);
        markerAttrs.put(NEDELEMENT_ID, context.getId());
        markerSync.addMarker(file, markerType, markerAttrs);

        if (markerType.equals(INedTypeResolver.NEDSYNTAXPROBLEM_MARKERID))
            context.syntaxProblemMarkerAdded(severity);
        else if (markerType.equals(INedTypeResolver.NEDCONSISTENCYPROBLEM_MARKERID))
            context.consistencyProblemMarkerAdded(severity);
        else
            throw new IllegalArgumentException(); // wrong marker type
        affectedElements.add(context);
    }

    public int getNumProblems() {
        return problemsAdded;
    }

    public Set<INedElement> getAffectedElements() {
        return affectedElements;
    }

}
