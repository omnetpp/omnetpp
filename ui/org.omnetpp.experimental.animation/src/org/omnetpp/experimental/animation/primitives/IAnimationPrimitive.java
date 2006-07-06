package org.omnetpp.experimental.animation.primitives;

public interface IAnimationPrimitive {
	public double getBeginSimulationTime();

	public double getEndSimulationTime();

	public void gotoSimulationTime(double t);
}
