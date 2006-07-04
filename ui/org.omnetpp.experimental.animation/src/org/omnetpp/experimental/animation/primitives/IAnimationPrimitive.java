package org.omnetpp.experimental.animation.primitives;

public interface IAnimationPrimitive {
	public void gotoSimulationTime(double t);
	
	public void animateSimulationTimeRange(double t0, double t1, double animationTime);
}
