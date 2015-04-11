/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

public interface IProblemDecorationSupport {
    // the following constants have been copied here from IMarker so that we don't
    // need to pull in org.eclipse.core.resources for our simulation runtime;
    // they must be kept in sync with IMarker values.
    public static final int SEVERITY_ERROR = 2;
    public static final int SEVERITY_WARNING = 1;
    public static final int SEVERITY_INFO = 0;

    /**
     * Display a "problem" image decoration on the submodule.
     * @param maxSeverity  any of the IMarker.SEVERITY_xxx constants, or -1 for none
     * @param textProvider callback to get the text to be displayed as a tooltip on hover event
     */
    void setProblemDecoration(int maxSeverity, ITooltipTextProvider textProvider);
}
