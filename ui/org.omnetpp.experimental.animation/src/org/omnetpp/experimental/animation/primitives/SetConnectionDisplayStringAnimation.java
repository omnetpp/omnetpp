package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class SetConnectionDisplayStringAnimation extends AbstractAnimationPrimitive {
	private DisplayString displayString;
	
	public SetConnectionDisplayStringAnimation(ReplayAnimationController animationController,
									 long eventNumber,
									 double simulationTime,
									 long animationNumber,
									 String displayString) {
		super(animationController, eventNumber, simulationTime, animationNumber);
		this.displayString = new DisplayString(null, null, displayString);
		//System.out.println(displayString);
	}
	
	public void animateAt(long eventNumber, double simulationTime, long animationNumber, double animationTime) {
		if (simulationTime >= beginSimulationTime) {
		}
		else {
			// TODO: or what to do?
		}
	}
}
