/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetModuleDisplayStringAnimation extends AbstractInfiniteAnimation {
	protected int moduleId;
	
	protected IDisplayString displayString;
	
	protected IDisplayString oldDisplayString; // FIXME: this is a temporary hack to be able to undo changes
	
	public SetModuleDisplayStringAnimation(ReplayAnimationController animationController,
										   AnimationPosition animationPosition,
										   int moduleId,
										   IDisplayString displayString) {
		super(animationController, animationPosition);
		this.moduleId = moduleId;
		this.displayString = displayString;
		//System.out.println("SetModuleDisplayStringAnimation: "+displayString);
	}
	
	@Override
	public void redo() {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			submoduleFigure.setSubmoduleVectorIndex(module.getName(), module.getVectorSize(), module.getIndex());
			oldDisplayString = null; //FIXME: submoduleFigure.getLastDisplayString();
			submoduleFigure.setDisplayString(1.0f, displayString);  //FIXME scale
		}
	}

	@Override
	public void undo() {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			moduleFigure.setDisplayString(1.0f, oldDisplayString); // FIXME: this is a temporary hack to be able to undo changes
		}
	}
}
