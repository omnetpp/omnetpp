package org.omnetpp.experimental.animation.model;

public class RuntimeSimulation implements IRuntimeSimulation {
	private RuntimeModule rootModule;
	
	public RuntimeModule getRootModule() {
		return rootModule;
	}

	public RuntimeModule getModuleByID(String fullPath) {
		return null;
	}

	public RuntimeModule getModuleByPath(String fullPath) {
		return null;
	}

}
