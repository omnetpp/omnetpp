package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class ScheduleSelfMessageAnimation extends AbstractAnimationPrimitive {
	private double endSimulationTime;	
	
	public ScheduleSelfMessageAnimation(ReplayAnimationController animationController,
										long eventNumber,
										double beginSimulationTime,
										long animationNumber,
										double endSimulationTime) {
		super(animationController, eventNumber, beginSimulationTime, animationNumber);
		this.endSimulationTime = endSimulationTime;
	}
	
	@Override
	public double getEndSimulationTime() {
		return endSimulationTime;
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
	}
}
