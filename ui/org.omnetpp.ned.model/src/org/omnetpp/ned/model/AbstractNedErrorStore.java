/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

import org.eclipse.core.resources.IMarker;


/**
 * Base class for implementing INedErrorStore's. All "add...()" methods
 * delegate to a common "add()" method.
 *
 * @author Andras
 */
public abstract class AbstractNedErrorStore implements INedErrorStore {

    public void addError(INedElement context, String message) {
        add(IMarker.SEVERITY_ERROR, context, getLineOf(context), message);
    }

    public void addError(INedElement context, int line, String message) {
        add(IMarker.SEVERITY_ERROR, context, line, message);
    }

    public void addWarning(INedElement context, String message) {
        add(IMarker.SEVERITY_WARNING, context, getLineOf(context), message);
    }

    public void addWarning(INedElement context, int line, String message) {
        add(IMarker.SEVERITY_WARNING, context, line, message);
    }

    private int getLineOf(INedElement context) {
        return NedTreeUtil.getLineFrom(context.getSourceLocation());
    }
}
