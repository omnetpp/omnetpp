package org.omnetpp.experimental.animation.replay.model;

import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

public class ReplaySimulation implements IRuntimeSimulation {
	private ReplayModule rootModule;
	
	public ReplaySimulation(ReplayModule rootModule) {
		this.rootModule = rootModule;
	}
	
	public ReplayModule getRootModule() {
		return rootModule;
	}

	public ReplayModule getModuleByID(int id) {
		// TODO: this is inefficient and wrong
		return rootModule.getSubmoduleByID(id);
	}

	public ReplayModule getModuleByPath(String fullPath) {
		// TODO: well this is clearly wrong
		return rootModule;
	}
}
