package org.omnetpp.ned.model;



/**
 * Interface for issuing NED error messages
 * 
 * @author andras
 */
public interface INEDErrorStore {
	
	public void addError(INEDElement context, String message);
	
	public void addError(INEDElement context, int line, String message);
	
	public void addWarning(INEDElement context, String message);
	
	public void addWarning(INEDElement context, int line, String message);
	
	public void add(int severity, INEDElement context, int line, String message);
	
	public int getNumProblems();
}
