package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private SubmoduleFigure moduleFigure;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double beginSimulationTime,
								 IRuntimeModule module) {
		super(animationController, eventNumber, beginSimulationTime);
		this.module = module;
		this.moduleFigure = new SubmoduleFigure();
	}
	
	public void animateAt(long eventNumber, double simulationTime) {
		if (simulationTime >= beginSimulationTime) {
			animationController.setFigure(module, moduleFigure);
			showFigure(moduleFigure);
		}
		else {
			hideFigure(moduleFigure);
		}
	}
}
