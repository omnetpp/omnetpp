package org.omnetpp.experimental.animation.replay.model;

import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

public class RuntimeSimulation implements IRuntimeSimulation {
	private RuntimeModule rootModule;
	
	public RuntimeSimulation(RuntimeModule rootModule) {
		this.rootModule = rootModule;
	}
	
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
