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

	public ReplayModule getModuleByID(String fullPath) {
		return null;
	}

	public ReplayModule getModuleByPath(String fullPath) {
		return null;
	}

}
