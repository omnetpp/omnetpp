package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetModuleDisplayStringAnimation extends AbstractAnimationPrimitive {
	private int moduleId;
	
	private IDisplayString displayString;
	
	public SetModuleDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 int moduleId,
									 IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.moduleId = moduleId;
		this.displayString = displayString;
		//System.out.println("SetModuleDisplayStringAnimation: "+displayString);
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
		IRuntimeModule module = animationEnvironment.getSimulation().getModuleByID(moduleId);
		if (module.getParentModule() == animationEnvironment.getSimulation().getRootModule()) { //FIXME
			SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
			moduleFigure.setDisplayString(displayString);
		}
	}

	public void undo() {
		// TODO: not enough info
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}
