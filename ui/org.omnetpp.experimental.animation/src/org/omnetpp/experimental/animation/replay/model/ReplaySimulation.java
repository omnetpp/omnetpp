package org.omnetpp.experimental.animation.replay.model;

import java.util.ArrayList;
import java.util.HashMap;

import org.omnetpp.experimental.animation.model.IRuntimeSimulation;

public class ReplaySimulation implements IRuntimeSimulation {
	private ReplayModule rootModule;
	private ArrayList<ReplayModule> modules = new ArrayList<ReplayModule>();
	private HashMap<String,ReplayModule> pathToModuleMap = new HashMap<String, ReplayModule>();
	
	public ReplaySimulation(ReplayModule rootModule) {
		this.rootModule = rootModule;
	}

	/* to be called after module creation */
	public void addModule(ReplayModule module) {
		module.setId(modules.size());
		modules.add(module);
		pathToModuleMap.put(module.getFullPath(), module);
	}

	public void removeModule(int id) {
		ReplayModule module = getModuleByID(id);
		modules.set(id, null); // NOT remove(), because it'd shift elements and thus corrupt id-to-module mapping
		pathToModuleMap.remove(module.getFullPath());
	}
	
	public ReplayModule getRootModule() {
		return rootModule;
	}

	public ReplayModule getModuleByID(int id) {
		return modules.get(id);
	}

	public ReplayModule getModuleByPath(String fullPath) {
		return pathToModuleMap.get(fullPath);
	}
}
