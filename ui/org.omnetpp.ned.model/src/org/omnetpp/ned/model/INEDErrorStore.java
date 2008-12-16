/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
