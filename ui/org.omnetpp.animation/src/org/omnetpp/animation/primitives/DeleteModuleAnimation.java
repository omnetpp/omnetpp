/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogModule;

public class DeleteModuleAnimation extends AbstractAnimationPrimitive {
	protected int moduleId;

	public DeleteModuleAnimation(AnimationController animationEnvironment, int moduleId) {
		super(animationEnvironment);
		this.moduleId = moduleId;
	}

	@Override
	public BigDecimal getEndSimulationTime() {
		return BigDecimal.getPositiveInfinity();
	}

	@Override
	public double getEndAnimationTime() {
		return Double.POSITIVE_INFINITY;
	}

	@Override
	public void activate() {
        super.activate();
  		EventLogModule module = animationController.getAnimationSimulation().getModuleById(moduleId);
		if (module.getParentModule() == getSimulation().getRootModule()) { //FIXME
			// TODO: getEnclosingModuleFigure().removeSubmoduleFigure((SubmoduleFigure)animationEnvironment.getFigure(module));
			animationController.setFigure(module, null);
		}
	}

	@Override
	public void deactivate() {
        super.deactivate();
		// TODO: animationEnvironment.setFigure(module, addFigure(new SubmoduleFigure()));
	}
}
