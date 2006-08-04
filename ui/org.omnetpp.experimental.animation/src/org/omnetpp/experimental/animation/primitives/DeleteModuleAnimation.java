package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class DeleteModuleAnimation extends AbstractAnimationPrimitive {
	private int moduleId;
	
	public DeleteModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double simulationTime,
								 long animationNumber,
								 int moduleId) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.moduleId = moduleId;
	}
	
	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
	
	public void redo() {
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		getEnclosingModuleFigure().removeSubmoduleFigure((SubmoduleFigure)animationEnvironment.getFigure(module));
		animationEnvironment.setFigure(module, null);
	}

	public void undo() {
		// TODO: animationEnvironment.setFigure(module, addFigure(new SubmoduleFigure()));
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}
