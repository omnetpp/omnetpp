/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.replay.model;

import java.util.HashMap;

import org.omnetpp.common.simulation.model.IRuntimeSimulation;

public class ReplaySimulation implements IRuntimeSimulation {
	private ReplayModule rootModule;
	private HashMap<Integer,ReplayModule> idToModuleMap = new HashMap<Integer, ReplayModule>();
	private HashMap<String,ReplayModule> pathToModuleMap = new HashMap<String, ReplayModule>();
	
	public ReplaySimulation(ReplayModule rootModule) {
		this.rootModule = rootModule;
	}

	/* to be called after module creation */
	public void addModule(ReplayModule module) {
		idToModuleMap.put(module.getId(), module);
		pathToModuleMap.put(module.getFullPath(), module);
	}

	public void removeModule(int id) {
		ReplayModule module = getModuleByID(id);
		idToModuleMap.remove(id);
		pathToModuleMap.remove(module.getFullPath());
	}
	
	public ReplayModule getRootModule() {
		return rootModule;
	}

	public ReplayModule getModuleByID(int id) {
		return idToModuleMap.get(id);
	}

	public ReplayModule getModuleByPath(String fullPath) {
		return pathToModuleMap.get(fullPath);
	}
}
