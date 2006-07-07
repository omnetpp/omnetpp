package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class HandleMessageAnimation extends AbstractAnimationPrimitive {
	private int eventNumber;
	
	public HandleMessageAnimation(ReplayAnimationController animationController, double beginSimulationTime, int eventNumber) {
		super(animationController, beginSimulationTime);
		this.eventNumber = eventNumber;
	}
	
	public int getEventNumber() {
		return eventNumber;
	}
	
	public void gotoSimulationTime(double simulationTime) {
		if (beginSimulationTime == simulationTime) {
			show();
		}
		else
			hide();
	}
}
