/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.ned.model.DisplayString;

public class SetModuleDisplayStringAnimation extends AbstractInfiniteAnimation {
	protected int moduleId;

	protected IDisplayString displayString;

	protected IDisplayString oldDisplayString;

	public SetModuleDisplayStringAnimation(AnimationController animationController, long eventNumber, BigDecimal simulationTime, int moduleId, IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime);
		this.moduleId = moduleId;
		this.displayString = displayString;
	}

	@Override
	public void activate() {
        super.activate();
		ModuleModel module = animationController.getSimulation().getModuleById(moduleId);
        SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
		if (submoduleFigure != null) {
			oldDisplayString = module.getDisplayString();
            module.setDisplayString(displayString);
			submoduleFigure.setDisplayString(1.0f, displayString, null/*TODO*/);
			Assert.isTrue(submoduleFigure.isVisible());
		}
	}

	@Override
	public void deactivate() {
        super.deactivate();
		ModuleModel module = animationController.getSimulation().getModuleById(moduleId);
        SubmoduleFigure submoduleFigure = (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
		if (submoduleFigure != null) {
		    module.setDisplayString(oldDisplayString);
			submoduleFigure.setDisplayString(1.0f, oldDisplayString == null ? new DisplayString("") : oldDisplayString, null/*TODO*/);
		}
	}
}
