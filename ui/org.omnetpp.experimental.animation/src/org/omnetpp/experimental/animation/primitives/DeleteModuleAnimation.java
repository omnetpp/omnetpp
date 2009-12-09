/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class DeleteModuleAnimation extends AbstractAnimationPrimitive {
	protected int moduleId;

	public DeleteModuleAnimation(ReplayAnimationController animationController,
								 AnimationPosition animationPosition,
								 int moduleId) {
		super(animationController, animationPosition);
		this.moduleId = moduleId;
	}

	@Override
	public double getEndSimulationTime() {
		return Double.MAX_VALUE;
	}

	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}

	@Override
	public void redo() {
/*		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == getSimulation().getRootModule()) { //FIXME
			getEnclosingModuleFigure().removeSubmoduleFigure((SubmoduleFigure)animationEnvironment.getFigure(module));
			animationEnvironment.setFigure(module, null);
		}
*/	}

	@Override
	public void undo() {
		// TODO: animationEnvironment.setFigure(module, addFigure(new SubmoduleFigure()));
	}
}
