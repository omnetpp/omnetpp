/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.simulation.model;

/**
 *  
 * @author andras
 */
public interface IRuntimeSimulation {
	public IRuntimeModule getRootModule();
	public IRuntimeModule getModuleByPath(String fullPath);
	public IRuntimeModule getModuleByID(int id);
}
