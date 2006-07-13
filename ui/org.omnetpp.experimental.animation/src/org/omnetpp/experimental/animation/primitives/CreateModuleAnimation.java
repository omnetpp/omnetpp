package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private SubmoduleFigure moduleFigure;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double simulationTime,
								 long animationNumber,
								 IRuntimeModule module) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
		this.moduleFigure = new SubmoduleFigure();
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (simulationTime >= beginSimulationTime) {
			animationEnvironment.setFigure(module, moduleFigure);
			showFigure(moduleFigure);
		}
		else {
			hideFigure(moduleFigure);
		}
	}
}
