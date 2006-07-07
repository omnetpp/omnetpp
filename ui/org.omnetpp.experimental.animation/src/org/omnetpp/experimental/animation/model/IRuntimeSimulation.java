package org.omnetpp.experimental.animation.model;

/**
 *  
 * @author andras
 */
public interface IRuntimeSimulation {
	public IRuntimeModule getRootModule();
	public IRuntimeModule getModuleByPath(String fullPath);
	public IRuntimeModule getModuleByID(int id);
}
