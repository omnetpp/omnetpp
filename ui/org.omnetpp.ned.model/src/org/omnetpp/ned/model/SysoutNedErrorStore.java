/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model;

/**
 * An error store that writes on the standard error.
 *
 * @author Andras
 */
public class SysoutNedErrorStore extends AbstractNedErrorStore {
	int count;

	public void add(int severity, INedElement context, int line, String message) {
		System.err.println("Error/Warning: " + message + " at line " + line + " at " + context);
		count++;
	}

	public int getNumProblems() {
		return count;
	}
}
