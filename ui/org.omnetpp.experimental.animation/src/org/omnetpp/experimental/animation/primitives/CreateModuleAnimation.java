package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.figures.ModuleFigure;
import org.omnetpp.figures.SubmoduleFigure;

public class CreateModuleAnimation extends AbstractAnimationPrimitive {
	protected ReplayModule module;
	
	protected String parentModulePath;

	public CreateModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double simulationTime,
								 long animationNumber,
								 ReplayModule module,
								 String parentModulePath) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
		this.parentModulePath = parentModulePath;
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
		SubmoduleFigure figure = new SubmoduleFigure();
		getCompoundModuleFigure().addSubmodule(figure);
		animationEnvironment.setFigure(module, figure);
		figure.setName(getReplayModule().getFullName());
		
		if (getParentModule() != null)
			getParentModule().addSubmodule(getReplayModule());

		getReplaySimulation().addModule(getReplayModule());
	}

	public void undo() {
		removeFigure((ModuleFigure)animationEnvironment.getFigure(module));
		animationEnvironment.setFigure(module, null);

		if (getParentModule() != null)
			getParentModule().removeSubmodule(getReplayModule());

		getReplaySimulation().removeModule(module.getId());
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}

	protected ReplaySimulation getReplaySimulation() {
		return (ReplaySimulation)animationEnvironment.getSimulation();
	}
	
	protected ReplayModule getReplayModule() {
		return (ReplayModule)module;
	}

	protected ReplayModule getParentModule() {
		return getReplaySimulation().getModuleByPath(parentModulePath);
	}
}
