package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.ModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	public CreateModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double simulationTime,
								 long animationNumber,
								 IRuntimeModule module) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
	}
	
	@Override
	public double getEndSimulationTime() {
		return Double.MAX_VALUE;
	}
	
	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
	
	public void redo() {
		animationEnvironment.setFigure(module, addFigure(new SubmoduleFigure()));
	}

	public void undo() {
		removeFigure((ModuleFigure)animationEnvironment.getFigure(module));
		animationEnvironment.setFigure(module, null);
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}
