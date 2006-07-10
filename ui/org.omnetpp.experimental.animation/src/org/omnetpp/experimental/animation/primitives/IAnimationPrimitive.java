package org.omnetpp.experimental.animation.primitives;

public interface IAnimationPrimitive {
	public long getEventNumber();

	public double getBeginSimulationTime();

	public double getEndSimulationTime();

	public void animateAt(long eventNumber, double simulationTime);
}
