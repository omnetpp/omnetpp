package org.omnetpp.ned.model;

import org.eclipse.core.resources.IMarker;


/**
 * Base class for implementing INEDErrorStore's. All "add...()" methods
 * delegate to a single "add()" method.
 * 
 * @author Andras
 */
public abstract class AbstractNedErrorStore implements INEDErrorStore {
	
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
}
