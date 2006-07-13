package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.simulation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class DeleteModuleAnimation extends AbstractAnimationPrimitive {
	private IRuntimeModule module;

	public DeleteModuleAnimation(ReplayAnimationController animationController,
								 long eventNumber,
								 double simulationTime,
								 long animationNumber,
								 IRuntimeModule module) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.module = module;
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
	}
}
