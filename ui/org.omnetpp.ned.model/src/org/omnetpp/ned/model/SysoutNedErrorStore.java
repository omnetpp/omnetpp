package org.omnetpp.ned.model;

/**
 * An error store that writes on the standard error.
 *   
 * @author Andras
 */
public class SysoutNedErrorStore extends AbstractNedErrorStore {
	int count;

	public void add(int severity, INEDElement context, int line, String message) {
		System.err.println("Error/Warning: " + message + " at line " + line + " at " + context);
		count++;
	}

	public int getNumProblems() {
		return count;
	}
}
