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
