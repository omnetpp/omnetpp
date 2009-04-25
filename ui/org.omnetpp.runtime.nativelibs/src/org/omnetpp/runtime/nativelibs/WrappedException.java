/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.runtime.nativelibs;

import org.omnetpp.runtime.nativelibs.simkernel.cException;

/**
 * Wraps a C++ cException.
 * @author Andras
 */
public class WrappedException extends RuntimeException {
	private static final long serialVersionUID = 1L;
	private cException exception;

	public WrappedException(long cPtr) {
		exception = new cException(cPtr);
	}

	public cException getException() {
		return exception;
	}

	@Override
	public String getMessage() {
		return exception.what();
	}

}
