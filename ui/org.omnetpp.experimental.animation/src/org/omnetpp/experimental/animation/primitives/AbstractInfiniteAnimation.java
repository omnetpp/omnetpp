package org.omnetpp.experimental.animation.primitives;

import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.replay.ReplayAnimationController;

public class AbstractInfiniteAnimation extends AbstractAnimationPrimitive {
	public AbstractInfiniteAnimation(ReplayAnimationController animationController, AnimationPosition animationPosition) {
		super(animationController, animationPosition);
	}
	
	@Override
	public double getEndSimulationTime() {
		return Double.MAX_VALUE;
	}
	
	@Override
	public double getEndAnimationTime() {
		return Double.MAX_VALUE;
	}
}
