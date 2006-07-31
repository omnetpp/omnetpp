package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;
import org.omnetpp.figures.SubmoduleFigure;

public class SetModuleDisplayStringAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;
	
	private IDisplayString displayString;
	
	public SetModuleDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 IRuntimeModule module,
									 IDisplayString displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
		this.displayString = displayString;
		//System.out.println(displayString);
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
		SubmoduleFigure moduleFigure = (SubmoduleFigure)animationEnvironment.getFigure(module);
		moduleFigure.setDisplayString(displayString);
	}

	public void undo() {
		// TODO: not enough info
	}

	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		// void
	}
}
