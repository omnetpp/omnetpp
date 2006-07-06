package org.omnetpp.experimental.animation.model;

public class RuntimeSimulation implements IRuntimeSimulation {
	private RuntimeModule rootModule;
	
	public IRuntimeModule getRootModule() {
		return rootModule;
	}

	public IRuntimeModule getModuleByID(String fullPath) {
		return null;
	}

	public IRuntimeModule getModuleByPath(String fullPath) {
		return null;
	}

}
