/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.replay.modelchange;

import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;

public class ModuleCreation implements IRuntimeModelChange {
	private ReplaySimulation simulation;
	private int id;
	private String moduleTypeName;
	private String parentFullPath;
	private String moduleName;
	private ReplayModule module;

	public ModuleCreation(ReplaySimulation simulation,
			int id, String moduleTypeName,
			String parentFullPath, String moduleName) {
		this.simulation = simulation;
		this.id = id;
		this.moduleTypeName = moduleTypeName;
		this.parentFullPath = parentFullPath;
		this.moduleName = moduleName;
	}

	public void execute() {
		ReplayModule parent = simulation.getModuleByPath(parentFullPath);
		ReplayModule mod = new ReplayModule(parent);
		mod.setName(moduleName);
		mod.setTypeName(moduleTypeName);
		mod.setId(id);

		this.module = mod;
	}

	public void undo() {
		module.getParentModule().removeSubmodule(module);
		module = null;
		//TODO remove connections
	}

}
