/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

import java.util.HashMap;

import org.omnetpp.common.eventlog.EventLogModule;

public class AnimationSimulation {
	private EventLogModule rootModule;
	private HashMap<Integer, EventLogModule> idToModuleMap = new HashMap<Integer, EventLogModule>();
	private HashMap<String, EventLogModule> pathToModuleMap = new HashMap<String, EventLogModule>();

	public AnimationSimulation(EventLogModule rootModule) {
		this.rootModule = rootModule;
		addModule(rootModule);
	}

	/* to be called after module creation */
	public void addModule(EventLogModule module) {
		idToModuleMap.put(module.getId(), module);
		pathToModuleMap.put(module.getFullPath(), module);
	}

    public void removeModule(EventLogModule module) {
        removeModule(module.getId());
    }

	public void removeModule(int id) {
		EventLogModule module = getModuleById(id);
		idToModuleMap.remove(id);
		pathToModuleMap.remove(module.getFullPath());
	}

	public EventLogModule getRootModule() {
		return rootModule;
	}

	public EventLogModule getModuleById(int id) {
		return idToModuleMap.get(id);
	}

	public EventLogModule getModuleByPath(String fullPath) {
		return pathToModuleMap.get(fullPath);
	}
}
